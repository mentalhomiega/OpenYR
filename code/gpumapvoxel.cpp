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
#include "convert.h"
#include "emissive.h"
#include "fnv.h"
#include "perfstat.h"
#include "unittype.h"
#include "voxlib.h"

#include <algorithm>
#include <bgfx/embedded_shader.h>
#include "bgfxviews.hh"
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <unordered_map>
#include <utility>

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

// At scale 1 a voxel is a horizontal line one or two pixels long. At a scale above 1 it is the outline of its
// cube, one triangle strip, with the first and last vertices doubled so the strips of successive voxels join.
static int const VERTICES_PER_LINE = 2;
static int const VERTICES_PER_CUBE = 8;

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
static unsigned char _TranslatorCopy[VOXEL_PALETTE_SIZE];
static bool _PaletteUploaded = false;

// The palette texture holds the lighting rows and an identity row, then the same rows through the scratch
// surface's translation: PALETTE_BLOCK rows each.
static int const PALETTE_BLOCK = MAX_PALETTE_LOOKUP_ENTRIES + 1;

// The tiles the voxel image holds, kept between passes, and whether the next pass starts by clearing the image.
static GpuVoxelTileStore _Tiles;
static bool _ClearImage = true;

static void Empty_Image(void)
{
	_Tiles.Empty();
	_ClearImage = true;
}

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
	_Image = bgfx::createTexture2D(GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE, false, 1, bgfx::TextureFormat::RG8, BGFX_TEXTURE_RT | BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	_Target = bgfx::isValid(_Image) ? bgfx::createFrameBuffer(1, &_Image, true) : bgfx::FrameBufferHandle{ bgfx::kInvalidHandle };
	_Model = bgfx::createTexture2D(MODEL_WIDTH, MODEL_ROWS, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	_Palette = bgfx::createTexture2D(VOXEL_PALETTE_SIZE, PALETTE_BLOCK * 2, false, 1, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	if (!bgfx::isValid(_Target) || !bgfx::isValid(_Model) || !bgfx::isValid(_Palette)) {
		GpuVoxel_Shutdown();
		return(false);
	}
	_Failed = false;
	_Ready = true;
	return(true);
}


// The orientation a shadow's walk is stored under, which no object's orientation uses.
static int const SHADOW_WALK = 0xFF;

static std::uint64_t Model_Key(unsigned serial, int layer, int info, int orientation)
{
	return(((std::uint64_t)serial << 32) | ((std::uint64_t)(unsigned short)layer << 16) | ((std::uint64_t)(unsigned char)info << 8) | (unsigned char)orientation);
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


// The scratch surface's translation from a voxel palette index to the index a composite keeps; the identity
// before the scratch drawer exists.
static void Read_Translator(unsigned char (&translator)[VOXEL_PALETTE_SIZE])
{
	unsigned char const * table = EightBitDrawer != NULL ? (unsigned char const *)EightBitDrawer->Get_Translate_Table() : NULL;
	for (int i = 0; i < VOXEL_PALETTE_SIZE; i++) {
		translator[i] = table != NULL ? table[i] : (unsigned char)i;
	}
}


// Returns true when the palette texture changed, so the pictures drawn through the old one are out of date.
static bool Upload_Palette(void)
{
	unsigned char translator[VOXEL_PALETTE_SIZE];
	Read_Translator(translator);
	if (_PaletteUploaded && std::memcmp(_PaletteCopy, VoxelPaletteTranslateTable, sizeof(_PaletteCopy)) == 0 && std::memcmp(_TranslatorCopy, translator, sizeof(translator)) == 0) {
		return(false);
	}
	std::memcpy(_PaletteCopy, VoxelPaletteTranslateTable, sizeof(_PaletteCopy));
	std::memcpy(_TranslatorCopy, translator, sizeof(translator));
	bgfx::Memory const * memory = bgfx::alloc(VOXEL_PALETTE_SIZE * PALETTE_BLOCK * 2);
	for (int block = 0; block < 2; block++) {
		for (int row = 0; row < PALETTE_BLOCK; row++) {
			unsigned char * out = memory->data + (block * PALETTE_BLOCK + row) * VOXEL_PALETTE_SIZE;
			for (int colour = 0; colour < VOXEL_PALETTE_SIZE; colour++) {
				int const index = row < MAX_PALETTE_LOOKUP_ENTRIES ? _PaletteCopy[row * VOXEL_PALETTE_SIZE + colour] : colour;
				out[colour] = block == 0 ? (unsigned char)index : _TranslatorCopy[index];
			}
		}
	}
	bgfx::updateTexture2D(_Palette, 0, 0, 0, 0, VOXEL_PALETTE_SIZE, PALETTE_BLOCK * 2, memory);
	_PaletteUploaded = true;
	return(true);
}


// One layer of a part as the shader draws it.
struct LayerDraw
{
	VoxelDrawSetup Setup;
	std::uint64_t Key;
	bool Lit;
	bool Reversed;
	bool Shadow;
	// The index of the layer's piece in its part.
	int Piece;
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
					if (layer.Shadow) {
						Gpu_Voxel_Walk_Shadow(layer.Setup.Arg, voxels);
					} else {
						Gpu_Voxel_Walk(layer.Setup.Arg, layer.Reversed, layer.Lit, voxels);
					}
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


// The longest tile side a part may have at each step down from the asked-for scale. The first cap lowers no part
// that could fit the image, and the last lowers every part to scale 1.
static int const TILE_CAPS[] = { GPU_VOXEL_IMAGE_SIZE, 1536, 1024, 768, 512, 384, 256, 192, 128, 96, 64, 48, 32, 0 };
static int const CAP_STEPS = (int)std::size(TILE_CAPS);

// The passes in a row that the next larger cap must fit before the parts go back to it, so that a part near the
// limit does not change sharpness from one pass to the next.
static int const CALM_PASSES = 120;

static int _CapStep = 0;
static int _CalmPasses = 0;
static int _ReducedParts = 0;

// The largest scale that divides scale and keeps the part's longer tile side within cap, or 1 when none does.
static int Part_Scale(Rect const & area, int scale, int cap)
{
	int const side = std::max(area.Width, area.Height);
	for (int candidate = scale; candidate > 1; candidate--) {
		if (scale % candidate == 0 && side * candidate <= cap) {
			return(candidate);
		}
	}
	return(1);
}


static bool Fits_Empty_Image(GpuDrawList const & list, int scale, int cap)
{
	GpuVoxelShelf shelf;
	for (GpuVoxelPart const & part : list.Voxels) {
		Rect const & bounds = part.Area;
		int const partscale = Part_Scale(bounds, scale, cap);
		int x = 0;
		int y = 0;
		if (bounds.Width > 0 && bounds.Height > 0 && !shelf.Place(GPU_VOXEL_IMAGE_SIZE, bounds.Width * partscale, bounds.Height * partscale, x, y)) {
			return(false);
		}
	}
	return(true);
}


bool GpuVoxel_Fits(GpuDrawList const & list)
{
	return(Fits_Empty_Image(list, 1, 0));
}


// The cap step for this pass: the step kept from earlier passes, a later one when the parts do not fit an empty image
// under it, or the one before it once that has fitted for CALM_PASSES passes in a row. CAP_STEPS when none fits.
static int Choose_Cap_Step(GpuDrawList const & list, int scale)
{
	int step = _CapStep;
	while (step < CAP_STEPS && !Fits_Empty_Image(list, scale, TILE_CAPS[step])) {
		step++;
	}
	if (step == CAP_STEPS) {
		return(step);
	}
	if (step > _CapStep || step == 0 || !Fits_Empty_Image(list, scale, TILE_CAPS[step - 1])) {
		_CalmPasses = 0;
	} else if (++_CalmPasses >= CALM_PASSES) {
		_CalmPasses = 0;
		step--;
	}
	_CapStep = step;
	return(step);
}


// Adds bytes to a hash eight at a time, because every part of a pass hashes a 256-byte lighting table.
static std::uint64_t Hash_Words(std::uint64_t hash, void const * data, std::size_t length)
{
	unsigned char const * bytes = static_cast<unsigned char const *>(data);
	for (; length >= sizeof(std::uint64_t); bytes += sizeof(std::uint64_t), length -= sizeof(std::uint64_t)) {
		std::uint64_t word;
		std::memcpy(&word, bytes, sizeof(word));
		hash = (hash ^ word) * FNV64_PRIME;
		hash ^= hash >> 32;
	}
	return(Fnv64_Add_Bytes(hash, bytes, length));
}


// A hash of everything the shader's output for a part depends on, apart from the palette texture, whose change empties the image.
static std::uint64_t Part_Signature(GpuVoxelPart const & part, std::vector<LayerDraw> const & layers, int scale)
{
	Rect const & area = part.Area;
	int const head[5] = { scale, area.Width, area.Height, part.Scratch ? 1 : 0, EmissiveDefaults[EMISSIVE_VOXEL_SPECULAR].Strength };
	std::uint64_t hash = Hash_Words(FNV64_BASIS, head, sizeof(head));
	for (GpuVoxelPiece const & piece : part.Pieces) {
		Rect const & bounds = piece.Prepared->Scene.Region.Bounds;
		int const place[6] = { piece.Dest.X - area.X, piece.Dest.Y - area.Y, bounds.X, bounds.Y, bounds.Width, bounds.Height };
		hash = Hash_Words(hash, place, sizeof(place));
		hash = Hash_Words(hash, piece.Prepared->Lighting, sizeof(piece.Prepared->Lighting));
	}
	for (LayerDraw const & layer : layers) {
		int const flags[2] = { (layer.Lit ? 1 : 0) | (layer.Reversed ? 2 : 0) | (layer.Shadow ? 4 : 0), layer.Piece };
		hash = Hash_Words(hash, &layer.Key, sizeof(layer.Key));
		hash = Hash_Words(hash, flags, sizeof(flags));
		hash = Hash_Words(hash, layer.Setup.Arg.TransformMatrix, sizeof(layer.Setup.Arg.TransformMatrix));
	}
	return(hash);
}


// Finds each part's tile at the tile's scale, placing the ones the image lacks; drawn[i] is set for a part that must
// be drawn into its tile. False when the parts do not fit what is left of the image.
static bool Assign_Tiles(GpuDrawList const & list, std::vector<std::vector<LayerDraw>> const & parts, std::vector<GpuVoxelTile> & tiles, std::vector<char> & drawn)
{
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		Rect const & bounds = list.Voxels[i].Area;
		GpuVoxelTile & tile = tiles[i];
		drawn[i] = 0;
		if (bounds.Width <= 0 || bounds.Height <= 0) {
			continue;
		}
		bool fresh = false;
		if (!_Tiles.Place(Part_Signature(list.Voxels[i], parts[i], tile.Scale), GPU_VOXEL_IMAGE_SIZE, bounds.Width * tile.Scale, bounds.Height * tile.Scale, tile.X, tile.Y, fresh)) {
			return(false);
		}
		drawn[i] = fresh ? 1 : 0;
	}
	return(true);
}


// Gives each part its scale and tile. A pass that does not fit what is left of the image empties it and places every
// part afresh, and one that does not fit an empty image either lowers the scale of more parts. False when the parts
// do not fit at scale 1.
static bool Place_Parts(GpuDrawList const & list, std::vector<std::vector<LayerDraw>> const & parts, int scale, std::vector<GpuVoxelTile> & tiles, std::vector<char> & drawn)
{
	for (int step = Choose_Cap_Step(list, scale); step < CAP_STEPS; step++) {
		for (std::size_t i = 0; i < list.Voxels.size(); i++) {
			tiles[i].Scale = Part_Scale(list.Voxels[i].Area, scale, TILE_CAPS[step]);
		}
		for (int attempt = 0; attempt < 2; attempt++) {
			if (Assign_Tiles(list, parts, tiles, drawn)) {
				_CapStep = step;
				return(true);
			}
			Empty_Image();
		}
	}
	return(false);
}


bool GpuVoxel_Submit(GpuDrawList const & list, int scale, std::vector<GpuVoxelTile> & tiles)
{
	if (!Start()) {
		return(false);
	}
	tiles.assign(list.Voxels.size(), GpuVoxelTile());
	std::vector<std::vector<LayerDraw>> parts(list.Voxels.size());
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		std::vector<GpuVoxelPiece> const & pieces = list.Voxels[i].Pieces;
		for (std::size_t p = 0; p < pieces.size(); p++) {
			for (VoxelShadowRenderStruct const & shadow : pieces[p].Prepared->Scene.Shadows) {
				LayerDraw layer;
				layer.Setup = shadow.VoxLib->Setup_Shadow(shadow, pieces[p].Prepared->Scene.Center);
				layer.Key = Model_Key(shadow.VoxLib->Get_Serial(), shadow.Layer, shadow.Info, SHADOW_WALK);
				// A shadow stamps two pixels a column, as a lit voxel does, with no shading.
				layer.Lit = true;
				layer.Reversed = false;
				layer.Shadow = true;
				layer.Piece = (int)p;
				parts[i].push_back(layer);
			}
			for (VoxelRenderStruct const & object : pieces[p].Prepared->Scene.Objects) {
				LayerDraw layer;
				layer.Setup = object.VoxLib->Setup_Object(object, pieces[p].Prepared->Scene.Center);
				layer.Key = Model_Key(object.VoxLib->Get_Serial(), object.Layer, object.Info, object.AnchorCornerIndex);
				layer.Lit = (layer.Setup.Function & 12) == 4;
				layer.Reversed = (layer.Setup.Function & 1) != 0;
				layer.Shadow = false;
				layer.Piece = (int)p;
				parts[i].push_back(layer);
			}
		}
	}
	if (Upload_Palette()) {
		Empty_Image();
	}

	std::vector<char> drawn(list.Voxels.size(), 0);
	if (!Place_Parts(list, parts, scale, tiles, drawn)) {
		return(false);
	}
	std::size_t drawn_parts = 0;
	int reduced = 0;
	for (std::size_t i = 0; i < parts.size(); i++) {
		drawn_parts += drawn[i] != 0 ? 1 : 0;
		reduced += tiles[i].Scale < scale ? 1 : 0;
		if (drawn[i] == 0) {
			parts[i].clear();
		}
	}
	Perf_Count(PERF_COUNT_GPU_VOXEL_PARTS, list.Voxels.size());
	Perf_Count(PERF_COUNT_GPU_VOXEL_DRAWN, drawn_parts);

	if (!Load_Models(parts)) {
		// The tiles just placed were never drawn, so a later pass must not take them for pictures.
		Empty_Image();
		DebugString("GPU_VOXEL: the layers do not fit the model store\n");
		return(false);
	}
	_ReducedParts += reduced;

	float projection[16];
	Backend_Build_Ortho_Projection(projection, GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE);
	bgfx::setViewFrameBuffer(VIEW_MAPVOXEL, _Target);
	bgfx::setViewRect(VIEW_MAPVOXEL, 0, 0, GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE);
	bgfx::setViewTransform(VIEW_MAPVOXEL, NULL, projection);
	bgfx::setViewMode(VIEW_MAPVOXEL, bgfx::ViewMode::Sequential);
	bgfx::setViewClear(VIEW_MAPVOXEL, _ClearImage ? BGFX_CLEAR_COLOR : BGFX_CLEAR_NONE, 0, 0.0f, 0);
	_ClearImage = false;
	bgfx::touch(VIEW_MAPVOXEL);

	for (std::size_t i = 0; i < parts.size(); i++) {
		GpuVoxelPart const & part = list.Voxels[i];
		Rect const & area = part.Area;
		int const tilescale = tiles[i].Scale;
		if (area.Width <= 0 || area.Height <= 0) {
			continue;
		}
		for (LayerDraw const & layer : parts[i]) {
			ModelSlot const * slot = Find_Slot(layer.Key);
			if (slot == NULL || slot->Count == 0) {
				continue;
			}
			GpuVoxelPiece const & piece = part.Pieces[layer.Piece];
			Rect const & bounds = piece.Prepared->Scene.Region.Bounds;
			// Where the piece's pixels land in the tile, and the part of the tile they may cover.
			int const shift_x = tiles[i].X + (piece.Dest.X - area.X - bounds.X) * tilescale;
			int const shift_y = tiles[i].Y + (piece.Dest.Y - area.Y - bounds.Y) * tilescale;
			Rect const clip = Intersect(Rect(piece.Dest.X - area.X, piece.Dest.Y - area.Y, bounds.Width, bounds.Height), Rect(0, 0, area.Width, area.Height));
			if (clip.Width <= 0 || clip.Height <= 0) {
				continue;
			}
			float light[VOXEL_PALETTE_SIZE];
			std::copy(std::begin(piece.Prepared->Lighting), std::end(piece.Prepared->Lighting), light);
			if (layer.Shadow) {
				// The identity row after the lighting rows keeps the shadow's colour 1.
				std::fill(std::begin(light), std::end(light), (float)MAX_PALETTE_LOOKUP_ENTRIES);
			}
			Vector3i16 const (&steps)[4] = layer.Setup.Arg.TransformMatrix;
			float const vox[16] = {
				(float)steps[0].I, (float)steps[0].J, (float)steps[1].I, (float)steps[1].J,
				(float)steps[2].I, (float)steps[2].J, (float)steps[3].I, (float)steps[3].J,
				(float)shift_x, (float)shift_y, layer.Lit ? 1.0f : 0.0f, part.Scratch ? (float)PALETTE_BLOCK : 0.0f,
				(float)slot->First, (float)tilescale, layer.Shadow ? 0.0f : (float)EmissiveDefaults[EMISSIVE_VOXEL_SPECULAR].Strength, 0.0f
			};
			bgfx::setUniform(_VoxelUniform, vox, 4);
			bgfx::setUniform(_LightUniform, light, 64);
			bgfx::setTexture(0, _ModelSampler, _Model);
			bgfx::setTexture(1, _PaletteSampler, _Palette);
			bgfx::setVertexCount((uint32_t)slot->Count * (tilescale > 1 ? VERTICES_PER_CUBE : VERTICES_PER_LINE));
			bgfx::setScissor((uint16_t)(tiles[i].X + clip.X * tilescale), (uint16_t)(tiles[i].Y + clip.Y * tilescale), (uint16_t)(clip.Width * tilescale), (uint16_t)(clip.Height * tilescale));
			bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | (tilescale > 1 ? BGFX_STATE_PT_TRISTRIP : BGFX_STATE_PT_LINES));
			bgfx::submit(VIEW_MAPVOXEL, _Program);
		}
	}
	return(true);
}


int GpuVoxel_Take_Reduced(void)
{
	return(std::exchange(_ReducedParts, 0));
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
	std::vector<unsigned char> MaskReference;
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
		check.Layers = 0;
		for (GpuVoxelPiece const & piece : part.Pieces) {
			check.Layers += (int)(piece.Prepared->Scene.Shadows.size() + piece.Prepared->Scene.Objects.size());
		}
		check.X = tiles[i].X;
		check.Y = tiles[i].Y;
		check.Width = part.Area.Width;
		check.Height = part.Area.Height;
		check.Reference = part.Reference;
		check.MaskReference = part.MaskReference;
		_Checks.push_back(std::move(check));
	}
	if (_Checks.empty()) {
		return(0);
	}
	if (!bgfx::isValid(_Readback)) {
		_Readback = bgfx::createTexture2D(GPU_VOXEL_IMAGE_SIZE, GPU_VOXEL_IMAGE_SIZE, false, 1, bgfx::TextureFormat::RG8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
		_ImagePixels.assign((std::size_t)GPU_VOXEL_IMAGE_SIZE * GPU_VOXEL_IMAGE_SIZE * 2, 0);
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


// Logs how far the highlight strengths the GPU wrote beside a direct part's indices are from the software's mask.
static void Compare_Mask(PartCheck const & check)
{
	int masked = 0;
	int differing = 0;
	int largest = 0;
	for (int r = 0; r < check.Height; r++) {
		for (int c = 0; c < check.Width; c++) {
			int const software = check.MaskReference[(std::size_t)r * check.Width + c];
			int const gpu = _ImagePixels[((std::size_t)(check.Y + r) * GPU_VOXEL_IMAGE_SIZE + check.X + c) * 2 + 1];
			masked += (software != 0 || gpu != 0) ? 1 : 0;
			if (software != gpu) {
				differing++;
				largest = std::max(largest, std::abs(software - gpu));
			}
		}
	}
	DebugString("GPU_VOXEL_MASK frame %d part %s pixels %d masked %d differing %d maxdiff %d\n", _CheckFrame, check.Label, check.Width * check.Height, masked, differing, largest);
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
				int const gpu = _ImagePixels[((std::size_t)(check.Y + r) * GPU_VOXEL_IMAGE_SIZE + check.X + c) * 2];
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
		if (!check.MaskReference.empty()) {
			Compare_Mask(check);
		}
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
	Empty_Image();
	_CapStep = 0;
	_CalmPasses = 0;
	_PaletteUploaded = false;
	_Pending = false;
	_Ready = false;
	_Failed = false;
}
