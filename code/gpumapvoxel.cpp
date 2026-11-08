/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "gpumapvoxel.h"

#include "bgfxbackend.h"
#include "dbgprint.h"
#include "gpudraw.h"
#include "gpuvoxel.h"
#include "voxlib.h"

#include <algorithm>
#include <bgfx/embedded_shader.h>
#include "bgfxviews.hh"
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <unordered_map>

#include "shaders/mapdraw/fs_mapvoxel.bin.h"
#include "shaders/mapdraw/vs_mapvoxel.bin.h"


static const bgfx::EmbeddedShader _Shaders[] = {
	BGFX_EMBEDDED_SHADER_D3D11(vs_mapvoxel),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapvoxel),
	BGFX_EMBEDDED_SHADER_END()
};

// The model texture holds two RGBA8 texels a voxel; vs_mapvoxel.sc uses the same width.
static int const MODEL_WIDTH = 2048;
static int const MODEL_ROWS = 2048;
static int const VOXELS_PER_ROW = MODEL_WIDTH / 2;

static bool _Ready = false;
static bool _Failed = false;
static bgfx::ProgramHandle _Program = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _VoxelUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _LightUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _ModelSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _PaletteSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Model = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Palette = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Image = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _Target = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Readback = BGFX_INVALID_HANDLE;

// The palette table the palette texture holds, kept to upload it again only when the game changes it.
static unsigned char _PaletteCopy[MAX_PALETTE_LOOKUP_ENTRIES * VOXEL_PALETTE_SIZE];
static bool _PaletteUploaded = false;

// The place of a layer's voxels in the model texture, counted in voxels from its start.
struct ModelSlot
{
	int First;
	int Count;
};

static std::unordered_map<std::uint64_t, ModelSlot> _Models;
static int _ModelNext = 0;


static bool Start(void)
{
	if (_Ready || _Failed) {
		return(_Ready);
	}
	_Failed = true;
	if (bgfx::getRendererType() != bgfx::RendererType::Direct3D11 || !(bgfx::getCaps()->supported & BGFX_CAPS_VERTEX_ID)) {
		return(false);
	}
	_Program = Create_Embedded_Program(_Shaders, "vs_mapvoxel", _Shaders, "fs_mapvoxel");
	if (!bgfx::isValid(_Program)) {
		DebugString("GPU_VOXEL unavailable: the voxel program did not build\n");
		return(false);
	}
	_VoxelUniform = bgfx::createUniform("u_vox", bgfx::UniformType::Vec4, 4);
	_LightUniform = bgfx::createUniform("u_vlight", bgfx::UniformType::Vec4, 64);
	_ModelSampler = bgfx::createUniform("s_vmodel", bgfx::UniformType::Sampler);
	_PaletteSampler = bgfx::createUniform("s_vpal", bgfx::UniformType::Sampler);
	_Image = bgfx::createTexture2D(GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE, false, 1, bgfx::TextureFormat::R8, BGFX_TEXTURE_RT | BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	_Target = bgfx::isValid(_Image) ? bgfx::createFrameBuffer(1, &_Image, true) : bgfx::FrameBufferHandle{ bgfx::kInvalidHandle };
	_Model = bgfx::createTexture2D(MODEL_WIDTH, MODEL_ROWS, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	_Palette = bgfx::createTexture2D(VOXEL_PALETTE_SIZE, MAX_PALETTE_LOOKUP_ENTRIES, false, 1, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	if (!bgfx::isValid(_Target) || !bgfx::isValid(_Model) || !bgfx::isValid(_Palette)) {
		GpuVoxel_Shutdown();
		return(false);
	}
	_Failed = false;
	_Ready = true;
	return(true);
}


static std::uint64_t Model_Key(unsigned serial, VoxelRenderStruct const & object)
{
	return(((std::uint64_t)serial << 32) | ((std::uint64_t)(unsigned short)object.Layer << 16) | ((std::uint64_t)(unsigned char)object.Info << 8) | (unsigned char)object.AnchorCornerIndex);
}


static int Rows_For(std::size_t voxels)
{
	return((int)((voxels + VOXELS_PER_ROW - 1) / VOXELS_PER_ROW));
}


// Writes a layer's voxels into the model texture after the ones already there. Each layer starts on a
// row, so the upload is whole rows.
static void Upload_Model(std::uint64_t key, std::vector<GpuVoxelInstance> const & voxels)
{
	int const first = _ModelNext;
	int const rows = Rows_For(voxels.size());
	_Models[key] = ModelSlot{ first, (int)voxels.size() };
	if (rows == 0) {
		return;
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)rows * MODEL_WIDTH * 4);
	std::memset(memory->data, 0, memory->size);
	for (std::size_t i = 0; i < voxels.size(); i++) {
		GpuVoxelInstance const & voxel = voxels[i];
		unsigned char * texel = memory->data + i * 8;
		texel[0] = voxel.I;
		texel[1] = voxel.J;
		texel[2] = voxel.K;
		texel[3] = voxel.Colour;
		texel[4] = voxel.Normal;
	}
	bgfx::updateTexture2D(_Model, 0, 0, 0, (uint16_t)(first / VOXELS_PER_ROW), (uint16_t)MODEL_WIDTH, (uint16_t)rows, memory);
	_ModelNext = first + rows * VOXELS_PER_ROW;
}


static void Upload_Palette(void)
{
	if (_PaletteUploaded && std::memcmp(_PaletteCopy, VoxelPaletteTranslateTable, sizeof(_PaletteCopy)) == 0) {
		return;
	}
	std::memcpy(_PaletteCopy, VoxelPaletteTranslateTable, sizeof(_PaletteCopy));
	bgfx::updateTexture2D(_Palette, 0, 0, 0, 0, VOXEL_PALETTE_SIZE, MAX_PALETTE_LOOKUP_ENTRIES, bgfx::copy(_PaletteCopy, sizeof(_PaletteCopy)));
	_PaletteUploaded = true;
}


// One layer of a part as the shader draws it.
struct LayerDraw
{
	VoxelDrawSetup Setup;
	std::uint64_t Key;
	bool Lit;
	bool Reversed;
};

static ModelSlot const * Find_Slot(std::uint64_t key)
{
	auto const found = _Models.find(key);
	return(found != _Models.end() ? &found->second : NULL);
}


// Makes the model texture hold the layers, walking the ones it lacks. A layer set that does not fit what
// is left empties the texture first, and one that does not fit an empty texture is refused.
static bool Load_Models(std::vector<std::vector<LayerDraw>> const & parts)
{
	for (int attempt = 0; attempt < 2; attempt++) {
		std::unordered_map<std::uint64_t, std::vector<GpuVoxelInstance>> fresh;
		int rows = 0;
		for (std::vector<LayerDraw> const & part : parts) {
			for (LayerDraw const & layer : part) {
				if (Find_Slot(layer.Key) == NULL && fresh.find(layer.Key) == fresh.end()) {
					std::vector<GpuVoxelInstance> & voxels = fresh[layer.Key];
					Gpu_Voxel_Walk(layer.Setup.Arg, layer.Reversed, layer.Lit, voxels);
					rows += Rows_For(voxels.size());
				}
			}
		}
		if (_ModelNext / VOXELS_PER_ROW + rows <= MODEL_ROWS) {
			for (auto const & model : fresh) {
				Upload_Model(model.first, model.second);
			}
			return(true);
		}
		if (attempt == 1 || _Models.empty()) {
			return(false);
		}
		_Models.clear();
		_ModelNext = 0;
	}
	return(false);
}


bool GpuVoxel_Pack(GpuDrawList const & list, std::vector<GpuVoxelTile> & tiles)
{
	tiles.assign(list.Voxels.size(), GpuVoxelTile());
	int x = 0;
	int y = 0;
	int shelf = 0;
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		Rect const & bounds = list.Voxels[i].Scene.Region.Bounds;
		if (bounds.Width <= 0 || bounds.Height <= 0) {
			continue;
		}
		if (bounds.Width > GPU_VOXEL_IMAGE_SIZE) {
			return(false);
		}
		if (x + bounds.Width > GPU_VOXEL_IMAGE_SIZE) {
			y += shelf;
			x = 0;
			shelf = 0;
		}
		if (y + bounds.Height > GPU_VOXEL_IMAGE_SIZE) {
			return(false);
		}
		tiles[i].X = x;
		tiles[i].Y = y;
		x += bounds.Width;
		shelf = std::max(shelf, bounds.Height);
	}
	return(true);
}


bool GpuVoxel_Submit(GpuDrawList const & list, std::vector<GpuVoxelTile> const & tiles)
{
	if (!Start()) {
		return(false);
	}
	std::vector<std::vector<LayerDraw>> parts(list.Voxels.size());
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		VoxelScene const & scene = list.Voxels[i].Scene;
		for (VoxelRenderStruct const & object : scene.Objects) {
			LayerDraw layer;
			layer.Setup = object.VoxLib->Setup_Object(object, scene.Center);
			layer.Key = Model_Key(object.VoxLib->Get_Serial(), object);
			layer.Lit = (layer.Setup.Function & 12) == 4;
			layer.Reversed = (layer.Setup.Function & 1) != 0;
			parts[i].push_back(layer);
		}
	}
	if (!Load_Models(parts)) {
		DebugString("GPU_VOXEL: the layers do not fit the model store\n");
		return(false);
	}
	Upload_Palette();

	float projection[16];
	Backend_Build_Ortho_Projection(projection, GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE);
	bgfx::setViewFrameBuffer(VIEW_MAPVOXEL, _Target);
	bgfx::setViewRect(VIEW_MAPVOXEL, 0, 0, GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE);
	bgfx::setViewTransform(VIEW_MAPVOXEL, NULL, projection);
	bgfx::setViewMode(VIEW_MAPVOXEL, bgfx::ViewMode::Sequential);
	bgfx::setViewClear(VIEW_MAPVOXEL, BGFX_CLEAR_COLOR, 0, 0.0f, 0);
	bgfx::touch(VIEW_MAPVOXEL);

	for (std::size_t i = 0; i < parts.size(); i++) {
		GpuVoxelPart const & part = list.Voxels[i];
		Rect const & bounds = part.Scene.Region.Bounds;
		if (bounds.Width <= 0 || bounds.Height <= 0) {
			continue;
		}
		float light[VOXEL_PALETTE_SIZE];
		std::copy(std::begin(part.Lighting), std::end(part.Lighting), light);
		for (LayerDraw const & layer : parts[i]) {
			ModelSlot const * slot = Find_Slot(layer.Key);
			if (slot == NULL || slot->Count == 0) {
				continue;
			}
			Vector3i16 const (&steps)[4] = layer.Setup.Arg.TransformMatrix;
			float const vox[16] = {
				(float)steps[0].I, (float)steps[0].J, (float)steps[1].I, (float)steps[1].J,
				(float)steps[2].I, (float)steps[2].J, (float)steps[3].I, (float)steps[3].J,
				(float)(tiles[i].X - bounds.X), (float)(tiles[i].Y - bounds.Y), layer.Lit ? 1.0f : 0.0f, 0.0f,
				(float)slot->First, 0.0f, 0.0f, 0.0f
			};
			bgfx::setUniform(_VoxelUniform, vox, 4);
			bgfx::setUniform(_LightUniform, light, 64);
			bgfx::setTexture(0, _ModelSampler, _Model);
			bgfx::setTexture(1, _PaletteSampler, _Palette);
			bgfx::setVertexCount((uint32_t)slot->Count * 6);
			bgfx::setScissor((uint16_t)tiles[i].X, (uint16_t)tiles[i].Y, (uint16_t)bounds.Width, (uint16_t)bounds.Height);
			bgfx::setState(BGFX_STATE_WRITE_R);
			bgfx::submit(VIEW_MAPVOXEL, _Program);
		}
	}
	return(true);
}


std::uint16_t GpuVoxel_Image(void)
{
	return(_Image.idx);
}


// What the check compares the voxel image with.
struct PartCheck
{
	char Label[24];
	int Layers;
	int X;
	int Y;
	int Width;
	int Height;
	std::vector<unsigned char> Reference;
};

static std::vector<PartCheck> _Checks;
static std::vector<unsigned char> _ImagePixels;
static std::uint32_t _ReadyFrame = 0;
static int _CheckFrame = 0;
static bool _Pending = false;


std::uint32_t GpuVoxel_Begin_Check(GpuDrawList const & list, std::vector<GpuVoxelTile> const & tiles, int frame)
{
	if (_Pending || !_Ready) {
		return(0);
	}
	_Checks.clear();
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		GpuVoxelPart const & part = list.Voxels[i];
		if (part.Reference.empty()) {
			continue;
		}
		PartCheck check;
		std::memcpy(check.Label, part.Label, sizeof(check.Label));
		check.Layers = (int)part.Scene.Objects.size();
		check.X = tiles[i].X;
		check.Y = tiles[i].Y;
		check.Width = part.Scene.Region.Bounds.Width;
		check.Height = part.Scene.Region.Bounds.Height;
		check.Reference = part.Reference;
		_Checks.push_back(std::move(check));
	}
	if (_Checks.empty()) {
		return(0);
	}
	if (!bgfx::isValid(_Readback)) {
		_Readback = bgfx::createTexture2D(GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE, false, 1, bgfx::TextureFormat::R8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
		_ImagePixels.assign((std::size_t)GPU_VOXEL_IMAGE_SIZE * GPU_VOXEL_IMAGE_SIZE, 0);
	}
	if (!bgfx::isValid(_Readback)) {
		return(0);
	}
	bgfx::blit(VIEW_MAPREAD, _Readback, 0, 0, _Image);
	_ReadyFrame = bgfx::readTexture(_Readback, _ImagePixels.data());
	_CheckFrame = frame;
	_Pending = true;
	return(_ReadyFrame);
}


// The largest step in a 565 channel between two voxel palette colours.
static int Colour_Step(int a, int b)
{
	return(std::max({ std::abs((VoxelRGBColors[a].Red >> 3) - (VoxelRGBColors[b].Red >> 3)),
		std::abs((VoxelRGBColors[a].Green >> 2) - (VoxelRGBColors[b].Green >> 2)),
		std::abs((VoxelRGBColors[a].Blue >> 3) - (VoxelRGBColors[b].Blue >> 3)) }));
}


void GpuVoxel_Compare(std::uint32_t frame)
{
	if (!_Pending || frame < _ReadyFrame) {
		return;
	}
	_Pending = false;
	for (PartCheck const & check : _Checks) {
		int covered = 0;
		int differing = 0;
		int coverage = 0;
		int largest = 0;
		for (int r = 0; r < check.Height; r++) {
			for (int c = 0; c < check.Width; c++) {
				int const software = check.Reference[(std::size_t)r * check.Width + c];
				int const gpu = _ImagePixels[(std::size_t)(check.Y + r) * GPU_VOXEL_IMAGE_SIZE + check.X + c];
				covered += (software != 0 || gpu != 0) ? 1 : 0;
				if (software == gpu) {
					continue;
				}
				differing++;
				if ((software == 0) != (gpu == 0)) {
					coverage++;
				} else {
					largest = std::max(largest, Colour_Step(software, gpu));
				}
			}
		}
		DebugString("GPU_VOXEL frame %d part %s layers %d pixels %d covered %d differing %d coverage %d maxstep %d\n", _CheckFrame, check.Label, check.Layers,
			check.Width * check.Height, covered, differing, coverage, largest);
	}
	_Checks.clear();
}


void GpuVoxel_Shutdown(void)
{
	Release_Handle(_Program);
	Release_Handle(_VoxelUniform);
	Release_Handle(_LightUniform);
	Release_Handle(_ModelSampler);
	Release_Handle(_PaletteSampler);
	Release_Handle(_Model);
	Release_Handle(_Palette);
	Release_Handle(_Readback);
	if (bgfx::isValid(_Target)) {
		Release_Handle(_Target);
	}
	_Image = BGFX_INVALID_HANDLE;
	_Models.clear();
	_ModelNext = 0;
	_PaletteUploaded = false;
	_Pending = false;
	_Ready = false;
	_Failed = false;
}
