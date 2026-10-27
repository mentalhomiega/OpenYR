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

#include <algorithm>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <utility>

#include "shaders/mapdraw/fs_mapdraw.bin.h"
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
	BGFX_EMBEDDED_SHADER_END()
};


// Width of the textures that hold byte arenas; fs_mapdraw.sc uses the same value.
static int const ARENA_WIDTH = 4096;
static int const TEXTURE_LIMIT = 16384;

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

static bool _Pending = false;
static std::uint32_t _ReadyFrame = 0;
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
	if (bgfx::isValid(_Target)) {
		bgfx::destroy(_Target);
		_Target = BGFX_INVALID_HANDLE;
		_TargetTexture = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_Readback)) {
		bgfx::destroy(_Readback);
		_Readback = BGFX_INVALID_HANDLE;
	}
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
	if (!bgfx::isValid(_Target) || !bgfx::isValid(_Readback)) {
		Destroy_Target();
		return(false);
	}
	_TargetWidth = width;
	_TargetHeight = height;
	_Pixels.assign((std::size_t)width * height * 4, 0);
	return(true);
}


// A point-sampled texture of the bytes, ARENA_WIDTH texels wide; an empty arena gets one texel.
static bgfx::TextureHandle Make_Arena(void const * bytes, std::size_t size)
{
	unsigned char const zero = 0;
	if (size == 0) {
		bytes = &zero;
		size = 1;
	}
	int const width = (int)std::min<std::size_t>(size, ARENA_WIDTH);
	int const rows = (int)((size + ARENA_WIDTH - 1) / ARENA_WIDTH);
	if (rows > TEXTURE_LIMIT) {
		return(bgfx::TextureHandle{ bgfx::kInvalidHandle });
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)width * rows);
	std::memset(memory->data, 0, memory->size);
	std::memcpy(memory->data, bytes, size);
	return(bgfx::createTexture2D((uint16_t)width, (uint16_t)rows, false, 1, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP, memory));
}


static bgfx::TextureHandle Make_R16(unsigned short const * values, int width, int height)
{
	if (width <= 0 || height <= 0 || width > TEXTURE_LIMIT || height > TEXTURE_LIMIT) {
		return(bgfx::TextureHandle{ bgfx::kInvalidHandle });
	}
	return(bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::R16, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP,
		bgfx::copy(values, (uint32_t)((std::size_t)width * height * sizeof(unsigned short)))));
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
		if (blit.Colour >= 0 || desc.Kind == BlitDesc::DARKEN) {
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

	bgfx::TextureHandle textures[SLOT_COUNT];
	textures[SLOT_INDEX] = Make_Arena(list.PixelArena.data(), list.PixelArena.size());
	textures[SLOT_LUT] = Make_R16(lut.data(), 256, (int)(lut.size() / 256));
	textures[SLOT_ALUT] = Make_R16(alut.data(), 256, (int)(alut.size() / 256));
	textures[SLOT_ZBUF] = Make_R16(pass.Depth->data(), area.Width, area.Height);
	textures[SLOT_ABUF] = Make_R16(pass.Alpha->data(), area.Width, area.Height);
	textures[SLOT_ZSHAPE] = Make_Arena(list.ZShapeArena.data(), list.ZShapeArena.size());
	textures[SLOT_BASE] = Make_R16(pass.Base->data(), area.Width, area.Height);
	bool valid = true;
	for (bgfx::TextureHandle texture : textures) {
		valid = valid && bgfx::isValid(texture);
	}

	int const quads = (int)list.Blits.size() + 1;
	if (valid && bgfx::getAvailTransientVertexBuffer(quads * 6, _Layout) < (uint32_t)quads * 6) {
		valid = false;
	}
	if (!valid) {
		DebugString("GPU_DIFF frame %d: the pass does not fit the GPU limits\n", pass.Frame);
		for (bgfx::TextureHandle texture : textures) {
			if (bgfx::isValid(texture)) {
				bgfx::destroy(texture);
			}
		}
		return(false);
	}

	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, quads * 6, _Layout);
	MapVertex * vertex = (MapVertex *)buffer.data;
	{
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, (float)KIND_BASE, 0.0f };
		Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
	}
	for (std::size_t i = 0; i < list.Blits.size(); i++) {
		GpuBlit const & blit = list.Blits[i];
		BlitDesc const & desc = blit.Desc;
		if (blit.Colour >= 0) {
			float const t1[4] = { (float)blit.Colour, 1.0f, 0.0f, -1.0f };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)KIND_PIXEL, 0.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, 1, 1, t1, t2, t3);
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
		float const t1[4] = { (float)blit.Pixels, (float)blit.Width, lutfirst, alutrow };
		float const t2[4] = { (float)blit.ZFirst, (float)blit.ZFraction, (float)std::max(blit.ZNumerator, 1), (float)blit.ZDenominator };
		float const t3[4] = { (float)blit.ZWrap, (float)blit.ZShape, (float)Shader_Kind(desc), 0.0f };
		Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
	}

	float projection[16];
	Backend_Build_Ortho_Projection(projection, area.Width, area.Height);
	bgfx::setViewFrameBuffer(VIEW_MAPDRAW, _Target);
	bgfx::setViewRect(VIEW_MAPDRAW, 0, 0, (uint16_t)area.Width, (uint16_t)area.Height);
	bgfx::setViewTransform(VIEW_MAPDRAW, NULL, projection);
	bgfx::setViewMode(VIEW_MAPDRAW, bgfx::ViewMode::Sequential);
	bgfx::setViewClear(VIEW_MAPDRAW, BGFX_CLEAR_NONE);

	uint64_t const write = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
	for (int i = 0; i < quads; i++) {
		int const kind = i == 0 ? KIND_BASE : (list.Blits[i - 1].Colour >= 0 ? KIND_PIXEL : Shader_Kind(list.Blits[i - 1].Desc));
		uint64_t state = write;
		// The shader outputs the source term in rgb and the target's weight in alpha (fs_mapdraw.sc).
		if (kind == KIND_DARKEN) {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA) | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_REVSUB);
		} else if (kind == KIND_SOURCE75 || kind == KIND_HALF || kind == KIND_SOURCE25) {
			state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA);
		}
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, 6);
		for (int slot = 0; slot < SLOT_COUNT; slot++) {
			bgfx::setTexture((uint8_t)slot, _Samplers[slot], textures[slot]);
		}
		bgfx::setState(state);
		bgfx::submit(VIEW_MAPDRAW, _Program);
	}
	for (bgfx::TextureHandle texture : textures) {
		bgfx::destroy(texture);
	}

	return(true);
}


bool GpuMap_Draw(GpuMapPass const & pass)
{
	return(Submit_Pass(pass));
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


std::uint16_t GpuMap_Texture(void)
{
	return(_TargetTexture.idx);
}


void GpuMap_After_Frame(std::uint32_t frame)
{
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


void GpuMap_Shutdown(void)
{
	Destroy_Target();
	if (bgfx::isValid(_Program)) {
		bgfx::destroy(_Program);
		_Program = BGFX_INVALID_HANDLE;
		for (bgfx::UniformHandle & sampler : _Samplers) {
			bgfx::destroy(sampler);
		}
	}
	_Ready = false;
	_Failed = false;
	_Pending = false;
}
