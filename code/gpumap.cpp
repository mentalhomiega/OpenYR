/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "gpumap.h"

#include "alphalighting.h"
#include "bgfxbackend.h"
#include "bgfxviews.hh"
#include "dbgprint.h"
#include "gpudraw.h"
#include "writewatch.h"

#include <algorithm>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <utility>

#include "shaders/mapdraw/fs_mapblur.bin.h"
#include "shaders/mapdraw/fs_mapdraw.bin.h"
#include "shaders/mapdraw/fs_mapglow.bin.h"
#include "shaders/mapdraw/fs_mappost.bin.h"
#include "shaders/mapdraw/vs_mapdraw.bin.h"


static const bgfx::EmbeddedShader _MapShaders[] = {
	{
		"vs_mapdraw",
		{
			{ bgfx::RendererType::Direct3D11, vs_mapdraw_dxbc, sizeof(vs_mapdraw_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	{
		"fs_mapdraw",
		{
			{ bgfx::RendererType::Direct3D11, fs_mapdraw_dxbc, sizeof(fs_mapdraw_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	{
		"fs_mapblur",
		{
			{ bgfx::RendererType::Direct3D11, fs_mapblur_dxbc, sizeof(fs_mapblur_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	{
		"fs_mappost",
		{
			{ bgfx::RendererType::Direct3D11, fs_mappost_dxbc, sizeof(fs_mappost_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	{
		"fs_mapglow",
		{
			{ bgfx::RendererType::Direct3D11, fs_mapglow_dxbc, sizeof(fs_mapglow_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	BGFX_EMBEDDED_SHADER_END()
};


// Width of the textures that hold byte arenas; fs_mapdraw.sc uses the same value.
static int const ARENA_WIDTH = 4096;
static int const TEXTURE_LIMIT = 16384;
// The index texture holds the atlas in its top rows and the pass's arena below them.
static int const ATLAS_ROWS = (int)(GPU_ATLAS_BYTES / ARENA_WIDTH);
static_assert(GPU_ATLAS_BYTES % ARENA_WIDTH == 0);

enum ShaderKind
{
	KIND_COPY,
	KIND_DARKEN,
	KIND_SOURCE75,
	KIND_HALF,
	KIND_SOURCE25,
	KIND_BASE,
	KIND_COPY_NO_DEPTH,
	KIND_PIXEL,
	KIND_SHADOW_MASK,
	KIND_SHADOWED_BASE,
	KIND_BRIGHTEN,
	KIND_LINE,
	KIND_ADDITIVE_LINE,
	KIND_ANTIALIASED_LINE,
	KIND_SONIC,
	KIND_TILE_SHADOW,
};

enum SamplerSlot
{
	SLOT_INDEX,
	SLOT_LUT,
	SLOT_ALUT,
	SLOT_ZBUF,
	SLOT_ABUF,
	SLOT_ZSHAPE,
	SLOT_BASE,
	SLOT_COUNT,
};

static char const * const _SamplerNames[SLOT_COUNT] = { "s_index", "s_lut", "s_alut", "s_zbuf", "s_abuf", "s_zshape", "s_base" };

struct MapVertex
{
	float X;
	float Y;
	float T0[4];
	float T1[4];
	float T2[4];
	float T3[4];
};

static bool _Ready = false;
static bool _Failed = false;
static bgfx::ProgramHandle _Program = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _Samplers[SLOT_COUNT];
static bgfx::VertexLayout _Layout;
static bgfx::TextureHandle _TargetTexture = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _Target = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Readback = BGFX_INVALID_HANDLE;
static int _TargetWidth = 0;
static int _TargetHeight = 0;

// Soft shadows: shadows are drawn into a mask, the mask is blurred, and the base is darkened through it.
static int _SoftShadowRadius = 0;
static bgfx::ProgramHandle _BlurProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _BlurUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _MaskSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _ShadowSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _ShadowTexture[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
static bgfx::FrameBufferHandle _ShadowTarget[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };

static int const RIPPLE_LIMIT = 16;
static bgfx::ProgramHandle _PostProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _SceneSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _RippleUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _RippleInfoUniform = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _PostTexture = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _PostTarget = BGFX_INVALID_HANDLE;
static bool _PostShown = false;
// Set while the target holds a picture the GPU drew.
static bool _TargetDrawn = false;
// Set when the kept pass could not be submitted; the next pass is then drawn in software.
static bool _SubmitFailed = false;
static bgfx::UniformHandle _RingUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _SnapshotSampler = BGFX_INVALID_HANDLE;
// The tile layer's shadow plane, kept between passes so only changed rows go up.
static bgfx::UniformHandle _TileShadowSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _TileShadow = BGFX_INVALID_HANDLE;
static bool _TileShadowTracked = false;

// Water ripples read the water marks from the same plane as the tile shadows.
static int _WaterStrength = 0;
static bgfx::UniformHandle _WaterUniform = BGFX_INVALID_HANDLE;
static int _CloudStrength = 0;
static bgfx::UniformHandle _WorldUniform = BGFX_INVALID_HANDLE;
static GpuWeather _Weather = GPU_WEATHER_NONE;
static int _WeatherStrength = 0;
static bgfx::UniformHandle _WeatherUniform = BGFX_INVALID_HANDLE;

// Bloom: the emissive mask, kept between passes like the tile shadows, weights the picture into a
// quarter-size glow that is blurred and added back in the post pass.
static int _BloomStrength = 0;
static int const GLOW_RADIUS = 6;
static bgfx::ProgramHandle _GlowProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _GlowUniform = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _GlowMaskSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _GlowSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _GlowMask = BGFX_INVALID_HANDLE;
static bool _GlowMaskTracked = false;
static bgfx::TextureHandle _GlowTexture[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
static bgfx::FrameBufferHandle _GlowTarget[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
static bgfx::TextureHandle _Snapshot = BGFX_INVALID_HANDLE;

// Set while a drawn pass's buffers are uploaded by reference and its frame is not yet submitted.
static bool _Referenced = false;
static bool _Pending = false;
static std::uint32_t _ReadyFrame = 0;

// True from a pass that handed the GPU its buffers without a copy until bgfx::frame reads them.
static std::vector<unsigned char> _Pixels;
static std::vector<unsigned short> _Expected;
static int _PendingFrame = 0;
static int _PendingBlits = 0;


static bool Start(void)
{
	if (_Ready || _Failed) {
		return(_Ready);
	}
	_Failed = true;
	if (bgfx::getRendererType() != bgfx::RendererType::Direct3D11) {
		DebugString("GPU_DIFF unavailable: the map shaders exist for Direct3D 11 only\n");
		return(false);
	}
	bgfx::RendererType::Enum const type = bgfx::getRendererType();
	_Program = bgfx::createProgram(bgfx::createEmbeddedShader(_MapShaders, type, "vs_mapdraw"), bgfx::createEmbeddedShader(_MapShaders, type, "fs_mapdraw"), true);
	if (!bgfx::isValid(_Program)) {
		DebugString("GPU_DIFF unavailable: the map program did not build\n");
		return(false);
	}
	for (int slot = 0; slot < SLOT_COUNT; slot++) {
		_Samplers[slot] = bgfx::createUniform(_SamplerNames[slot], bgfx::UniformType::Sampler);
	}
	_ShadowSampler = bgfx::createUniform("s_shadow", bgfx::UniformType::Sampler);
	_MaskSampler = bgfx::createUniform("s_mask", bgfx::UniformType::Sampler);
	_BlurUniform = bgfx::createUniform("u_blur", bgfx::UniformType::Vec4);
	_BlurProgram = bgfx::createProgram(bgfx::createEmbeddedShader(_MapShaders, type, "vs_mapdraw"), bgfx::createEmbeddedShader(_MapShaders, type, "fs_mapblur"), true);
	_SceneSampler = bgfx::createUniform("s_scene", bgfx::UniformType::Sampler);
	_RippleUniform = bgfx::createUniform("u_ripples", bgfx::UniformType::Vec4, RIPPLE_LIMIT);
	_RippleInfoUniform = bgfx::createUniform("u_rippleinfo", bgfx::UniformType::Vec4);
	_RingUniform = bgfx::createUniform("u_ring", bgfx::UniformType::Vec4, 2);
	_SnapshotSampler = bgfx::createUniform("s_snapshot", bgfx::UniformType::Sampler);
	_TileShadowSampler = bgfx::createUniform("s_tileshadow", bgfx::UniformType::Sampler);
	_WaterUniform = bgfx::createUniform("u_water", bgfx::UniformType::Vec4);
	_WorldUniform = bgfx::createUniform("u_world", bgfx::UniformType::Vec4);
	_WeatherUniform = bgfx::createUniform("u_weather", bgfx::UniformType::Vec4);
	_PostProgram = bgfx::createProgram(bgfx::createEmbeddedShader(_MapShaders, type, "vs_mapdraw"), bgfx::createEmbeddedShader(_MapShaders, type, "fs_mappost"), true);
	_GlowProgram = bgfx::createProgram(bgfx::createEmbeddedShader(_MapShaders, type, "vs_mapdraw"), bgfx::createEmbeddedShader(_MapShaders, type, "fs_mapglow"), true);
	_GlowUniform = bgfx::createUniform("u_glow", bgfx::UniformType::Vec4);
	_GlowMaskSampler = bgfx::createUniform("s_glowmask", bgfx::UniformType::Sampler);
	_GlowSampler = bgfx::createUniform("s_glow", bgfx::UniformType::Sampler);
	if (!bgfx::isValid(_BlurProgram) || !bgfx::isValid(_PostProgram) || !bgfx::isValid(_GlowProgram)) {
		DebugString("GPU_DIFF: the shadow blur, shockwave or glow program did not build\n");
	}
	_Layout.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord2, 4, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord3, 4, bgfx::AttribType::Float)
		.end();
	_Failed = false;
	_Ready = true;
	return(true);
}


static void Destroy_Target(void)
{
	if (bgfx::isValid(_PostTarget)) {
		bgfx::destroy(_PostTarget);
		_PostTarget = BGFX_INVALID_HANDLE;
		_PostTexture = BGFX_INVALID_HANDLE;
	}
	_PostShown = false;
	_TargetDrawn = false;
	for (int i = 0; i < 2; i++) {
		if (bgfx::isValid(_GlowTarget[i])) {
			bgfx::destroy(_GlowTarget[i]);
			_GlowTarget[i] = BGFX_INVALID_HANDLE;
			_GlowTexture[i] = BGFX_INVALID_HANDLE;
		}
	}
	if (bgfx::isValid(_GlowMask)) {
		bgfx::destroy(_GlowMask);
		_GlowMask = BGFX_INVALID_HANDLE;
	}
	_GlowMaskTracked = false;
	for (int i = 0; i < 2; i++) {
		if (bgfx::isValid(_ShadowTarget[i])) {
			bgfx::destroy(_ShadowTarget[i]);
			_ShadowTarget[i] = BGFX_INVALID_HANDLE;
			_ShadowTexture[i] = BGFX_INVALID_HANDLE;
		}
	}
	if (bgfx::isValid(_Target)) {
		bgfx::destroy(_Target);
		_Target = BGFX_INVALID_HANDLE;
		_TargetTexture = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_Readback)) {
		bgfx::destroy(_Readback);
		_Readback = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_Snapshot)) {
		bgfx::destroy(_Snapshot);
		_Snapshot = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_TileShadow)) {
		bgfx::destroy(_TileShadow);
		_TileShadow = BGFX_INVALID_HANDLE;
	}
	_TileShadowTracked = false;
	_TargetWidth = 0;
	_TargetHeight = 0;
}


static bool Size_Target(int width, int height)
{
	if (width == _TargetWidth && height == _TargetHeight && bgfx::isValid(_Target)) {
		return(true);
	}
	Destroy_Target();
	uint64_t const point = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	_TargetTexture = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | point);
	if (!bgfx::isValid(_TargetTexture)) {
		return(false);
	}
	_Target = bgfx::createFrameBuffer(1, &_TargetTexture, true);
	_Readback = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | point);
	_Snapshot = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | point);
	_TileShadow = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::R8, point);
	if (!bgfx::isValid(_Target) || !bgfx::isValid(_Readback) || !bgfx::isValid(_Snapshot) || !bgfx::isValid(_TileShadow)) {
		Destroy_Target();
		return(false);
	}
	for (int i = 0; i < 2; i++) {
		_ShadowTexture[i] = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::R8, BGFX_TEXTURE_RT | point);
		_ShadowTarget[i] = bgfx::isValid(_ShadowTexture[i]) ? bgfx::createFrameBuffer(1, &_ShadowTexture[i], true) : bgfx::FrameBufferHandle{ bgfx::kInvalidHandle };
		if (!bgfx::isValid(_ShadowTarget[i])) {
			Destroy_Target();
			return(false);
		}
	}
	_PostTexture = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | point);
	_PostTarget = bgfx::isValid(_PostTexture) ? bgfx::createFrameBuffer(1, &_PostTexture, true) : bgfx::FrameBufferHandle{ bgfx::kInvalidHandle };
	if (!bgfx::isValid(_PostTarget)) {
		Destroy_Target();
		return(false);
	}
	_GlowMask = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::R8, point);
	uint64_t const linear = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	for (int i = 0; i < 2; i++) {
		_GlowTexture[i] = bgfx::createTexture2D((uint16_t)((width + 3) / 4), (uint16_t)((height + 3) / 4), false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | linear);
		_GlowTarget[i] = bgfx::isValid(_GlowTexture[i]) ? bgfx::createFrameBuffer(1, &_GlowTexture[i], true) : bgfx::FrameBufferHandle{ bgfx::kInvalidHandle };
	}
	if (!bgfx::isValid(_GlowMask) || !bgfx::isValid(_GlowTarget[0]) || !bgfx::isValid(_GlowTarget[1])) {
		Destroy_Target();
		return(false);
	}
	_TargetWidth = width;
	_TargetHeight = height;
	_Pixels.assign((std::size_t)width * height * 4, 0);
	return(true);
}


// A texture the passes rewrite; it is remade only when it must grow or change width.
struct Store
{
	bgfx::TextureHandle Handle = BGFX_INVALID_HANDLE;
	int Width = 0;
	int Height = 0;

	// Set while the texture holds the values Fill_R16_Changed or Fill_R16_Watched last wrote.
	bool Tracked = false;
};

static Store _Stores[SLOT_COUNT];


static void Destroy_Stores(void)
{
	for (Store & store : _Stores) {
		if (bgfx::isValid(store.Handle)) {
			bgfx::destroy(store.Handle);
		}
		store = Store();
	}
}


// Writes memory into the store's top rows. Exact makes the store exactly that many rows; otherwise
// it keeps spare rows so that it is remade rarely.
static bool Fill_Store(Store & store, bgfx::TextureFormat::Enum format, int width, int rows, bool exact, bgfx::Memory const * memory)
{
	if (width <= 0 || rows <= 0 || width > TEXTURE_LIMIT || rows > TEXTURE_LIMIT) {
		return(false);
	}
	store.Tracked = false;
	if (!bgfx::isValid(store.Handle) || store.Width != width || store.Height < rows || (exact && store.Height != rows)) {
		if (bgfx::isValid(store.Handle)) {
			bgfx::destroy(store.Handle);
		}
		int const height = exact ? rows : std::min(TEXTURE_LIMIT, std::max(rows, store.Height) * 2);
		store.Handle = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, format, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
		store.Width = width;
		store.Height = height;
		if (!bgfx::isValid(store.Handle)) {
			store = Store();
			return(false);
		}
	}
	bgfx::updateTexture2D(store.Handle, 0, 0, 0, 0, (uint16_t)width, (uint16_t)rows, memory);
	return(true);
}


// Fills the index store: the atlas in its top ATLAS_ROWS rows, uploading only what the store does not
// hold yet, then the pass's arena, ARENA_WIDTH texels a row.
static bool Fill_Index(Store & store, GpuAtlas const & atlas, void const * bytes, std::size_t size)
{
	static unsigned uploaded_epoch = 0;
	static std::size_t uploaded = 0;

	int const rows = std::max(1, (int)((size + ARENA_WIDTH - 1) / ARENA_WIDTH));
	if (ATLAS_ROWS + rows > TEXTURE_LIMIT) {
		return(false);
	}
	store.Tracked = false;
	if (!bgfx::isValid(store.Handle) || store.Width != ARENA_WIDTH || store.Height < ATLAS_ROWS + rows) {
		if (bgfx::isValid(store.Handle)) {
			bgfx::destroy(store.Handle);
		}
		int const height = std::min(TEXTURE_LIMIT, std::max(ATLAS_ROWS + rows, store.Height) * 2);
		store.Handle = bgfx::createTexture2D((uint16_t)ARENA_WIDTH, (uint16_t)height, false, 1, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
		store.Width = ARENA_WIDTH;
		store.Height = height;
		uploaded = 0;
		if (!bgfx::isValid(store.Handle)) {
			store = Store();
			return(false);
		}
	}
	if (uploaded_epoch != atlas.Epoch || uploaded > atlas.Bytes.size()) {
		uploaded_epoch = atlas.Epoch;
		uploaded = 0;
	}
	if (uploaded < atlas.Bytes.size()) {
		int const first = (int)(uploaded / ARENA_WIDTH);
		int const last = (int)((atlas.Bytes.size() + ARENA_WIDTH - 1) / ARENA_WIDTH);
		std::size_t const start = (std::size_t)first * ARENA_WIDTH;
		bgfx::Memory const * memory = bgfx::alloc((uint32_t)ARENA_WIDTH * (last - first));
		std::memcpy(memory->data, atlas.Bytes.data() + start, atlas.Bytes.size() - start);
		std::memset(memory->data + (atlas.Bytes.size() - start), 0, memory->size - (atlas.Bytes.size() - start));
		bgfx::updateTexture2D(store.Handle, 0, 0, 0, (uint16_t)first, (uint16_t)ARENA_WIDTH, (uint16_t)(last - first), memory);
		uploaded = atlas.Bytes.size();
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)ARENA_WIDTH * rows);
	if (size > 0) {
		std::memcpy(memory->data, bytes, size);
	}
	std::memset(memory->data + size, 0, memory->size - size);
	bgfx::updateTexture2D(store.Handle, 0, 0, 0, (uint16_t)ATLAS_ROWS, (uint16_t)ARENA_WIDTH, (uint16_t)rows, memory);
	return(true);
}


// The offset of a blit's colour indices in the index texture.
static float Index_Offset(GpuBlit const & blit)
{
	return((float)(blit.InAtlas ? blit.Pixels : (std::uint64_t)ATLAS_ROWS * ARENA_WIDTH + blit.Pixels));
}


// Fills the store with the bytes, ARENA_WIDTH texels a row; an empty arena gets one row of zeros.
static bool Fill_Arena(Store & store, void const * bytes, std::size_t size)
{
	int const rows = std::max(1, (int)((size + ARENA_WIDTH - 1) / ARENA_WIDTH));
	if (rows > TEXTURE_LIMIT) {
		return(false);
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)ARENA_WIDTH * rows);
	if (size > 0) {
		std::memcpy(memory->data, bytes, size);
	}
	std::memset(memory->data + size, 0, memory->size - size);
	return(Fill_Store(store, bgfx::TextureFormat::R8, ARENA_WIDTH, rows, false, memory));
}


// With reference set, the values must stay unchanged until the next frame is submitted.
static bool Fill_R16(Store & store, unsigned short const * values, int width, int rows, bool exact, bool reference = false)
{
	uint32_t const size = (uint32_t)((std::size_t)width * rows * sizeof(unsigned short));
	return(Fill_Store(store, bgfx::TextureFormat::R16, width, rows, exact, reference ? bgfx::makeRef(values, size) : bgfx::copy(values, size)));
}


// Uploads rows first to end of the values, which are width values a row.
static void Update_Rows(Store & store, unsigned short const * values, int width, int first, int end)
{
	uint32_t const size = (uint32_t)((std::size_t)(end - first) * width * sizeof(unsigned short));
	bgfx::updateTexture2D(store.Handle, 0, 0, 0, (uint16_t)first, (uint16_t)width, (uint16_t)(end - first), bgfx::copy(values + (std::size_t)first * width, size));
}


// Makes the store exactly width by rows values, uploading only the rows on pages written since the
// last call when the system lists them (writewatch.h). Reference is as for Fill_R16.
static bool Fill_R16_Watched(Store & store, unsigned short const * values, int width, int rows, bool reference)
{
	std::size_t const bytes = (std::size_t)width * rows * sizeof(unsigned short);
	std::vector<std::pair<std::size_t, std::size_t>> runs;
	bool const listed = WriteWatch_Take(values, bytes, runs);
	if (!listed || !store.Tracked || store.Width != width || store.Height != rows) {
		if (!Fill_R16(store, values, width, rows, true, reference)) {
			return(false);
		}
		store.Tracked = listed;
		return(true);
	}
	std::size_t const pitch = (std::size_t)width * sizeof(unsigned short);
	int done = 0;
	for (auto const & run : runs) {
		int const first = std::max(done, (int)(run.first / pitch));
		int const end = std::min(rows, (int)((run.second + pitch - 1) / pitch));
		if (first < end) {
			Update_Rows(store, values, width, first, end);
			done = end;
		}
	}
	return(true);
}


// Makes the store exactly width by rows values, uploading only the rows flagged in changed when
// it already holds the rest, and clears the flags.
static bool Fill_R16_Changed(Store & store, unsigned short const * values, int width, int rows, std::vector<unsigned char> & changed)
{
	if (!store.Tracked || store.Width != width || store.Height != rows || (int)changed.size() != rows) {
		if (!Fill_R16(store, values, width, rows, true)) {
			return(false);
		}
		store.Tracked = true;
		changed.assign(rows, 0);
		return(true);
	}
	// Changed rows up to 8 apart go up together, so a scattered change takes few updates.
	int const GAP = 8;
	int row = 0;
	while (row < rows) {
		if (changed[row] == 0) {
			row++;
			continue;
		}
		int const first = row;
		int unchanged = 0;
		for (row++; row < rows && unchanged < GAP; row++) {
			unchanged = changed[row] != 0 ? 0 : unchanged + 1;
		}
		int const end = row - unchanged;
		std::fill(changed.begin() + first, changed.begin() + end, 0);
		Update_Rows(store, values, width, first, end);
	}
	return(true);
}


// Uploads the rows of a plane kept in texture that changed since the GPU last took them, and clears
// their flags. Tracked is cleared when the texture is remade, which sends every row.
static void Upload_Plane(bgfx::TextureHandle texture, bool & tracked, std::vector<unsigned char> const & values, int width, int rows, std::vector<unsigned char> & changed)
{
	if (!tracked || (int)changed.size() != rows) {
		changed.assign(rows, 1);
		tracked = true;
	}
	int const GAP = 8;
	int row = 0;
	while (row < rows) {
		if (changed[row] == 0) {
			row++;
			continue;
		}
		int const first = row;
		int unchanged = 0;
		for (row++; row < rows && unchanged < GAP; row++) {
			unchanged = changed[row] != 0 ? 0 : unchanged + 1;
		}
		int const end = row - unchanged;
		std::fill(changed.begin() + first, changed.begin() + end, 0);
		bgfx::updateTexture2D(texture, 0, 0, 0, (uint16_t)first, (uint16_t)width, (uint16_t)(end - first), bgfx::copy(&values[(std::size_t)first * width], (uint32_t)width * (end - first)));
	}
}


// Rows of 256 translator entries a blit through remap can read: the highest band it adds, plus one.
static int Translator_Rows(AlphaLightingRemapClass const * remap, std::unordered_map<AlphaLightingRemapClass const *, int> & cache)
{
	if (remap == NULL) {
		return(1);
	}
	auto found = cache.find(remap);
	if (found != cache.end()) {
		return(found->second);
	}
	unsigned short const * entry = &remap->Buffer[0][0];
	unsigned short const highest = *std::max_element(entry, entry + AlphaLightingRemapClass::BUFFER_SIZE);
	int const rows = highest / 256 + 1;
	cache[remap] = rows;
	return(rows);
}


static int Shader_Kind(BlitDesc const & desc)
{
	switch (desc.Kind) {
		case BlitDesc::DARKEN:
			return(KIND_DARKEN);
		case BlitDesc::LUCENT25:
			return(KIND_SOURCE75);
		case BlitDesc::LUCENT50:
			return(KIND_HALF);
		case BlitDesc::LUCENT75:
			return(KIND_SOURCE25);
		default:
			return(desc.ZRead ? KIND_COPY : KIND_COPY_NO_DEPTH);
	}
}


static void Set_Quad(MapVertex * vertex, Rect const & area, int x, int y, int width, int height, float const * t1, float const * t2, float const * t3)
{
	float const left = (float)(x - area.X);
	float const top = (float)(y - area.Y);
	float const corners[4][2] = { { 0.0f, 0.0f }, { (float)width, 0.0f }, { (float)width, (float)height }, { 0.0f, (float)height } };
	static int const order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int i = 0; i < 6; i++) {
		float const * corner = corners[order[i]];
		MapVertex & v = vertex[i];
		v.X = left + corner[0];
		v.Y = top + corner[1];
		v.T0[0] = corner[0];
		v.T0[1] = corner[1];
		v.T0[2] = left + corner[0];
		v.T0[3] = top + corner[1];
		std::memcpy(v.T1, t1, sizeof(v.T1));
		std::memcpy(v.T2, t2, sizeof(v.T2));
		std::memcpy(v.T3, t3, sizeof(v.T3));
	}
}


static void Set_Map_View(bgfx::ViewId view, bgfx::FrameBufferHandle target, Rect const & area, float const * projection)
{
	bgfx::setViewFrameBuffer(view, target);
	bgfx::setViewRect(view, 0, 0, (uint16_t)area.Width, (uint16_t)area.Height);
	bgfx::setViewTransform(view, NULL, projection);
	bgfx::setViewMode(view, bgfx::ViewMode::Sequential);
	bgfx::setViewClear(view, BGFX_CLEAR_NONE);
}


// Draws every shadow's coverage, with the tile layer's shadows when tileshadow is set, into the first
// shadow mask and blurs it there, through the second.
static void Submit_Shadows(GpuDrawList const & list, Rect const & area, float const * projection, float const * ring, int shadowquads, bool tileshadow)
{
	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, shadowquads * 6, _Layout);
	MapVertex * vertex = (MapVertex *)buffer.data;
	{
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, 0.0f, 1.0f };
		Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
	}
	int quad = 1;
	if (tileshadow) {
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, (float)KIND_TILE_SHADOW, 1.0f };
		Set_Quad(vertex + 6, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
		quad++;
	}
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Colour >= 0 || blit.Brighten >= 0 || blit.Desc.Kind != BlitDesc::DARKEN) {
			continue;
		}
		float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, 0.0f, -1.0f };
		float const t2[4] = { (float)blit.ZFirst, (float)blit.ZFraction, (float)std::max(blit.ZNumerator, 1), (float)blit.ZDenominator };
		float const t3[4] = { (float)blit.ZWrap, (float)blit.ZShape, (float)KIND_SHADOW_MASK, blit.Desc.ZRead ? 0.0f : 1.0f };
		Set_Quad(vertex + quad * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
		quad++;
	}

	Set_Map_View(VIEW_MAPSHADOW, _ShadowTarget[0], area, projection);
	bgfx::setViewClear(VIEW_MAPSHADOW, BGFX_CLEAR_COLOR, 0x00000000);
	for (int i = 1; i < shadowquads; i++) {
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, 6);
		for (int slot = 0; slot < SLOT_COUNT; slot++) {
			bgfx::setTexture((uint8_t)slot, _Samplers[slot], _Stores[slot].Handle);
		}
		if (tileshadow) {
			bgfx::setTexture(SLOT_COUNT + 2, _TileShadowSampler, _TileShadow);
		}
		bgfx::setUniform(_RingUniform, ring, 2);
		bgfx::setState(BGFX_STATE_WRITE_RGB);
		bgfx::submit(VIEW_MAPSHADOW, _Program);
	}

	bgfx::ViewId const views[2] = { VIEW_MAPBLURX, VIEW_MAPBLURY };
	for (int pass = 0; pass < 2; pass++) {
		Set_Map_View(views[pass], _ShadowTarget[1 - pass], area, projection);
		float const blur[4] = { pass == 0 ? 1.0f : 0.0f, pass == 0 ? 0.0f : 1.0f, (float)_SoftShadowRadius, 0.0f };
		bgfx::setUniform(_BlurUniform, blur);
		bgfx::setVertexBuffer(0, &buffer, 0, 6);
		bgfx::setTexture(0, _MaskSampler, _ShadowTexture[pass]);
		bgfx::setState(BGFX_STATE_WRITE_RGB);
		bgfx::submit(views[pass], _BlurProgram);
	}
}


// Weights the drawn picture by the emissive mask into the first glow picture, a quarter of the size,
// and blurs it there through the second.
static void Submit_Glow(GpuMapPass const & pass, Rect const & area)
{
	Upload_Plane(_GlowMask, _GlowMaskTracked, *pass.Glow, area.Width, area.Height, *pass.GlowChanged);
	Rect const quarter(0, 0, (area.Width + 3) / 4, (area.Height + 3) / 4);
	float projection[16];
	Backend_Build_Ortho_Projection(projection, quarter.Width, quarter.Height);
	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, 6, _Layout);
	float const t[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Set_Quad((MapVertex *)buffer.data, quarter, 0, 0, quarter.Width, quarter.Height, t, t, t);

	Set_Map_View(VIEW_MAPGLOW, _GlowTarget[0], quarter, projection);
	float const prepare[4] = { (float)area.Width, (float)area.Height, 0.0f, 0.0f };
	bgfx::setUniform(_GlowUniform, prepare);
	bgfx::setVertexBuffer(0, &buffer, 0, 6);
	bgfx::setTexture(0, _SceneSampler, _TargetTexture);
	bgfx::setTexture(1, _GlowMaskSampler, _GlowMask);
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	bgfx::submit(VIEW_MAPGLOW, _GlowProgram);

	bgfx::ViewId const views[2] = { VIEW_MAPGLOWX, VIEW_MAPGLOWY };
	for (int step = 0; step < 2; step++) {
		Set_Map_View(views[step], _GlowTarget[1 - step], quarter, projection);
		float const blur[4] = { step == 0 ? 1.0f : 0.0f, step == 0 ? 0.0f : 1.0f, (float)GLOW_RADIUS, 1.0f };
		bgfx::setUniform(_GlowUniform, blur);
		bgfx::setVertexBuffer(0, &buffer, 0, 6);
		bgfx::setTexture(0, _SceneSampler, _GlowTexture[step]);
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
		bgfx::submit(views[step], _GlowProgram);
	}
}


// Checks the limits Submit_Pass rejects a pass for that are known when it is kept, so a pass the GPU
// cannot draw is drawn in software rather than leaving the map's key colour on screen.
static bool Pass_Fits(GpuMapPass const & pass)
{
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	if (area.Width <= 0 || area.Height <= 0 || area.Width > TEXTURE_LIMIT || area.Height > TEXTURE_LIMIT) {
		return(false);
	}
	std::size_t const arenarows = (list.PixelArena.size() + ARENA_WIDTH - 1) / ARENA_WIDTH;
	std::size_t const zshaperows = (list.ZShapeArena.size() + ARENA_WIDTH - 1) / ARENA_WIDTH;
	if (ATLAS_ROWS + arenarows > (std::size_t)TEXTURE_LIMIT || zshaperows > (std::size_t)TEXTURE_LIMIT) {
		return(false);
	}
	int waves = 0;
	int shadows = 0;
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Line >= 0 && blit.LineKind == GPU_LINE_SONIC) {
			waves++;
		}
		if (blit.Colour < 0 && blit.Brighten < 0 && blit.Desc.Kind == BlitDesc::DARKEN) {
			shadows++;
		}
	}
	if (waves > VIEW_MAPWAVE_LAST - VIEW_MAPWAVE + 1) {
		return(false);
	}
	// At most the vertices Submit_Pass takes, which allocates them later in the same frame.
	uint32_t const vertices = (uint32_t)((list.Blits.size() + 1 + (_SoftShadowRadius > 0 ? shadows + 2 : 0)) * 6);
	return(bgfx::getAvailTransientVertexBuffer(vertices, _Layout) >= vertices);
}


static bool Submit_Pass(GpuMapPass const & pass)
{
	if (!Start()) {
		return(false);
	}
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	if (!Size_Target(area.Width, area.Height)) {
		DebugString("GPU_DIFF frame %d: no %dx%d target\n", pass.Frame, area.Width, area.Height);
		return(false);
	}

	// Every translator the list draws through is stacked into one texture of 256-entry rows, and
	// every lighting remap into another, 256 rows each.
	std::unordered_map<AlphaLightingRemapClass const *, int> bandrows;
	std::unordered_map<unsigned short const *, std::pair<int, int>> lutrow;
	std::unordered_map<AlphaLightingRemapClass const *, int> alutbase;
	std::vector<unsigned short> lut;
	std::vector<unsigned short> alut;
	for (GpuBlit const & blit : list.Blits) {
		BlitDesc const & desc = blit.Desc;
		if (blit.Colour >= 0 || blit.Brighten >= 0 || blit.Line >= 0 || desc.Kind == BlitDesc::DARKEN) {
			continue;
		}
		int const rows = Translator_Rows(desc.AlphaRemap, bandrows);
		auto found = lutrow.find(desc.Translator);
		if (found == lutrow.end() || found->second.second < rows) {
			lutrow[desc.Translator] = { (int)(lut.size() / 256), rows };
			lut.insert(lut.end(), desc.Translator, desc.Translator + rows * 256);
		}
		if (desc.AlphaRemap != NULL && alutbase.find(desc.AlphaRemap) == alutbase.end()) {
			alutbase[desc.AlphaRemap] = (int)(alut.size() / 256);
			unsigned short const * entry = &desc.AlphaRemap->Buffer[0][0];
			alut.insert(alut.end(), entry, entry + AlphaLightingRemapClass::BUFFER_SIZE);
		}
	}
	if (lut.empty()) {
		lut.assign(256, 0);
	}
	if (alut.empty()) {
		alut.assign(256, 0);
	}

	bool valid = Fill_Index(_Stores[SLOT_INDEX], GpuDraw_Atlas(), list.PixelArena.data(), list.PixelArena.size());
	valid = Fill_R16(_Stores[SLOT_LUT], lut.data(), 256, (int)(lut.size() / 256), false) && valid;
	valid = Fill_R16(_Stores[SLOT_ALUT], alut.data(), 256, (int)(alut.size() / 256), false) && valid;
	// A pass the GPU has not read yet still holds the buffers' memory, so this one is copied.
	bool const reference = pass.Persistent && !_Referenced;
	if (reference) {
		static bool registered = false;
		if (!registered) {
			registered = true;
			std::atexit(GpuMap_Release_References);
		}
		_Referenced = true;
	}
	// The shader finds an area pixel's depth and alpha through ring addressing; an unwrapped copy is
	// a ring of the area's width that starts at 0.
	float ring[8] = { 0.0f, (float)area.Width, (float)area.Width * area.Height, 0.0f, 0.0f, (float)area.Width, (float)area.Width * area.Height, 0.0f };
	if (pass.Persistent) {
		GpuRing const * const rings[2] = { &pass.DepthRing, &pass.AlphaRing };
		Store * const stores[2] = { &_Stores[SLOT_ZBUF], &_Stores[SLOT_ABUF] };
		for (int i = 0; i < 2; i++) {
			GpuRing const & source = *rings[i];
			if (source.Values != NULL) {
				valid = Fill_R16_Watched(*stores[i], source.Values, source.Width, source.Height, reference) && valid;
				ring[i * 4 + 0] = (float)source.Start;
				ring[i * 4 + 1] = (float)source.Width;
				ring[i * 4 + 2] = (float)source.Width * source.Height;
			} else if (!bgfx::isValid(stores[i]->Handle)) {
				valid = Fill_R16(*stores[i], pass.Base->data(), area.Width, area.Height, true) && valid;
			}
		}
	} else {
		valid = Fill_R16(_Stores[SLOT_ZBUF], pass.Depth->data(), area.Width, area.Height, true) && valid;
		valid = Fill_R16(_Stores[SLOT_ABUF], pass.Alpha->data(), area.Width, area.Height, true) && valid;
	}
	valid = Fill_Arena(_Stores[SLOT_ZSHAPE], list.ZShapeArena.data(), list.ZShapeArena.size()) && valid;
	if (pass.Persistent && pass.BaseChanged != NULL) {
		valid = Fill_R16_Changed(_Stores[SLOT_BASE], pass.Base->data(), area.Width, area.Height, *pass.BaseChanged) && valid;
	} else {
		valid = Fill_R16(_Stores[SLOT_BASE], pass.Base->data(), area.Width, area.Height, true) && valid;
	}

	int shadows = 0;
	if (_SoftShadowRadius > 0) {
		for (GpuBlit const & blit : list.Blits) {
			if (blit.Colour < 0 && blit.Brighten < 0 && blit.Desc.Kind == BlitDesc::DARKEN) {
				shadows++;
			}
		}
	}
	int const waves = (int)std::count_if(list.Blits.begin(), list.Blits.end(), [](GpuBlit const & blit) {
		return(blit.Line >= 0 && blit.LineKind == GPU_LINE_SONIC);
	});
	if (waves > VIEW_MAPWAVE_LAST - VIEW_MAPWAVE + 1) {
		DebugString("GPU_DIFF frame %d: %d sonic waves are more than the pass draws\n", pass.Frame, waves);
		return(false);
	}
	bool const plane = pass.TileShadow != NULL && pass.TileShadowChanged != NULL && pass.TileShadow->size() == (std::size_t)area.Width * area.Height;
	bool const tileshadow = plane && _SoftShadowRadius > 0;
	bool const water = plane && _WaterStrength > 0;
	if (tileshadow || water) {
		Upload_Plane(_TileShadow, _TileShadowTracked, *pass.TileShadow, area.Width, area.Height, *pass.TileShadowChanged);
	}
	bool const shadowed = shadows > 0 || tileshadow;
	bool const clouds = _CloudStrength > 0;
	bool const weather = _Weather != GPU_WEATHER_NONE && _WeatherStrength > 0;
	float world[4] = { (float)pass.WorldX, (float)pass.WorldY, 0.0f, clouds ? 0.3f * _CloudStrength / 100.0f : 0.0f };
	if (water || clouds || weather) {
		// Wrapped every 1,000 seconds, so the float keeps its precision.
		long long const ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		world[2] = (float)(ms % 1000000) / 1000.0f;
	}
	int const quads = (int)list.Blits.size() + 1;
	int const shadowquads = shadowed ? shadows + 1 + (tileshadow ? 1 : 0) : 0;
	if (valid && bgfx::getAvailTransientVertexBuffer((quads + shadowquads) * 6, _Layout) < (uint32_t)(quads + shadowquads) * 6) {
		valid = false;
	}
	if (!valid) {
		DebugString("GPU_DIFF frame %d: the pass does not fit the GPU limits\n", pass.Frame);
		return(false);
	}

	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, quads * 6, _Layout);
	MapVertex * vertex = (MapVertex *)buffer.data;
	{
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, (float)(shadowed ? KIND_SHADOWED_BASE : KIND_BASE), 0.0f };
		Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
	}
	for (std::size_t i = 0; i < list.Blits.size(); i++) {
		GpuBlit const & blit = list.Blits[i];
		BlitDesc const & desc = blit.Desc;
		if (blit.Brighten >= 0) {
			float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, (float)blit.Brighten, -1.0f };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)KIND_BRIGHTEN, 1.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			continue;
		}
		if (blit.Line >= 0) {
			float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, (float)blit.Line, (float)blit.Fade };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)(KIND_LINE + blit.LineKind), 1.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			continue;
		}
		if (blit.Colour >= 0) {
			float const t1[4] = { (float)blit.Colour, 1.0f, 0.0f, -1.0f };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)KIND_PIXEL, 0.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			continue;
		}
		float lutfirst = 0.0f;
		float alutrow = -1.0f;
		if (desc.Kind != BlitDesc::DARKEN) {
			lutfirst = (float)lutrow[desc.Translator].first;
			if (desc.AlphaRemap != NULL) {
				int const level = std::max(blit.AlphaLevel, 0);
				alutrow = (float)(alutbase[desc.AlphaRemap] + std::min((261 * level) >> 11, 254));
			}
		}
		float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, lutfirst, alutrow };
		float const t2[4] = { (float)blit.ZFirst, (float)blit.ZFraction, (float)std::max(blit.ZNumerator, 1), (float)blit.ZDenominator };
		float const t3[4] = { (float)blit.ZWrap, (float)blit.ZShape, (float)Shader_Kind(desc), desc.ZRead ? 0.0f : 1.0f };
		Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
	}

	float projection[16];
	Backend_Build_Ortho_Projection(projection, area.Width, area.Height);
	if (shadowed) {
		Submit_Shadows(list, area, projection, ring, shadowquads, tileshadow);
	}
	Set_Map_View(VIEW_MAPDRAW, _Target, area, projection);

	uint64_t const write = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
	bgfx::ViewId view = VIEW_MAPDRAW;
	for (int i = 0; i < quads; i++) {
		int kind = KIND_BASE;
		if (i > 0) {
			GpuBlit const & blit = list.Blits[i - 1];
			kind = blit.Brighten >= 0 ? KIND_BRIGHTEN : (blit.Line >= 0 ? KIND_LINE + blit.LineKind : (blit.Colour >= 0 ? KIND_PIXEL : Shader_Kind(blit.Desc)));
		}
		if (kind == KIND_DARKEN && shadowed) {
			continue;
		}
		uint64_t state = write;
		// The shader outputs the source term in rgb and the target's weight in alpha (fs_mapdraw.sc).
		if (kind == KIND_DARKEN) {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA) | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_REVSUB);
		} else if (kind == KIND_SOURCE75 || kind == KIND_HALF || kind == KIND_SOURCE25 || kind == KIND_LINE) {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA);
		} else if (kind == KIND_ADDITIVE_LINE) {
			state = BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
		} else if (kind == KIND_ANTIALIASED_LINE) {
			// The line leaves the channels it does not blend unwritten.
			int const channels = list.Blits[i - 1].Fade;
			state = BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA)
				| ((channels & GPU_LINE_RED) ? BGFX_STATE_WRITE_R : 0)
				| ((channels & GPU_LINE_GREEN) ? BGFX_STATE_WRITE_G : 0)
				| ((channels & GPU_LINE_BLUE) ? BGFX_STATE_WRITE_B : 0);
		} else if (kind == KIND_BRIGHTEN) {
			// The shader outputs the brightening factor; the target gains itself times it.
			state = BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ONE);
		}
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, 6);
		for (int slot = 0; slot < SLOT_COUNT; slot++) {
			bgfx::setTexture((uint8_t)slot, _Samplers[slot], _Stores[slot].Handle);
		}
		if (i == 0 && shadowed) {
			bgfx::setTexture(SLOT_COUNT, _ShadowSampler, _ShadowTexture[0]);
		}
		if (i == 0) {
			float waves[4] = {};
			if (water) {
				waves[1] = 1.5f * _WaterStrength / 100.0f;
				waves[2] = 0.06f * _WaterStrength / 100.0f;
				waves[3] = 0.4f * _WaterStrength / 100.0f;
				bgfx::setTexture(SLOT_COUNT + 2, _TileShadowSampler, _TileShadow);
			}
			bgfx::setUniform(_WaterUniform, waves);
			bgfx::setUniform(_WorldUniform, world);
		}
		if (kind == KIND_SONIC) {
			// A view's blits run before its draws, so the wave reads everything drawn before it.
			view = (bgfx::ViewId)(view == VIEW_MAPDRAW ? VIEW_MAPWAVE : view + 1);
			Set_Map_View(view, _Target, area, projection);
			bgfx::blit(view, _Snapshot, 0, 0, _TargetTexture);
			bgfx::setTexture(SLOT_COUNT + 1, _SnapshotSampler, _Snapshot);
		}
		bgfx::setUniform(_RingUniform, ring, 2);
		bgfx::setState(state);
		bgfx::submit(view, _Program);
	}

	bool const glow = pass.Glow != NULL && pass.GlowChanged != NULL && _BloomStrength > 0 && bgfx::isValid(_GlowProgram)
		&& pass.Glow->size() == (std::size_t)area.Width * area.Height;
	if (glow) {
		Submit_Glow(pass, area);
	}

	_PostShown = false;
	bool const ripple = pass.Ripples != NULL && !pass.Ripples->empty();
	if (ripple || glow || clouds || weather) {
		int const count = ripple ? std::min((int)pass.Ripples->size(), RIPPLE_LIMIT) : 0;
		float ripples[RIPPLE_LIMIT * 4] = {};
		for (int i = 0; i < count; i++) {
			GpuRipple const & ripple = (*pass.Ripples)[i];
			ripples[i * 4 + 0] = ripple.X;
			ripples[i * 4 + 1] = ripple.Y;
			ripples[i * 4 + 2] = ripple.Radius;
			ripples[i * 4 + 3] = ripple.Strength;
		}
		float const info[4] = { (float)count, (float)area.Width, (float)area.Height, glow ? 1.2f * (float)_BloomStrength / 100.0f : 0.0f };
		Set_Map_View(VIEW_MAPPOST, _PostTarget, area, projection);
		bgfx::setUniform(_RippleUniform, ripples, (uint16_t)std::max(count, 1));
		bgfx::setUniform(_RippleInfoUniform, info);
		bgfx::setUniform(_WorldUniform, world);
		float const falling[4] = { weather ? (float)_Weather : 0.0f, _WeatherStrength / 100.0f, pass.Flash, 0.0f };
		bgfx::setUniform(_WeatherUniform, falling);
		bgfx::setVertexBuffer(0, &buffer, 0, 6);
		bgfx::setTexture(0, _SceneSampler, _TargetTexture);
		if (glow) {
			bgfx::setTexture(1, _GlowSampler, _GlowTexture[0]);
		}
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
		bgfx::submit(VIEW_MAPPOST, _PostProgram);
		_PostShown = true;
	}
	return(true);
}


// Only the pass kept last before a frame is presented is drawn, because passes drawn in one frame
// share the views and the uploaded buffers.
static GpuMapPass _Kept;
static std::vector<GpuRipple> _KeptRipples;
static bool _HasKept = false;


bool GpuMap_Draw(GpuMapPass const & pass)
{
	if (!Start() || !Pass_Fits(pass)) {
		return(false);
	}
	if (_SubmitFailed) {
		_SubmitFailed = false;
		return(false);
	}
	_Kept = pass;
	_KeptRipples = pass.Ripples != NULL ? *pass.Ripples : std::vector<GpuRipple>();
	_Kept.Ripples = &_KeptRipples;
	_HasKept = true;
	return(true);
}


void GpuMap_Drop(void)
{
	_HasKept = false;
}


bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected)
{
	if (_Pending || !Submit_Pass(pass)) {
		return(false);
	}
	bgfx::blit(VIEW_MAPREAD, _Readback, 0, 0, _TargetTexture);
	_ReadyFrame = bgfx::readTexture(_Readback, _Pixels.data());
	_Expected = expected;
	_PendingFrame = pass.Frame;
	_PendingBlits = (int)pass.List->Blits.size();
	_Pending = true;
	return(true);
}


bool GpuMap_Supported(void)
{
	return(Backend_Is_Running() && Start());
}


void GpuMap_Set_Soft_Shadows(int radius)
{
	_SoftShadowRadius = std::clamp(radius, 0, 16);
}


void GpuMap_Set_Bloom(int strength)
{
	_BloomStrength = std::clamp(strength, 0, 400);
}


int GpuMap_Bloom(void)
{
	return(_BloomStrength);
}


void GpuMap_Set_Water(int strength)
{
	_WaterStrength = std::clamp(strength, 0, 400);
}


void GpuMap_Set_Clouds(int strength)
{
	_CloudStrength = std::clamp(strength, 0, 400);
}


void GpuMap_Set_Weather(GpuWeather weather, int strength)
{
	_Weather = weather;
	_WeatherStrength = std::clamp(strength, 0, 400);
}


std::uint16_t GpuMap_Texture(void)
{
	if (_HasKept) {
		_HasKept = false;
		if (Submit_Pass(_Kept)) {
			_TargetDrawn = true;
		} else {
			// The map area holds the key colour, so this frame shows the last picture, if one of the
			// same size, and the next pass is drawn in software.
			_SubmitFailed = true;
			if (!_TargetDrawn || _TargetWidth != _Kept.Area.Width || _TargetHeight != _Kept.Area.Height) {
				return(bgfx::kInvalidHandle);
			}
		}
	}
	return(_PostShown ? _PostTexture.idx : _TargetTexture.idx);
}


void GpuMap_After_Frame(std::uint32_t frame)
{
	_Referenced = false;
	if (!_Pending || frame < _ReadyFrame) {
		return;
	}
	_Pending = false;

	// The GPU picture is compared in 565, the format the software result is in.
	int differing = 0;
	int beyond = 0;
	int largest = 0;
	int first = -1;
	for (std::size_t i = 0; i < _Expected.size(); i++) {
		unsigned char const * rgba = &_Pixels[i * 4];
		int const red = (rgba[0] * 31 + 127) / 255;
		int const green = (rgba[1] * 63 + 127) / 255;
		int const blue = (rgba[2] * 31 + 127) / 255;
		unsigned short const want = _Expected[i];
		int const dr = std::abs(red - (want >> 11));
		int const dg = std::abs(green - ((want >> 5) & 63));
		int const db = std::abs(blue - (want & 31));
		int const diff = std::max(dr, std::max(dg, db));
		if (diff > 1) {
			beyond++;
		}
		if (diff != 0) {
			if (first < 0) {
				first = (int)i;
			}
			differing++;
			largest = std::max(largest, diff);
		}
	}
	DebugString("GPU_DIFF frame %d blits %d pixels %d differing %d over1 %d maxdiff %d first %d,%d\n", _PendingFrame, _PendingBlits, (int)_Expected.size(), differing, beyond, largest,
		first < 0 ? -1 : first % _TargetWidth, first < 0 ? -1 : first / _TargetWidth);
}


/// <summary>
/// Forgets the kept pass and makes the GPU read the buffers the last drawn pass handed it without a
/// copy, by ending a bgfx frame early when one is still waiting. Call it before freeing the depth or
/// alpha buffer.
/// </summary>
void GpuMap_Release_References(void)
{
	GpuMap_Drop();
	if (_Referenced) {
		bgfx::frame();
		_Referenced = false;
	}
}


void GpuMap_Shutdown(void)
{
	GpuMap_Release_References();
	Destroy_Target();
	Destroy_Stores();
	if (bgfx::isValid(_Program)) {
		bgfx::destroy(_Program);
		_Program = BGFX_INVALID_HANDLE;
		for (bgfx::UniformHandle & sampler : _Samplers) {
			bgfx::destroy(sampler);
		}
		bgfx::destroy(_BlurProgram);
		bgfx::destroy(_BlurUniform);
		bgfx::destroy(_MaskSampler);
		bgfx::destroy(_ShadowSampler);
		_BlurProgram = BGFX_INVALID_HANDLE;
		bgfx::destroy(_PostProgram);
		bgfx::destroy(_SceneSampler);
		bgfx::destroy(_RippleUniform);
		bgfx::destroy(_RippleInfoUniform);
		bgfx::destroy(_RingUniform);
		_PostProgram = BGFX_INVALID_HANDLE;
		if (bgfx::isValid(_GlowProgram)) {
			bgfx::destroy(_GlowProgram);
			_GlowProgram = BGFX_INVALID_HANDLE;
		}
		bgfx::destroy(_GlowUniform);
		bgfx::destroy(_WaterUniform);
		bgfx::destroy(_WorldUniform);
		bgfx::destroy(_WeatherUniform);
		bgfx::destroy(_GlowMaskSampler);
		bgfx::destroy(_GlowSampler);
	}
	_Ready = false;
	_Failed = false;
	_Pending = false;
}
