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
#include <cmath>
#include <bgfx/embedded_shader.h>
#include "bgfxviews.hh"
#include <cfloat>
#include <cstring>
#include <iterator>
#include <unordered_map>
#include <utility>

#include "shaders/mapdraw/fs_voxelscene.bin.h"
#include "shaders/mapdraw/vs_voxelface.bin.h"
#include "shaders/mapdraw/vs_voxelscene.bin.h"


static const bgfx::EmbeddedShader _Shaders[] = {
	BGFX_EMBEDDED_SHADER_D3D11(vs_voxelscene),
	BGFX_EMBEDDED_SHADER_D3D11(vs_voxelface),
	BGFX_EMBEDDED_SHADER_D3D11(fs_voxelscene),
	BGFX_EMBEDDED_SHADER_END()
};

// The model texture holds two RGBA8 texels a voxel; voxelwalk.sh uses the same width.
static int const MODEL_WIDTH = 2048;
static int const MODEL_ROWS = 8192;
static int const VOXELS_PER_ROW = MODEL_WIDTH / 2;

// The face texture holds one RG32U texel a face; vs_voxelface.sc uses the same width.
static int const FACE_WIDTH = 2048;
static int const FACE_ROWS = 2048;

// At scale 1 a voxel is a horizontal line one or two pixels long. At a scale above 1 it is the outline of its
// cube, one triangle strip, with the first and last vertices doubled so the strips of successive voxels join,
// or, for a layer in the face texture, two triangles for each face turned toward the viewer.
static int const VERTICES_PER_LINE = 2;
static int const VERTICES_PER_CUBE = 8;
static int const VERTICES_PER_FACE = 6;

static bool _Ready = false;
static bool _Failed = false;
static bool _UnfitLogged = false;
static bgfx::UniformHandle _VoxelUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _ModelSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _PaletteSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Model = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Palette = BGFX_INVALID_HANDLE;

// The program that draws a layer's walk into the map pass, which reads the model and palette textures at these stages.
static bgfx::ProgramHandle _DirectProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _DepthUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _BlitUniform = BGFX_INVALID_HANDLE;
static uint8_t const DIRECT_MODEL_STAGE = 11;
static uint8_t const DIRECT_PALETTE_STAGE = 12;
static bool _SharedDepth = true;
static bool _CloakRipple = false;

// The program that draws a layer's faces, valid only with the face texture, which it reads at the model's stage.
static bgfx::ProgramHandle _FaceProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _FaceUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _FaceSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _FaceStore = BGFX_INVALID_HANDLE;

// The palette table the palette texture holds, kept to upload it again only when the game changes it.
static unsigned char _PaletteCopy[MAX_PALETTE_LOOKUP_ENTRIES * VOXEL_PALETTE_SIZE];
static unsigned char _TranslatorCopy[VOXEL_PALETTE_SIZE];
static bool _PaletteUploaded = false;

// The palette texture holds the lighting rows and an identity row, then the same rows through the scratch
// surface's translation: PALETTE_BLOCK rows each.
static int const PALETTE_BLOCK = MAX_PALETTE_LOOKUP_ENTRIES + 1;

// The rows after the palette's hold lighting tables, a lighting row for each normal: first the table of a shadow or
// unlit layer, whose every entry is the identity row, then the tables of the pieces drawn lately.
static int const PALETTE_ROWS = 4096;
static int const IDENTITY_LIGHT_ROW = PALETTE_BLOCK * 2;
static int const LIGHT_FIRST = IDENTITY_LIGHT_ROW + 1;
static int const LIGHT_ROWS = PALETTE_ROWS - LIGHT_FIRST;

// The tables in the palette texture's rows from LIGHT_FIRST, in row order, and the rows that hold a table of each hash.
static std::vector<unsigned char> _Lights;
static std::unordered_multimap<std::uint64_t, int> _LightIndex;

// The place of a layer's voxels in the model texture, counted in voxels from its start.
struct ModelSlot
{
	int First;
	int Count;
};

static std::unordered_map<std::uint64_t, ModelSlot> _Models;
static int _ModelNext = 0;
static int const MODEL_VOXELS = VOXELS_PER_ROW * MODEL_ROWS;

// The voxel count of every layer walked so far. A pass whose layers are all measured and cannot fit the model
// texture is refused from these counts, without walking a layer.
static std::unordered_map<std::uint64_t, int> _ModelSizes;

// The passes refused so far, for the log line that says which store was too small.
static int _Refusals = 0;

// The place of a layer's faces in the face texture: its first face, and where each direction's faces start after it.
struct FaceSlot
{
	int First;
	int Start[GPU_VOXEL_DIRECTIONS + 1];
};

static std::unordered_map<std::uint64_t, FaceSlot> _Faces;
static int _FaceNext = 0;


static bool Start(void)
{
	if (_Ready || _Failed) {
		return(_Ready);
	}
	_Failed = true;
	if (bgfx::getRendererType() != bgfx::RendererType::Direct3D11 || !(bgfx::getCaps()->supported & BGFX_CAPS_VERTEX_ID)) {
		return(false);
	}
	_DirectProgram = Create_Embedded_Program(_Shaders, "vs_voxelscene", _Shaders, "fs_voxelscene");
	if (!bgfx::isValid(_DirectProgram)) {
		DebugString("GPU_VOXEL unavailable: the voxel program did not build\n");
		return(false);
	}
	uint64_t const face_flags = BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP;
	if (bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RG32U, face_flags)) {
		_FaceStore = bgfx::createTexture2D(FACE_WIDTH, FACE_ROWS, false, 1, bgfx::TextureFormat::RG32U, face_flags);
		_FaceProgram = Create_Embedded_Program(_Shaders, "vs_voxelface", _Shaders, "fs_voxelscene");
	}
	if (!bgfx::isValid(_FaceStore) || !bgfx::isValid(_FaceProgram)) {
		Release_Handle(_FaceProgram);
		DebugString("GPU_VOXEL: no face store, so layers draw as cube outlines at every scale\n");
	}
	_FaceUniform = bgfx::createUniform("u_vface", bgfx::UniformType::Vec4, 2);
	_FaceSampler = bgfx::createUniform("s_vface", bgfx::UniformType::Sampler);
	_DepthUniform = bgfx::createUniform("u_vdepth", bgfx::UniformType::Vec4, 3);
	_BlitUniform = bgfx::createUniform("u_vblit", bgfx::UniformType::Vec4, 4);
	_VoxelUniform = bgfx::createUniform("u_vox", bgfx::UniformType::Vec4, 4);
	_ModelSampler = bgfx::createUniform("s_vmodel", bgfx::UniformType::Sampler);
	_PaletteSampler = bgfx::createUniform("s_vpal", bgfx::UniformType::Sampler);
	_Model = bgfx::createTexture2D(MODEL_WIDTH, MODEL_ROWS, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	_Palette = bgfx::createTexture2D(VOXEL_PALETTE_SIZE, PALETTE_ROWS, false, 1, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	if (!bgfx::isValid(_Model) || !bgfx::isValid(_Palette)) {
		GpuVoxel_Shutdown();
		_Failed = true;
		DebugString("GPU_VOXEL unavailable: the model or palette texture could not be made\n");
		return(false);
	}
	bgfx::Memory const * identity = bgfx::alloc(VOXEL_PALETTE_SIZE);
	std::memset(identity->data, MAX_PALETTE_LOOKUP_ENTRIES, identity->size);
	bgfx::updateTexture2D(_Palette, 0, 0, 0, IDENTITY_LIGHT_ROW, VOXEL_PALETTE_SIZE, 1, identity);
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


static int Rows_For(std::size_t items, int per_row)
{
	return((int)((items + per_row - 1) / per_row));
}


// Writes voxels into the model texture from the place first, counted in voxels: the rest of a started row, then
// whole rows, then the start of a last row.
static void Upload_Voxels(int first, std::vector<GpuVoxelInstance> const & voxels)
{
	std::size_t done = 0;
	while (done < voxels.size()) {
		int const column = (first + (int)done) % VOXELS_PER_ROW;
		int const row = (first + (int)done) / VOXELS_PER_ROW;
		std::size_t const left = voxels.size() - done;
		int rows = 1;
		std::size_t run = std::min<std::size_t>(left, (std::size_t)(VOXELS_PER_ROW - column));
		if (column == 0 && left >= (std::size_t)VOXELS_PER_ROW) {
			rows = (int)(left / VOXELS_PER_ROW);
			run = (std::size_t)rows * VOXELS_PER_ROW;
		}
		bgfx::Memory const * memory = bgfx::alloc((uint32_t)run * 8);
		std::memset(memory->data, 0, memory->size);
		for (std::size_t i = 0; i < run; i++) {
			GpuVoxelInstance const & voxel = voxels[done + i];
			unsigned char * texel = memory->data + i * 8;
			texel[0] = voxel.I;
			texel[1] = voxel.J;
			texel[2] = voxel.K;
			texel[3] = voxel.Colour;
			texel[4] = voxel.Normal;
		}
		int const width = run >= (std::size_t)VOXELS_PER_ROW ? MODEL_WIDTH : (int)run * 2;
		bgfx::updateTexture2D(_Model, 0, 0, (uint16_t)(column * 2), (uint16_t)row, (uint16_t)width, (uint16_t)rows, memory);
		done += run;
	}
}


// Writes a layer's voxels into the model texture right after the ones already there, so a small layer takes only
// its own voxels and not a whole row.
static void Upload_Model(std::uint64_t key, std::vector<GpuVoxelInstance> const & voxels)
{
	_Models[key] = ModelSlot{ _ModelNext, (int)voxels.size() };
	Upload_Voxels(_ModelNext, voxels);
	_ModelNext += (int)voxels.size();
}


static void Upload_Faces(std::uint64_t key, GpuVoxelFaceList const & list)
{
	FaceSlot slot;
	slot.First = _FaceNext;
	std::copy(std::begin(list.First), std::end(list.First), slot.Start);
	_Faces[key] = slot;
	int const rows = Rows_For(list.Faces.size(), FACE_WIDTH);
	if (rows == 0) {
		return;
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)rows * FACE_WIDTH * 8);
	std::memset(memory->data, 0, memory->size);
	for (std::size_t i = 0; i < list.Faces.size(); i++) {
		GpuVoxelFace const & face = list.Faces[i];
		// The bytes of the texel's two 32-bit channels, lowest first.
		unsigned char * texel = memory->data + i * 8;
		texel[0] = face.X;
		texel[1] = face.Y;
		texel[2] = face.Z;
		texel[3] = face.Direction;
		texel[4] = face.Colour;
		texel[5] = face.Normal;
	}
	bgfx::updateTexture2D(_FaceStore, 0, 0, 0, (uint16_t)(slot.First / FACE_WIDTH), (uint16_t)FACE_WIDTH, (uint16_t)rows, memory);
	_FaceNext = slot.First + rows * FACE_WIDTH;
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


static void Upload_Palette(void)
{
	unsigned char translator[VOXEL_PALETTE_SIZE];
	Read_Translator(translator);
	if (_PaletteUploaded && std::memcmp(_PaletteCopy, VoxelPaletteTranslateTable, sizeof(_PaletteCopy)) == 0 && std::memcmp(_TranslatorCopy, translator, sizeof(translator)) == 0) {
		return;
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
	// The palette texture row that holds the layer's lighting table.
	int LightRow = IDENTITY_LIGHT_ROW;
	// The prepared layer of an object, NULL for a shadow, and its place in the part's depth.
	VoxelRenderStruct const * Object = NULL;
	GpuVoxelLayerDepth Depth;
};

// The layers of each part of the pass GpuVoxel_Submit took last, in the list's order; empty for a part no blit shows.
static std::vector<std::vector<LayerDraw>> _DirectLayers;

static ModelSlot const * Find_Slot(std::uint64_t key)
{
	auto const found = _Models.find(key);
	return(found != _Models.end() ? &found->second : NULL);
}


static void Walk_Layer(LayerDraw const & layer, std::vector<GpuVoxelInstance> & voxels)
{
	if (layer.Shadow) {
		Gpu_Voxel_Walk_Shadow(layer.Setup.Arg, voxels);
	} else {
		Gpu_Voxel_Walk(layer.Setup.Arg, layer.Reversed, layer.Lit, voxels);
	}
}


// Says, for the first refused pass and then one in 600, which store was too small and by how much.
static void Log_Refusal(char const * store, long long needed, long long capacity)
{
	if (_Refusals++ % 600 == 0) {
		DebugString("GPU_VOXEL: the pass needs %lld %s and the store holds %lld, so the software draws it (%d passes refused)\n", needed, store, capacity, _Refusals);
	}
}


// Makes the model texture hold the layers, walking the ones it lacks. A layer set that does not fit what
// is left empties the texture first, and one that does not fit an empty texture is refused. A layer is walked to
// measure it only once, so a pass that is refused again and again costs no walk.
static bool Load_Models(std::vector<std::vector<LayerDraw>> const & parts)
{
	std::unordered_map<std::uint64_t, std::vector<GpuVoxelInstance>> walked;
	for (int attempt = 0; attempt < 2; attempt++) {
		std::unordered_map<std::uint64_t, LayerDraw const *> missing;
		long long needed = 0;
		for (std::vector<LayerDraw> const & part : parts) {
			for (LayerDraw const & layer : part) {
				if (Find_Slot(layer.Key) != NULL || !missing.emplace(layer.Key, &layer).second) {
					continue;
				}
				auto size = _ModelSizes.find(layer.Key);
				if (size == _ModelSizes.end()) {
					std::vector<GpuVoxelInstance> & voxels = walked[layer.Key];
					Walk_Layer(layer, voxels);
					size = _ModelSizes.emplace(layer.Key, (int)voxels.size()).first;
				}
				needed += size->second;
			}
		}
		if (_ModelNext + needed <= MODEL_VOXELS) {
			for (auto const & layer : missing) {
				auto found = walked.find(layer.first);
				if (found == walked.end()) {
					found = walked.emplace(layer.first, std::vector<GpuVoxelInstance>()).first;
					Walk_Layer(*layer.second, found->second);
				}
				Upload_Model(layer.first, found->second);
			}
			return(true);
		}
		if (attempt == 1 || _Models.empty()) {
			Log_Refusal("voxels", needed, MODEL_VOXELS);
			return(false);
		}
		_Models.clear();
		_ModelNext = 0;
	}
	return(false);
}


// True when a part's object layers draw as faces at this scale; otherwise they draw as the model store's walk.
static bool Faces_Wanted(int scale)
{
	return(scale > 1 && _SharedDepth && bgfx::isValid(_FaceProgram));
}


// A layer's faces are the same in every orientation, so the face texture keeps one list for all of them.
static std::uint64_t Face_Key(LayerDraw const & layer)
{
	return(Model_Key(layer.Object->VoxLib->Get_Serial(), layer.Object->Layer, layer.Object->Info, layer.Lit ? 1 : 0));
}


// The layer's place in the face texture, or NULL when it draws as its walk.
static FaceSlot const * Find_Faces(LayerDraw const & layer, int scale)
{
	if (layer.Shadow || !Faces_Wanted(scale)) {
		return(NULL);
	}
	auto const found = _Faces.find(Face_Key(layer));
	return(found != _Faces.end() ? &found->second : NULL);
}


// Makes the face texture hold the object layers of the parts, listing the faces of the ones it lacks. A pass that does
// not fit what is left empties the texture first; one that does not fit an empty texture leaves it empty, and
// Find_Faces then finds none of its layers.
static void Load_Faces(std::vector<std::vector<LayerDraw>> const & parts)
{
	for (int attempt = 0; attempt < 2; attempt++) {
		std::unordered_map<std::uint64_t, GpuVoxelFaceList> fresh;
		int rows = 0;
		for (std::vector<LayerDraw> const & part : parts) {
			for (LayerDraw const & layer : part) {
				if (layer.Shadow) {
					continue;
				}
				std::uint64_t const key = Face_Key(layer);
				if (_Faces.find(key) == _Faces.end() && fresh.find(key) == fresh.end()) {
					GpuVoxelFaceList & list = fresh[key];
					Gpu_Voxel_Faces(layer.Setup.Arg, layer.Lit, list);
					rows += Rows_For(list.Faces.size(), FACE_WIDTH);
				}
			}
		}
		if (_FaceNext / FACE_WIDTH + rows <= FACE_ROWS) {
			for (auto const & faces : fresh) {
				Upload_Faces(faces.first, faces.second);
			}
			return;
		}
		_Faces.clear();
		_FaceNext = 0;
	}
}


void GpuVoxel_Set_Shared_Depth(bool shared)
{
	_SharedDepth = shared;
}


void GpuVoxel_Set_Cloak_Ripple(bool ripple)
{
	_CloakRipple = ripple;
}


bool GpuVoxel_Cloak_Ripple(void)
{
	return(_CloakRipple);
}


static bool Objects_Only(GpuVoxelPart const & part)
{
	return(std::all_of(part.Pieces.begin(), part.Pieces.end(), [](GpuVoxelPiece const & piece) { return(piece.Prepared->Scene.Shadows.empty()); }));
}


static bool Shadows_Only(GpuVoxelPart const & part)
{
	return(!part.Scratch && part.Pieces.size() == 1 && part.Pieces[0].Prepared->Scene.Objects.empty());
}


bool GpuVoxel_Fits(GpuDrawList const & list)
{
	if (list.Voxels.empty()) {
		return(true);
	}
	if (!Start()) {
		return(false);
	}
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Voxel < 0 || list.Voxels[blit.Voxel].Pieces.empty()) {
			continue;
		}
		GpuVoxelPart const & part = list.Voxels[blit.Voxel];
		GpuBlitClass const shown = blit.Class();
		if ((shown == GPU_BLIT_IMAGE && Objects_Only(part)) || (shown == GPU_BLIT_SHADOW && Shadows_Only(part))) {
			continue;
		}
		if (!_UnfitLogged) {
			_UnfitLogged = true;
			DebugString("GPU_VOXEL: a voxel part is shown in a way the GPU does not draw, so the software draws the pass\n");
		}
		return(false);
	}
	_UnfitLogged = false;
	return(true);
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


static int Find_Light(std::uint64_t hash, unsigned char const * table)
{
	auto const range = _LightIndex.equal_range(hash);
	for (auto found = range.first; found != range.second; ++found) {
		if (std::memcmp(&_Lights[(std::size_t)found->second * VOXEL_PALETTE_SIZE], table, VOXEL_PALETTE_SIZE) == 0) {
			return(found->second);
		}
	}
	return(-1);
}


// Gives each lit object layer the palette texture row of its piece's lighting table, adding the tables the texture
// lacks. Tables that do not fit the rows left empty the rows first; false when they do not fit empty rows either,
// which leaves the rows empty.
static bool Load_Lights(GpuDrawList const & list, std::vector<std::vector<LayerDraw>> & parts)
{
	for (int attempt = 0; attempt < 2; attempt++) {
		int const kept = (int)(_Lights.size() / VOXEL_PALETTE_SIZE);
		bool fits = true;
		for (std::size_t i = 0; i < parts.size() && fits; i++) {
			int piece = -1;
			int row = IDENTITY_LIGHT_ROW;
			for (LayerDraw & layer : parts[i]) {
				if (layer.Shadow || !layer.Lit) {
					continue;
				}
				if (layer.Piece != piece) {
					unsigned char const * table = list.Voxels[i].Pieces[layer.Piece].Prepared->Lighting;
					std::uint64_t const hash = Hash_Words(FNV64_BASIS, table, VOXEL_PALETTE_SIZE);
					int index = Find_Light(hash, table);
					if (index < 0) {
						index = (int)(_Lights.size() / VOXEL_PALETTE_SIZE);
						if (index == LIGHT_ROWS) {
							fits = false;
							break;
						}
						_Lights.insert(_Lights.end(), table, table + VOXEL_PALETTE_SIZE);
						_LightIndex.emplace(hash, index);
					}
					piece = layer.Piece;
					row = LIGHT_FIRST + index;
				}
				layer.LightRow = row;
			}
		}
		if (fits) {
			int const added = (int)(_Lights.size() / VOXEL_PALETTE_SIZE) - kept;
			if (added > 0) {
				bgfx::updateTexture2D(_Palette, 0, 0, 0, (uint16_t)(LIGHT_FIRST + kept), VOXEL_PALETTE_SIZE, (uint16_t)added, bgfx::copy(&_Lights[(std::size_t)kept * VOXEL_PALETTE_SIZE], (uint32_t)added * VOXEL_PALETTE_SIZE));
			}
			return(true);
		}
		_Lights.clear();
		_LightIndex.clear();
		if (kept == 0) {
			break;
		}
	}
	Log_Refusal("lighting tables", (long long)LIGHT_ROWS + 1, LIGHT_ROWS);
	return(false);
}


// The layers of a part in drawing order: each piece's shadows, then its objects.
static std::vector<LayerDraw> Layers_Of(GpuVoxelPart const & part)
{
	std::vector<LayerDraw> layers;
	for (std::size_t p = 0; p < part.Pieces.size(); p++) {
		VoxelScene const & scene = part.Pieces[p].Prepared->Scene;
		for (VoxelShadowRenderStruct const & shadow : scene.Shadows) {
			LayerDraw layer;
			layer.Setup = shadow.VoxLib->Setup_Shadow(shadow, scene.Center);
			layer.Key = Model_Key(shadow.VoxLib->Get_Serial(), shadow.Layer, shadow.Info, SHADOW_WALK);
			// A shadow stamps two pixels a column, as a lit voxel does, with no shading.
			layer.Lit = true;
			layer.Reversed = false;
			layer.Shadow = true;
			layer.Piece = (int)p;
			layers.push_back(layer);
		}
		for (VoxelRenderStruct const & object : scene.Objects) {
			LayerDraw layer;
			layer.Setup = object.VoxLib->Setup_Object(object, scene.Center);
			layer.Key = Model_Key(object.VoxLib->Get_Serial(), object.Layer, object.Info, object.AnchorCornerIndex);
			layer.Lit = (layer.Setup.Function & 12) == 4;
			layer.Reversed = (layer.Setup.Function & 1) != 0;
			layer.Shadow = false;
			layer.Piece = (int)p;
			layer.Object = &object;
			layers.push_back(layer);
		}
	}
	return(layers);
}


// Places each object layer in its part's depth: by view depth across the part with GpuVoxelLayerDepth=Shared, else in
// drawing order. The layers must be in the model store.
static void Set_Layer_Depths(std::vector<LayerDraw> & layers)
{
	// A shadow part is all shadow layers, which neither test nor write depth.
	if (layers.empty() || layers[0].Shadow) {
		return;
	}
	if (_SharedDepth) {
		float nearest = -FLT_MAX;
		float farthest = FLT_MAX;
		for (LayerDraw const & layer : layers) {
			Gpu_Voxel_Widen_Depth_Range(*layer.Object, nearest, farthest);
		}
		for (LayerDraw & layer : layers) {
			layer.Depth = Gpu_Voxel_Shared_Depth(*layer.Object, layer.Setup.Arg, nearest, farthest);
		}
		return;
	}
	int total = 0;
	for (LayerDraw const & layer : layers) {
		ModelSlot const * slot = Find_Slot(layer.Key);
		total += slot != NULL ? slot->Count : 0;
	}
	int first = 0;
	for (LayerDraw & layer : layers) {
		layer.Depth = Gpu_Voxel_Ordered_Depth(first, total);
		ModelSlot const * slot = Find_Slot(layer.Key);
		first += slot != NULL ? slot->Count : 0;
	}
}


// Sets u_vox for drawing a layer of part whose voxel buffer pixel (0, 0) lands at shift_x, shift_y in the target. The
// projection is for the axes the layer's records count along, and first is the place of its first voxel in the model
// store.
static void Set_Layer_Uniforms(GpuVoxelPart const & part, LayerDraw const & layer, Vector3i16 const (&steps)[4], int first, int shift_x, int shift_y, int scale)
{
	float const vox[16] = {
		(float)steps[0].I, (float)steps[0].J, (float)steps[1].I, (float)steps[1].J,
		(float)steps[2].I, (float)steps[2].J, (float)steps[3].I, (float)steps[3].J,
		(float)shift_x, (float)shift_y, layer.Lit ? 1.0f : 0.0f, part.Scratch ? (float)PALETTE_BLOCK : 0.0f,
		(float)first, (float)scale, layer.Shadow ? 0.0f : (float)EmissiveDefaults[EMISSIVE_VOXEL_SPECULAR].Strength, (float)layer.LightRow
	};
	bgfx::setUniform(_VoxelUniform, vox, 4);
}


static uint64_t Primitive_State(int scale)
{
	return(scale > 1 ? BGFX_STATE_PT_TRISTRIP : BGFX_STATE_PT_LINES);
}


static uint32_t Vertex_Count(ModelSlot const & slot, int scale)
{
	return((uint32_t)slot.Count * (scale > 1 ? VERTICES_PER_CUBE : VERTICES_PER_LINE));
}


bool GpuVoxel_Submit(GpuDrawList const & list, int scale)
{
	if (!Start()) {
		return(false);
	}
	std::vector<char> shown(list.Voxels.size(), 0);
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Voxel >= 0) {
			shown[blit.Voxel] = 1;
		}
	}
	std::vector<std::vector<LayerDraw>> parts(list.Voxels.size());
	std::size_t shown_parts = 0;
	for (std::size_t i = 0; i < list.Voxels.size(); i++) {
		if (shown[i] != 0) {
			parts[i] = Layers_Of(list.Voxels[i]);
			shown_parts++;
		}
	}
	Upload_Palette();
	Perf_Count(PERF_COUNT_GPU_VOXEL_PARTS, list.Voxels.size());
	Perf_Count(PERF_COUNT_GPU_VOXEL_DIRECT, shown_parts);

	_DirectLayers.assign(list.Voxels.size(), std::vector<LayerDraw>());
	if (!Load_Models(parts) || !Load_Lights(list, parts)) {
		return(false);
	}
	if (Faces_Wanted(scale)) {
		Load_Faces(parts);
	}
	for (std::vector<LayerDraw> & layers : parts) {
		Set_Layer_Depths(layers);
	}
	_DirectLayers = std::move(parts);
	return(true);
}


// Sets u_vdepth for a layer of a direct blit. Only the walk's shader reads voxels, the number the layer's walk holds.
static void Set_Depth_Uniform(GpuVoxelDirectBlit const & blit, GpuVoxelLayerDepth const & depth, int voxels)
{
	float const values[12] = {
		(float)(GPU_VOXEL_SLICES - 1 - blit.Slice), 1.0f / GPU_VOXEL_SLICES, bgfx::getCaps()->homogeneousDepth ? 1.0f : 0.0f, depth.PerVoxel,
		depth.Origin, depth.Step[0], depth.Step[1], depth.Step[2],
		blit.LastFirst ? 1.0f : 0.0f, (float)voxels, 0.0f, 0.0f
	};
	bgfx::setUniform(_DepthUniform, values, 3);
}


void GpuVoxel_Draw_Direct(GpuDrawList const & list, GpuVoxelDirectBlit const & blit, int scale, std::uint16_t view, std::function<void(void)> const & bind)
{
	if (!_Ready || blit.Part < 0 || blit.Part >= (int)_DirectLayers.size()) {
		return;
	}
	GpuVoxelPart const & part = list.Voxels[blit.Part];
	Rect const & area = part.Area;
	// An object drawn between whole map pixels (GpuDraw_Set_Voxel_Shift) moves by whole target pixels, which only a
	// scale above 1 makes finer than a map pixel. Its shadow stays on the ground, and its clip gains the pixel it may
	// move into.
	int const move_x = scale > 1 ? (int)std::lround(part.ShiftX * scale) : 0;
	int const move_y = scale > 1 ? (int)std::lround(part.ShiftY * scale) : 0;
	int const room = (move_x != 0 || move_y != 0) ? 1 : 0;
	Rect const shown = Intersect(Rect(blit.OriginX, blit.OriginY, area.Width, area.Height), blit.Clip);
	std::vector<LayerDraw> const & layers = _DirectLayers[blit.Part];
	for (std::size_t i = 0; i < layers.size(); i++) {
		LayerDraw const & layer = layers[blit.LastFirst ? layers.size() - 1 - i : i];
		ModelSlot const * slot = Find_Slot(layer.Key);
		if (slot == NULL || slot->Count == 0) {
			continue;
		}
		GpuVoxelPiece const & piece = part.Pieces[layer.Piece];
		Rect const & bounds = piece.Prepared->Scene.Region.Bounds;
		// The piece's top left corner in pass pixels, before the scale.
		int const left = blit.OriginX + piece.Dest.X - area.X;
		int const top = blit.OriginY + piece.Dest.Y - area.Y;
		Rect clip = Intersect(Rect(left, top, bounds.Width, bounds.Height), shown);
		if (clip.Width <= 0 || clip.Height <= 0) {
			continue;
		}
		bool const moved = room != 0 && !layer.Shadow;
		if (moved) {
			int const right = clip.X + clip.Width + room;
			int const bottom = clip.Y + clip.Height + room;
			clip.X = std::max(clip.X - room, 0);
			clip.Y = std::max(clip.Y - room, 0);
			clip.Width = right - clip.X;
			clip.Height = bottom - clip.Y;
		}
		int const shift_x = (left - bounds.X) * scale + (moved ? move_x : 0);
		int const shift_y = (top - bounds.Y) * scale + (moved ? move_y : 0);
		FaceSlot const * faces = Find_Faces(layer, scale);
		if (faces != NULL) {
			GpuVoxelModelView const model = Gpu_Voxel_Model_View(layer.Setup.Arg, layer.Reversed, layer.Depth);
			float face[8] = {};
			int total = 0;
			for (int axis = 0; axis < 3; axis++) {
				int const direction = model.Visible[axis];
				int const count = faces->Start[direction + 1] - faces->Start[direction];
				face[axis] = (float)(faces->First + faces->Start[direction]);
				face[4 + axis] = (float)count;
				total += count;
			}
			if (total == 0) {
				continue;
			}
			Set_Layer_Uniforms(part, layer, model.Projection, 0, shift_x, shift_y, scale);
			Set_Depth_Uniform(blit, model.Depth, 0);
			bgfx::setUniform(_FaceUniform, face, 2);
			bgfx::setTexture(DIRECT_MODEL_STAGE, _FaceSampler, _FaceStore);
			bgfx::setVertexCount((uint32_t)total * VERTICES_PER_FACE);
			Perf_Count(PERF_COUNT_GPU_VOXEL_FACES, total);
			bgfx::setState(blit.State);
		} else {
			Set_Layer_Uniforms(part, layer, layer.Setup.Arg.TransformMatrix, slot->First, shift_x, shift_y, scale);
			Set_Depth_Uniform(blit, layer.Depth, slot->Count);
			bgfx::setTexture(DIRECT_MODEL_STAGE, _ModelSampler, _Model);
			bgfx::setVertexCount(Vertex_Count(*slot, scale));
			bgfx::setState(blit.State | Primitive_State(scale));
		}
		bgfx::setUniform(_BlitUniform, blit.Values, 4);
		bind();
		bgfx::setTexture(DIRECT_PALETTE_STAGE, _PaletteSampler, _Palette);
		bgfx::setScissor((uint16_t)(clip.X * scale), (uint16_t)(clip.Y * scale), (uint16_t)(clip.Width * scale), (uint16_t)(clip.Height * scale));
		bgfx::setStencil(blit.Stencil);
		bgfx::submit((bgfx::ViewId)view, faces != NULL ? _FaceProgram : _DirectProgram);
	}
}


void GpuVoxel_Shutdown(void)
{
	Release_Handle(_DirectProgram);
	Release_Handle(_DepthUniform);
	Release_Handle(_BlitUniform);
	Release_Handle(_VoxelUniform);
	Release_Handle(_ModelSampler);
	Release_Handle(_PaletteSampler);
	Release_Handle(_Model);
	Release_Handle(_Palette);
	Release_Handle(_FaceProgram);
	Release_Handle(_FaceUniform);
	Release_Handle(_FaceSampler);
	Release_Handle(_FaceStore);
	_Faces.clear();
	_FaceNext = 0;
	_Models.clear();
	_ModelNext = 0;
	_Lights.clear();
	_LightIndex.clear();
	_DirectLayers.clear();
	_PaletteUploaded = false;
	_UnfitLogged = false;
	_Ready = false;
	_Failed = false;
}
