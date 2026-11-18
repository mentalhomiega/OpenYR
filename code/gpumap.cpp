/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "gpumap.h"
#include "layerfade.h"

#include "alphalighting.h"
#include "bgfxbackend.h"
#include "dbgprint.h"
#include "gpudraw.h"
#include "gpumapvoxel.h"
#include "gpuvoxel.h"
#include "monotonic.h"
#include "perfstat.h"
#include "renderthread.h"
#include "writewatch.h"

#include <algorithm>
#include <atomic>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include "bgfxviews.hh"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <utility>

#include "shaders/mapdraw/fs_mapblur.bin.h"
#include "shaders/mapdraw/fs_mapclear.bin.h"
#include "shaders/mapdraw/fs_mapdraw.bin.h"
#include "shaders/mapdraw/fs_mapglow.bin.h"
#include "shaders/mapdraw/fs_mapmask.bin.h"
#include "shaders/mapdraw/fs_mapplane.bin.h"
#include "shaders/mapdraw/fs_mappost.bin.h"
#include "shaders/mapdraw/vs_mapclear.bin.h"
#include "shaders/mapdraw/vs_mapdraw.bin.h"


static const bgfx::EmbeddedShader _MapShaders[] = {
	BGFX_EMBEDDED_SHADER_D3D11(vs_mapdraw),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapdraw),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapblur),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mappost),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapglow),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapmask),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapplane),
	BGFX_EMBEDDED_SHADER_D3D11(vs_mapclear),
	BGFX_EMBEDDED_SHADER_D3D11(fs_mapclear),
	BGFX_EMBEDDED_SHADER_END()
};


// Width of the textures that hold byte arenas; fs_mapdraw.sc uses the same value.
static int const ARENA_WIDTH = 4096;
static int const TEXTURE_LIMIT = GPU_MAP_STORE_ROWS;
// The most pixels a scaled pass's scene, snapshot and post textures may each have: a little over a 3840x2160 view at
// scale 4, 536.9 MB a texture in RGBA8.
static std::int64_t const SCALED_PIXEL_LIMIT = std::int64_t(1) << 27;
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
	// Only fs_voxelscene.sc draws this kind: a voxel's highlight strength into the emissive mask.
	KIND_VOXEL_STRENGTH,
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
// The target's depth and stencil attachment, made for the first pass with voxel parts and kept from then on.
static bgfx::TextureHandle _DepthTexture = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _Target = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Readback = BGFX_INVALID_HANDLE;
static int _TargetWidth = 0;
static int _TargetHeight = 0;
static int _TargetScale = 1;
// The scale GpuMap_Set_Voxel_Scale asked for; a pass the check reads back is drawn at 1, and a large one lower (Pass_Scale).
static int _VoxelScale = 1;

// Soft shadows: shadows are drawn into a mask, the mask is blurred, and the base is darkened through it.
// With one shadow darkening the ground once (ShadowStacking=no) the mask is drawn and not blurred.
static int _SoftShadowRadius = 0;
static bool _UnionShadows = false;
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
static bgfx::UniformHandle _BaseRingUniform = BGFX_INVALID_HANDLE;
// The scales of a deferred light fade (GpuMapPass::Fade) and the size they were last uploaded at.
static bgfx::UniformHandle _FadeSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _FadeUniform = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _Fade = BGFX_INVALID_HANDLE;
static int _FadeWidth = 0;
static int _FadeHeight = 0;
static int _FadeVersion = 0;
// Whether the base store holds the tile layer's memory as it lies, instead of the area's picture.
static bool _BaseInPlace = false;
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

// The emissive mask of a pass with GPU voxels, built from the CPU plane and the voxel parts' layers; made on first use.
static bgfx::ProgramHandle _MaskProgram = BGFX_INVALID_HANDLE;
// Sets the depth of every pixel of a quad to the far plane and, where the state asks, its stencil value to 0.
static bgfx::ProgramHandle _ClearProgram = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _MaskTexture = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _MaskDepth = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _MaskTarget = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _MaskReadback = BGFX_INVALID_HANDLE;
static bool _MaskBuilt = false;

// The emissive plane of a pass that listed its plane writes, built from Glow and the list; made on first use.
static bgfx::ProgramHandle _PlaneProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _MaskValueSampler = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _PlaneTexture = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _PlaneTarget = BGFX_INVALID_HANDLE;
static int _PlaneWidth = 0;
static int _PlaneHeight = 0;
// The scene target as the game thread sees it, for GpuMap_Plane_Ready: its size, whether it has depth, and the
// frames ended since it was made.
static std::atomic<int> _ReadyWidth = 0;
static std::atomic<int> _ReadyHeight = 0;
static std::atomic<int> _ReadyScale = 0;
static std::atomic<bool> _ReadyDepth = false;
static std::atomic<int> _ReadyFrames = 0;
// The maskcheck step reads the built plane back and compares it with the plane the CPU made at GpuMap_After_Frame.
// A read takes about two frames, so three slots read every pass. A slot's texture only grows, so a read still
// pending when the area changes keeps its texture and buffer.
struct PlaneRead
{
	bgfx::TextureHandle Texture = BGFX_INVALID_HANDLE;
	int TextureWidth = 0;
	int TextureHeight = 0;
	int Width = 0;
	std::vector<unsigned char> Pixels;
	std::vector<unsigned char> Expected;
	std::uint32_t Ready = 0;
	int Frame = 0;
	bool Pending = false;
};
static PlaneRead _PlaneReads[3];
static int _PlanesChecked = 0;
static int _PlanesDiffered = 0;

// Set while a drawn pass's buffers are uploaded by reference and its frame is not yet submitted.
static bool _Referenced = false;
static bool _Pending = false;
static std::uint32_t _ReadyFrame = 0;

// The GPU picture the check reads back with bgfx::readTexture, as RGBA bytes.
static std::vector<unsigned char> _Pixels;
// The software result the check compares with _Pixels, one value per area pixel.
static std::vector<unsigned short> _Expected;
// The emissive mask the check reads back and the software mask it is compared with; empty when not checked.
static std::vector<unsigned char> _MaskPixels;
static std::vector<unsigned char> _ExpectedMask;
static int _PendingFrame = 0;
static int _PendingBlits = 0;

// One pass read back for GpuMap_Count_Shadows: its voxel shadow rectangles in area pixels, the base picture and the target.
struct ShadowCountState
{
	bool Armed = false;
	bool Pending = false;
	std::uint32_t ReadyFrame = 0;
	bgfx::TextureHandle Readback = BGFX_INVALID_HANDLE;
	int Width = 0;
	int Height = 0;
	int Scale = 1;
	std::vector<Rect> Rects;
	std::vector<unsigned short> Base;
	std::vector<unsigned char> Pixels;
};

static ShadowCountState _ShadowCount;


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
	_Program = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mapdraw");
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
	_BlurProgram = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mapblur");
	_SceneSampler = bgfx::createUniform("s_scene", bgfx::UniformType::Sampler);
	_RippleUniform = bgfx::createUniform("u_ripples", bgfx::UniformType::Vec4, RIPPLE_LIMIT);
	_RippleInfoUniform = bgfx::createUniform("u_rippleinfo", bgfx::UniformType::Vec4);
	_RingUniform = bgfx::createUniform("u_ring", bgfx::UniformType::Vec4, 2);
	_BaseRingUniform = bgfx::createUniform("u_basering", bgfx::UniformType::Vec4);
	_FadeSampler = bgfx::createUniform("s_fade", bgfx::UniformType::Sampler);
	_FadeUniform = bgfx::createUniform("u_fade", bgfx::UniformType::Vec4);
	_SnapshotSampler = bgfx::createUniform("s_snapshot", bgfx::UniformType::Sampler);
	_TileShadowSampler = bgfx::createUniform("s_tileshadow", bgfx::UniformType::Sampler);
	_WaterUniform = bgfx::createUniform("u_water", bgfx::UniformType::Vec4);
	_WorldUniform = bgfx::createUniform("u_world", bgfx::UniformType::Vec4);
	_WeatherUniform = bgfx::createUniform("u_weather", bgfx::UniformType::Vec4);
	_PostProgram = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mappost");
	_GlowProgram = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mapglow");
	_GlowUniform = bgfx::createUniform("u_glow", bgfx::UniformType::Vec4);
	_GlowMaskSampler = bgfx::createUniform("s_glowmask", bgfx::UniformType::Sampler);
	_GlowSampler = bgfx::createUniform("s_glow", bgfx::UniformType::Sampler);
	// The GPU map cannot draw its shadows or shockwaves without the blur and shockwave programs, so Start fails
	// without them. The glow pass checks its program before it submits, so a missing glow program only turns glow off.
	if (!bgfx::isValid(_BlurProgram) || !bgfx::isValid(_PostProgram)) {
		DebugString("GPU_DIFF unavailable: the shadow blur or shockwave program did not build\n");
		return(false);
	}
	if (!bgfx::isValid(_GlowProgram)) {
		DebugString("GPU_DIFF: the glow program did not build, so the glow pass is off\n");
	}
	_MaskProgram = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mapmask");
	if (!bgfx::isValid(_MaskProgram)) {
		DebugString("GPU_DIFF: the voxel mask program did not build, so GPU voxels add no emissive mask\n");
	}
	_PlaneProgram = Create_Embedded_Program(_MapShaders, "vs_mapdraw", _MapShaders, "fs_mapplane");
	_MaskValueSampler = bgfx::createUniform("s_maskvalues", bgfx::UniformType::Sampler);
	_ClearProgram = Create_Embedded_Program(_MapShaders, "vs_mapclear", _MapShaders, "fs_mapclear");
	if (!bgfx::isValid(_ClearProgram)) {
		DebugString("GPU_DIFF unavailable: the depth clear program did not build\n");
		return(false);
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


// Sampler flags of the render targets and planes the passes read by pixel position.
static uint64_t const POINT_CLAMP = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
static uint64_t const LINEAR_CLAMP = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;


// The targets the presenter samples, their depth attachments (a frame buffer needs attachments of one size) and the
// textures that a shader bounds by their size are made at the pass's own size and again when it changes: the presenter
// finds a pixel by its texture coordinate, and a larger texture rounds some of them to other texels. The shadow masks are
// read by pixel position inside the area, so they are made for the largest area a pass has had or the view GpuMap_Reserve
// named, and a pass uses their top left corner; a zoom step makes none of them. The other textures are made when a pass
// first uses them.
static int _ReservedWidth = 0;
static int _ReservedHeight = 0;
// Set once a pass has needed a depth attachment: the scene target, and the mask target, keep one from then on.
static bool _DepthWanted = false;
static bool _MaskDepthWanted = false;
static int _ShadowWidth = 0;
static int _ShadowHeight = 0;


// Makes a render target of one colour texture, which the frame buffer owns. False leaves both handles invalid.
static bool Make_Colour_Target(bgfx::TextureHandle & texture, bgfx::FrameBufferHandle & target, int width, int height, bgfx::TextureFormat::Enum format, uint64_t sampler)
{
	texture = Create_Texture(width, height, format, BGFX_TEXTURE_RT | sampler);
	if (bgfx::isValid(texture)) {
		target = bgfx::createFrameBuffer(1, &texture, true);
		if (!bgfx::isValid(target)) {
			Release_Handle(texture);
		}
	}
	return(bgfx::isValid(texture) && bgfx::isValid(target));
}


// Destroys a render target made by Make_Colour_Target.
static void Release_Colour_Target(bgfx::TextureHandle & texture, bgfx::FrameBufferHandle & target)
{
	if (bgfx::isValid(target)) {
		Release_Handle(target);
		texture = BGFX_INVALID_HANDLE;
	}
}


// Makes a depth and stencil texture of width by height pixels; the caller makes the frame buffer that uses it.
static bool Make_Depth(bgfx::TextureHandle & texture, int width, int height)
{
	texture = Create_Texture(width, height, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT_WRITE_ONLY);
	return(bgfx::isValid(texture));
}


// Destroys what depends on the area's size or on the scene target: the targets, the planes and the readbacks. The
// depth attachments and shadow masks stay for the next size.
static void Destroy_Scene(void)
{
	Release_Colour_Target(_PostTexture, _PostTarget);
	_PostShown = false;
	_TargetDrawn = false;
	_ReadyWidth = 0;
	for (int i = 0; i < 2; i++) {
		Release_Colour_Target(_GlowTexture[i], _GlowTarget[i]);
	}
	Release_Handle(_GlowMask);
	_GlowMaskTracked = false;
	Release_Handle(_MaskTarget);
	Release_Handle(_MaskTexture);
	Release_Handle(_MaskReadback);
	_MaskBuilt = false;
	Release_Handle(_Target);
	Release_Handle(_TargetTexture);
	Release_Handle(_DepthTexture);
	Release_Handle(_MaskDepth);
	Release_Handle(_Readback);
	Release_Handle(_ShadowCount.Readback);
	Release_Handle(_Snapshot);
	Release_Handle(_TileShadow);
	_TileShadowTracked = false;
	Release_Handle(_Fade);
	_FadeWidth = 0;
	_FadeHeight = 0;
	_TargetWidth = 0;
	_TargetHeight = 0;
	_TargetScale = 1;
}


static void Destroy_Target(void)
{
	Destroy_Scene();
	for (int i = 0; i < 2; i++) {
		Release_Colour_Target(_ShadowTexture[i], _ShadowTarget[i]);
	}
	_DepthWanted = false;
	_MaskDepthWanted = false;
	_ShadowWidth = 0;
	_ShadowHeight = 0;
}


// The scene and snapshot are scale times the area; every other texture is the area's size. With depth, the scene target
// also has a depth and stencil attachment of its size.
static bool Size_Target(int width, int height, int scale, bool depth)
{
	if (width == _TargetWidth && height == _TargetHeight && scale == _TargetScale && depth == _DepthWanted && bgfx::isValid(_Target)) {
		return(true);
	}
	Destroy_Scene();
	_TargetTexture = Create_Texture(width * scale, height * scale, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | POINT_CLAMP);
	if (!bgfx::isValid(_TargetTexture) || (depth && !Make_Depth(_DepthTexture, width * scale, height * scale))) {
		Destroy_Scene();
		return(false);
	}
	bgfx::TextureHandle const attachments[2] = { _TargetTexture, _DepthTexture };
	_Target = bgfx::createFrameBuffer(depth ? 2 : 1, attachments, false);
	if (!bgfx::isValid(_Target)) {
		Destroy_Scene();
		return(false);
	}
	_TargetWidth = width;
	_TargetHeight = height;
	_TargetScale = scale;
	_ReadyFrames = 0;
	_ReadyWidth = width;
	_ReadyHeight = height;
	_ReadyScale = scale;
	_ReadyDepth = depth;
	_DepthWanted = depth;
	return(true);
}


// Makes the texture, if there is none, as a plane of one byte for each pixel of the area, which the pass fills from the
// area's own bytes; tracked is cleared so that the first upload sends it whole.
static bool Ensure_Plane_Texture(bgfx::TextureHandle & texture, bool & tracked)
{
	if (!bgfx::isValid(texture)) {
		tracked = false;
		texture = Create_Texture(_TargetWidth, _TargetHeight, bgfx::TextureFormat::R8, POINT_CLAMP);
	}
	return(bgfx::isValid(texture));
}


// The copy of the target a sonic wave or a rippled cloak reads.
static bool Ensure_Snapshot(void)
{
	if (!bgfx::isValid(_Snapshot)) {
		_Snapshot = Create_Texture(_TargetWidth * _TargetScale, _TargetHeight * _TargetScale, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | POINT_CLAMP);
	}
	return(bgfx::isValid(_Snapshot));
}


// The picture the check reads back and the buffer it lands in.
static bool Ensure_Readback(void)
{
	if (!bgfx::isValid(_Readback)) {
		_Readback = Create_Texture(_TargetWidth, _TargetHeight, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | POINT_CLAMP);
		_Pixels.assign((std::size_t)_TargetWidth * _TargetHeight * 4, 0);
	}
	return(bgfx::isValid(_Readback));
}


// The post pass's target, which the presenter shows in place of the scene while the post pass runs.
static bool Ensure_Post_Target(void)
{
	return(bgfx::isValid(_PostTarget) || Make_Colour_Target(_PostTexture, _PostTarget, _TargetWidth * _TargetScale, _TargetHeight * _TargetScale, bgfx::TextureFormat::RGBA8, POINT_CLAMP));
}


// The two quarter-size pictures the glow is reduced and blurred in.
static bool Ensure_Glow_Targets(void)
{
	for (int i = 0; i < 2; i++) {
		if (!bgfx::isValid(_GlowTarget[i]) && !Make_Colour_Target(_GlowTexture[i], _GlowTarget[i], (_TargetWidth + 3) / 4, (_TargetHeight + 3) / 4, bgfx::TextureFormat::RGBA8, LINEAR_CLAMP)) {
			return(false);
		}
	}
	return(true);
}


// The two masks the shadows are drawn and blurred in, as large as the biggest area so far.
static bool Ensure_Shadow_Targets(void)
{
	bool const fits = bgfx::isValid(_ShadowTarget[0]) && bgfx::isValid(_ShadowTarget[1]) && _ShadowWidth >= _TargetWidth && _ShadowHeight >= _TargetHeight;
	if (fits) {
		return(true);
	}
	_ShadowWidth = std::max(_TargetWidth, _ReservedWidth);
	_ShadowHeight = std::max(_TargetHeight, _ReservedHeight);
	for (int i = 0; i < 2; i++) {
		Release_Colour_Target(_ShadowTexture[i], _ShadowTarget[i]);
		if (!Make_Colour_Target(_ShadowTexture[i], _ShadowTarget[i], _ShadowWidth, _ShadowHeight, bgfx::TextureFormat::R8, POINT_CLAMP)) {
			return(false);
		}
	}
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
// The values of the listed plane writes that copy values (GPU_MASK_VALUES), for the plane program.
static Store _MaskValueStore;

// The tile layer memory a base store holds the values of, known by its address and its serial (WriteWatch_Serial), since
// freed memory can come back at the same address. The store of the layer not read now is kept, so that a change between
// the screen's tile layer and the zoomed map's makes no texture and sends only what was written to the other meanwhile.
struct BaseKey
{
	void const * Values = NULL;
	unsigned Serial = 0;

	bool operator == (BaseKey const & other) const = default;
};
static BaseKey _BaseKey;
static Store _OtherBase;
static BaseKey _OtherBaseKey;


static void Destroy_Stores(void)
{
	for (Store & store : _Stores) {
		Release_Handle(store.Handle);
		store = Store();
	}
	Release_Handle(_MaskValueStore.Handle);
	_MaskValueStore = Store();
	Release_Handle(_OtherBase.Handle);
	_OtherBase = Store();
	_BaseKey = BaseKey();
	_OtherBaseKey = BaseKey();
}


// Sends memory to an area of a texture, counting its bytes as an upload in the perf log.
static void Update_Texture(bgfx::TextureHandle texture, int x, int y, int width, int height, bgfx::Memory const * memory)
{
	Perf_Count(PERF_COUNT_UPLOAD_BYTES, memory->size);
	Perf_Count(PERF_COUNT_UPLOADS);
	bgfx::updateTexture2D(texture, 0, 0, (uint16_t)x, (uint16_t)y, (uint16_t)width, (uint16_t)height, memory);
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
		Release_Handle(store.Handle);
		int const height = exact ? rows : std::min(TEXTURE_LIMIT, std::max(rows, store.Height) * 2);
		store.Handle = Create_Texture(width, height, format, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
		store.Width = width;
		store.Height = height;
		if (!bgfx::isValid(store.Handle)) {
			store = Store();
			return(false);
		}
	}
	Update_Texture(store.Handle, 0, 0, width, rows, memory);
	return(true);
}


// The rows of ARENA_WIDTH texels that hold size bytes; zero bytes need no rows.
static std::size_t Arena_Rows(std::size_t size)
{
	return((size + ARENA_WIDTH - 1) / ARENA_WIDTH);
}


// Fills the index store: the atlas in its top ATLAS_ROWS rows, uploading only what the store does not
// hold yet, then the pass's arena, ARENA_WIDTH texels a row.
static bool Fill_Index(Store & store, GpuAtlas const & atlas, void const * bytes, std::size_t size)
{
	static unsigned uploaded_epoch = 0;
	static std::size_t uploaded = 0;

	int const rows = std::max(1, (int)Arena_Rows(size));
	if (ATLAS_ROWS + rows > TEXTURE_LIMIT) {
		return(false);
	}
	store.Tracked = false;
	if (!bgfx::isValid(store.Handle) || store.Width != ARENA_WIDTH || store.Height < ATLAS_ROWS + rows) {
		Release_Handle(store.Handle);
		int const height = std::min(TEXTURE_LIMIT, std::max(ATLAS_ROWS + rows, store.Height) * 2);
		store.Handle = Create_Texture(ARENA_WIDTH, height, bgfx::TextureFormat::R8, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
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
		Update_Texture(store.Handle, 0, first, ARENA_WIDTH, last - first, memory);
		uploaded = atlas.Bytes.size();
	}
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)ARENA_WIDTH * rows);
	if (size > 0) {
		std::memcpy(memory->data, bytes, size);
	}
	std::memset(memory->data + size, 0, memory->size - size);
	Update_Texture(store.Handle, 0, ATLAS_ROWS, ARENA_WIDTH, rows, memory);
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
	int const rows = std::max(1, (int)Arena_Rows(size));
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
	Update_Texture(store.Handle, 0, first, width, end - first, bgfx::copy(values + (std::size_t)first * width, size));
}


// Uploads one rectangle of the values, which are width values a row.
static void Update_Rect(Store & store, unsigned short const * values, int width, Rect const & rect)
{
	std::size_t const rowbytes = (std::size_t)rect.Width * sizeof(unsigned short);
	bgfx::Memory const * const memory = bgfx::alloc((uint32_t)(rowbytes * rect.Height));
	for (int r = 0; r < rect.Height; r++) {
		std::memcpy(memory->data + r * rowbytes, values + (std::size_t)(rect.Y + r) * width + rect.X, rowbytes);
	}
	Update_Texture(store.Handle, rect.X, rect.Y, rect.Width, rect.Height, memory);
}


// The upload check keeps a copy of what each watched store was sent and logs an UPLOAD_CHECK DIFFERENCE
// line when the values differ from it after an upload, which means a write was neither named nor listed.
// OPENYR_UPLOAD_CHECK=1 turns it on from the start, and the uploadcheck autotest step turns it on and off.
static bool _UploadChecked = [] {
	char const * const value = std::getenv("OPENYR_UPLOAD_CHECK");
	return(value != NULL && value[0] == '1');
}();
static std::unordered_map<Store const *, std::vector<unsigned short>> _UploadCopies;
static int _UploadDifferences = 0;
static int _NamedUploads = 0;

static bool Upload_Is_Checked(void)
{
	return(_UploadChecked);
}


void GpuMap_Set_Upload_Check(bool enabled)
{
	if (_UploadChecked && !enabled) {
		DebugString("UPLOAD_CHECK differences %d, uploads of named rectangles %s\n", _UploadDifferences, _NamedUploads > 0 ? "yes" : "no");
	}
	_UploadChecked = enabled;
	_UploadCopies.clear();
	_UploadDifferences = 0;
	_NamedUploads = 0;
}


static void Check_Upload(Store const & store, unsigned short const * values, int width, int rows, std::vector<Rect> const & sent)
{
	std::size_t const count = (std::size_t)width * rows;
	auto found = _UploadCopies.find(&store);
	if (found == _UploadCopies.end() || found->second.size() != count) {
		// The first upload seen is taken to leave the store equal to the values.
		_UploadCopies[&store].assign(values, values + count);
		return;
	}
	std::vector<unsigned short> & copy = found->second;
	for (Rect const & rect : sent) {
		for (int r = 0; r < rect.Height; r++) {
			std::size_t const at = (std::size_t)(rect.Y + r) * width + rect.X;
			std::memcpy(&copy[at], values + at, (std::size_t)rect.Width * sizeof(unsigned short));
		}
	}
	if (std::memcmp(copy.data(), values, count * sizeof(unsigned short)) != 0) {
		std::size_t at = 0;
		while (copy[at] == values[at]) {
			at++;
		}
		_UploadDifferences++;
		DebugString("UPLOAD_CHECK DIFFERENCE store %d row %d column %d of %dx%d\n", (int)(&store - _Stores), (int)(at / width), (int)(at % width), width, rows);
		std::memcpy(copy.data(), values, count * sizeof(unsigned short));
	}
}


// Makes the base store the one for the tile layer at values, keeping the store it replaces, with the copy the upload
// check holds of it, as the other.
static void Use_Base_Store(void const * values)
{
	BaseKey const key{ values, WriteWatch_Serial(values) };
	if (key == _BaseKey) {
		return;
	}
	if (key != _OtherBaseKey) {
		Release_Handle(_OtherBase.Handle);
		_OtherBase = Store();
		_OtherBaseKey = key;
		_UploadCopies.erase(&_OtherBase);
	}
	std::swap(_Stores[SLOT_BASE], _OtherBase);
	std::swap(_BaseKey, _OtherBaseKey);
	std::swap(_UploadCopies[&_Stores[SLOT_BASE]], _UploadCopies[&_OtherBase]);
}


// Makes the store exactly width by rows values. When the system lists the pages written since the last
// call (writewatch.h), only those go up: the rectangles named in written, and whole rows for a page that
// no named rectangle touches. Reference is as for Fill_R16.
static bool Fill_R16_Watched(Store & store, unsigned short const * values, int width, int rows, bool reference, WrittenRects * written = NULL)
{
	std::size_t const bytes = (std::size_t)width * rows * sizeof(unsigned short);
	static std::vector<std::pair<std::size_t, std::size_t>> runs;
	static std::vector<Rect> rects;
	static std::vector<Rect> sent;
	bool const listed = WriteWatch_Take(values, bytes, runs);
	bool const named = written != NULL && written->Take(rects);
	sent.clear();
	if (!listed || !store.Tracked || store.Width != width || store.Height != rows) {
		// The copy the check keeps is made now, so the values must be read now as well.
		if (!Fill_R16(store, values, width, rows, true, reference && !Upload_Is_Checked())) {
			return(false);
		}
		store.Tracked = listed;
		if (Upload_Is_Checked()) {
			sent.push_back(Rect(0, 0, width, rows));
			Check_Upload(store, values, width, rows, sent);
		}
		return(true);
	}
	std::size_t const pitch = (std::size_t)width * sizeof(unsigned short);
	std::size_t const page = WriteWatch_Page_Size();
	static std::vector<unsigned char> covered;
	if (named) {
		_NamedUploads += rects.empty() ? 0 : 1;
		covered.assign(bytes / page + 1, 0);
		for (Rect const & rect : rects) {
			Rect const inside = Intersect(rect, Rect(0, 0, width, rows));
			if (!inside.Is_Valid()) {
				continue;
			}
			Update_Rect(store, values, width, inside);
			sent.push_back(inside);
			for (int r = inside.Y; r < inside.Y + inside.Height; r++) {
				std::size_t const first = r * pitch + (std::size_t)inside.X * sizeof(unsigned short);
				std::size_t const last = first + (std::size_t)inside.Width * sizeof(unsigned short) - 1;
				std::fill(covered.begin() + first / page, covered.begin() + last / page + 1, (unsigned char)1);
			}
		}
	}
	int done = 0;
	auto send_rows = [&](std::size_t from, std::size_t to) {
		int const first = std::max(done, (int)(from / pitch));
		int const end = std::min(rows, (int)((to + pitch - 1) / pitch));
		if (first < end) {
			Update_Rows(store, values, width, first, end);
			sent.push_back(Rect(0, first, width, end - first));
			done = end;
		}
	};
	for (auto const & run : runs) {
		if (!named) {
			send_rows(run.first, run.second);
			continue;
		}
		std::size_t at = run.first / page;
		std::size_t const end = (run.second + page - 1) / page;
		while (at < end) {
			if (covered[at] != 0) {
				at++;
				continue;
			}
			std::size_t const from = at;
			while (at < end && covered[at] == 0) {
				at++;
			}
			send_rows(from * page, std::min(at * page, bytes));
		}
	}
	if (Upload_Is_Checked()) {
		Check_Upload(store, values, width, rows, sent);
	}
	return(true);
}


// Calls upload(first, end) for each run of flagged rows, after clearing the run's flags. Flagged rows with
// fewer than GAP unchanged rows between them share a run, so a scattered change takes few uploads.
template<class Upload>
static void For_Each_Changed_Run(std::vector<unsigned char> & changed, int rows, Upload upload)
{
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
		upload(first, end);
	}
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
	For_Each_Changed_Run(changed, rows, [&](int first, int end) {
		Update_Rows(store, values, width, first, end);
	});
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
	For_Each_Changed_Run(changed, rows, [&](int first, int end) {
		Update_Texture(texture, 0, first, width, end - first, bgfx::copy(&values[(std::size_t)first * width], (uint32_t)width * (end - first)));
	});
}


// Uploads the tiles of a plane kept in texture that changed since the GPU last took them, and clears
// their marks. Tracked is cleared when the texture is remade, which sends the whole plane.
static void Upload_Plane_Tiles(bgfx::TextureHandle texture, bool & tracked, std::vector<unsigned char> const & values, int width, int rows, TileGridClass & tiles)
{
	if (!tracked) {
		tiles.Mark_All();
		tracked = true;
	}
	static std::vector<TileRect> rects;
	tiles.Rects(rects);
	for (TileRect const & rect : rects) {
		bgfx::Memory const * memory = bgfx::alloc((uint32_t)rect.Width * rect.Height);
		for (int y = 0; y < rect.Height; y++) {
			std::memcpy(memory->data + (std::size_t)y * rect.Width, &values[(std::size_t)(rect.Y + y) * width + rect.X], (std::size_t)rect.Width);
		}
		Update_Texture(texture, rect.X, rect.Y, rect.Width, rect.Height, memory);
	}
	tiles.Clear();
}


// Sends the scales of a deferred light fade, if the pass has any. False means the base is drawn as it is.
static bool Upload_Fade(GpuMapPass const & pass)
{
	if (pass.Fade == NULL || pass.FadeWidth <= 0 || pass.FadeHeight <= 0 || pass.Fade->size() != (std::size_t)pass.FadeWidth * pass.FadeHeight) {
		return(false);
	}
	bool const fresh = !bgfx::isValid(_Fade) || _FadeWidth != pass.FadeWidth || _FadeHeight != pass.FadeHeight;
	if (fresh) {
		Release_Handle(_Fade);
		_Fade = Create_Texture(pass.FadeWidth, pass.FadeHeight, bgfx::TextureFormat::R16);
		_FadeWidth = pass.FadeWidth;
		_FadeHeight = pass.FadeHeight;
		if (!bgfx::isValid(_Fade)) {
			return(false);
		}
	}
	if (fresh || pass.FadeVersion != _FadeVersion) {
		_FadeVersion = pass.FadeVersion;
		Update_Texture(_Fade, 0, 0, _FadeWidth, _FadeHeight, bgfx::copy(pass.Fade->data(), (uint32_t)(pass.Fade->size() * sizeof(unsigned short))));
	}
	return(true);
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


// The projection maps area pixels, so a view of scale times the area's size draws the same quads finer.
static void Set_Map_View(bgfx::ViewId view, bgfx::FrameBufferHandle target, Rect const & area, float const * projection, int scale = 1)
{
	bgfx::setViewFrameBuffer(view, target);
	bgfx::setViewRect(view, 0, 0, (uint16_t)(area.Width * scale), (uint16_t)(area.Height * scale));
	bgfx::setViewTransform(view, NULL, projection);
	bgfx::setViewMode(view, bgfx::ViewMode::Sequential);
	bgfx::setViewClear(view, BGFX_CLEAR_NONE);
}


// A blit of a voxel part as GpuVoxel_Draw_Direct takes it into a target at scale, with the values of the blit's quad;
// the caller sets its state.
static GpuVoxelDirectBlit Direct_Blit(GpuBlit const & blit, MapVertex const & quad, Rect const & area, int scale)
{
	GpuVoxelDirectBlit direct;
	direct.Part = blit.Voxel;
	direct.OriginX = blit.X - area.X - blit.VoxelX;
	direct.OriginY = blit.Y - area.Y - blit.VoxelY;
	direct.Clip = Intersect(Rect(blit.X - area.X, blit.Y - area.Y, blit.Width, blit.Height), Rect(0, 0, area.Width, area.Height));
	std::memcpy(direct.Values, quad.T1, sizeof(quad.T1));
	std::memcpy(direct.Values + 4, quad.T2, sizeof(quad.T2));
	std::memcpy(direct.Values + 8, quad.T3, sizeof(quad.T3));
	// A voxel blit has no pixels to give a pitch; its depth shape is laid out at the blit's width.
	direct.Values[1] = (float)blit.Width;
	direct.Values[12] = (float)(blit.X - area.X);
	direct.Values[13] = (float)(blit.Y - area.Y);
	direct.Values[14] = (float)scale;
	return(direct);
}


// Binds the pass's stores and their ring addressing for the next draw.
static void Bind_Stores(float const * ring)
{
	for (int slot = 0; slot < SLOT_COUNT; slot++) {
		bgfx::setTexture((uint8_t)slot, _Samplers[slot], _Stores[slot].Handle);
	}
	bgfx::setUniform(_RingUniform, ring, 2);
	bgfx::setUniform(_BaseRingUniform, ring + 8);
}


// Draws every shadow's coverage, with the tile layer's shadows when tileshadow is set, into the first
// shadow mask and, with a soft radius, blurs it there through the second. A voxel part's shadow is drawn from its layers at scale 1,
// the mask's own scale.
static void Submit_Shadows(GpuDrawList const & list, int scale, Rect const & area, float const * projection, float const * ring, int shadowquads, bool tileshadow)
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
	// The blit each quad draws, NULL for the whole-area quads.
	std::vector<GpuBlit const *> blits(shadowquads, NULL);
	int quad = 1;
	if (tileshadow) {
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, (float)KIND_TILE_SHADOW, 1.0f };
		Set_Quad(vertex + 6, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
		quad++;
	}
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Class() != GPU_BLIT_SHADOW) {
			continue;
		}
		float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, 0.0f, -1.0f };
		float const t2[4] = { (float)blit.ZFirst, (float)blit.ZFraction, (float)std::max(blit.ZNumerator, 1), (float)blit.ZDenominator };
		float const t3[4] = { (float)blit.ZWrap, (float)blit.ZShape, (float)KIND_SHADOW_MASK, blit.Desc.ZRead ? 0.0f : 1.0f };
		Set_Quad(vertex + quad * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
		blits[quad] = &blit;
		quad++;
	}

	Set_Map_View(VIEW_MAPSHADOW, _ShadowTarget[0], area, projection);
	bgfx::setViewClear(VIEW_MAPSHADOW, BGFX_CLEAR_COLOR, 0x00000000);
	float const shadow_scale[4] = { (float)scale, 0.0f, 0.0f, 0.0f };
	auto const is_voxel = [&](int index) { return(blits[index] != NULL && blits[index]->Voxel >= 0); };
	// Every other quad draws the same coverage with the same state, so a run of them goes in one draw.
	for (int i = 1; i < shadowquads;) {
		if (is_voxel(i)) {
			GpuVoxelDirectBlit direct = Direct_Blit(*blits[i], vertex[i * 6], area, 1);
			direct.State = BGFX_STATE_WRITE_RGB;
			GpuVoxel_Draw_Direct(list, direct, 1, VIEW_MAPSHADOW, [ring]() { Bind_Stores(ring); });
			i++;
			continue;
		}
		int end = i + 1;
		while (end < shadowquads && !is_voxel(end)) {
			end++;
		}
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, (uint32_t)(end - i) * 6);
		for (int slot = 0; slot < SLOT_COUNT; slot++) {
			bgfx::setTexture((uint8_t)slot, _Samplers[slot], _Stores[slot].Handle);
		}
		bgfx::setUniform(_WaterUniform, shadow_scale);
		if (tileshadow) {
			bgfx::setTexture(SLOT_COUNT + 2, _TileShadowSampler, _TileShadow);
		}
		bgfx::setUniform(_RingUniform, ring, 2);
		bgfx::setUniform(_BaseRingUniform, ring + 8);
		bgfx::setState(BGFX_STATE_WRITE_RGB);
		bgfx::submit(VIEW_MAPSHADOW, _Program);
		i = end;
	}

	if (_SoftShadowRadius == 0) {
		return;
	}
	bgfx::ViewId const views[2] = { VIEW_MAPBLURX, VIEW_MAPBLURY };
	for (int pass = 0; pass < 2; pass++) {
		Set_Map_View(views[pass], _ShadowTarget[1 - pass], area, projection);
		float const blur[4] = { pass == 0 ? 1.0f : 0.0f, pass == 0 ? 0.0f : 1.0f, (float)_SoftShadowRadius, (float)(pass == 0 ? area.Width : area.Height) };
		bgfx::setUniform(_BlurUniform, blur);
		bgfx::setVertexBuffer(0, &buffer, 0, 6);
		bgfx::setTexture(0, _MaskSampler, _ShadowTexture[pass]);
		bgfx::setState(BGFX_STATE_WRITE_RGB);
		bgfx::submit(views[pass], _BlurProgram);
	}
}


// Weights the drawn picture by the emissive mask into the first glow picture, a quarter of the size,
// and blurs it there through the second.
static void Submit_Glow(bgfx::TextureHandle mask, Rect const & area, int scale)
{
	Rect const quarter(0, 0, (area.Width + 3) / 4, (area.Height + 3) / 4);
	float projection[16];
	Backend_Build_Ortho_Projection(projection, quarter.Width, quarter.Height);
	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, 6, _Layout);
	float const t[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Set_Quad((MapVertex *)buffer.data, quarter, 0, 0, quarter.Width, quarter.Height, t, t, t);

	Set_Map_View(VIEW_MAPGLOW, _GlowTarget[0], quarter, projection);
	float const prepare[4] = { (float)area.Width, (float)area.Height, (float)scale, 0.0f };
	bgfx::setUniform(_GlowUniform, prepare);
	bgfx::setVertexBuffer(0, &buffer, 0, 6);
	bgfx::setTexture(0, _SceneSampler, _TargetTexture);
	bgfx::setTexture(1, _GlowMaskSampler, mask);
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


// The quads the mask pass draws: the copy of the CPU plane and one for each voxel blit.
static int Count_Mask_Quads(GpuDrawList const & list)
{
	return(1 + (int)std::count_if(list.Blits.begin(), list.Blits.end(), [](GpuBlit const & blit) { return(blit.Voxel >= 0); }));
}


// Makes the mask target at the area's size, with a depth attachment when depth is set; false when it cannot be made.
static bool Size_Mask_Target(Rect const & area, bool depth)
{
	if (bgfx::isValid(_MaskTarget) && depth == _MaskDepthWanted) {
		return(true);
	}
	Release_Handle(_MaskTarget);
	Release_Handle(_MaskTexture);
	Release_Handle(_MaskDepth);
	_MaskTexture = Create_Texture(area.Width, area.Height, bgfx::TextureFormat::R8, BGFX_TEXTURE_RT | POINT_CLAMP);
	if (bgfx::isValid(_MaskTexture) && (!depth || Make_Depth(_MaskDepth, area.Width, area.Height))) {
		bgfx::TextureHandle const attachments[2] = { _MaskTexture, _MaskDepth };
		_MaskTarget = bgfx::createFrameBuffer(depth ? 2 : 1, attachments, false);
	}
	if (!bgfx::isValid(_MaskTarget)) {
		Release_Handle(_MaskTexture);
		Release_Handle(_MaskDepth);
		return(false);
	}
	_MaskDepthWanted = depth;
	return(true);
}


// fs_voxelscene.sc reads the CPU plane at this stage.
static uint8_t const DIRECT_PLANE_STAGE = 6;

// Sets the depth of view's target to the far plane and, with stencil, its stencil values to 0. The first quad of
// buffer must cover the target.
static void Submit_Depth_Reset(bgfx::ViewId view, bgfx::TransientVertexBuffer const & buffer, bool stencil)
{
	bgfx::setVertexBuffer(0, &buffer, 0, 6);
	bgfx::setState(BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_ALWAYS);
	if (stencil) {
		bgfx::setStencil(BGFX_STENCIL_TEST_ALWAYS | BGFX_STENCIL_FUNC_REF(0) | BGFX_STENCIL_FUNC_RMASK(0xFF)
			| BGFX_STENCIL_OP_FAIL_S_REPLACE | BGFX_STENCIL_OP_FAIL_Z_REPLACE | BGFX_STENCIL_OP_PASS_Z_REPLACE);
	}
	bgfx::submit(view, _ClearProgram);
}

// Makes the plane target at the area's size; false when it cannot be made.
static bool Size_Plane_Target(Rect const & area)
{
	if (bgfx::isValid(_PlaneTarget) && _PlaneWidth == area.Width && _PlaneHeight == area.Height) {
		return(true);
	}
	Release_Colour_Target(_PlaneTexture, _PlaneTarget);
	_PlaneWidth = area.Width;
	_PlaneHeight = area.Height;
	return(Make_Colour_Target(_PlaneTexture, _PlaneTarget, _PlaneWidth, _PlaneHeight, bgfx::TextureFormat::R8, POINT_CLAMP));
}


// The quads Submit_Plane draws: the plane as the pass found it and one for each listed write.
static int Count_Plane_Quads(GpuDrawList const & list)
{
	return(1 + (int)list.MaskOps.size());
}


// Builds the emissive plane of a pass that listed its plane writes into _PlaneTexture: Glow, the plane as the pass found
// it, then each write in order. No quad tests or writes depth, so the writes land in their order in one draw.
static bool Submit_Plane(GpuMapPass const & pass, float const * projection, float const * ring)
{
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	int const quads = Count_Plane_Quads(list);
	// No write reads the store when the pass copied no values, so a store from an earlier pass serves.
	bool const novalues = list.MaskValues.empty() && bgfx::isValid(_MaskValueStore.Handle);
	if (!Size_Plane_Target(area) || !(novalues || Fill_Arena(_MaskValueStore, list.MaskValues.data(), list.MaskValues.size()))
		|| bgfx::getAvailTransientVertexBuffer((uint32_t)quads * 6, _Layout) < (uint32_t)quads * 6) {
		return(false);
	}
	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, (uint32_t)quads * 6, _Layout);
	MapVertex * vertex = (MapVertex *)buffer.data;
	float const none[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, none, none, none);
	for (std::size_t i = 0; i < list.MaskOps.size(); i++) {
		GpuMaskOp const & op = list.MaskOps[i];
		float t1[4] = { 0.0f, (float)op.Width, (float)op.Value, 0.0f };
		float t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float t3[4] = { 0.0f, -1.0f, 2.0f, 1.0f };
		if (op.Kind == GPU_MASK_VALUES) {
			t1[0] = (float)op.Values;
			t3[2] = 3.0f;
		} else if (op.Kind == GPU_MASK_SHAPE) {
			GpuBlit const & blit = list.Blits[op.Blit];
			t1[0] = Index_Offset(blit);
			t1[1] = (float)blit.Pitch;
			t1[3] = (float)blit.Width;
			t2[0] = (float)blit.ZFirst;
			t2[1] = (float)blit.ZFraction;
			t2[2] = (float)std::max(blit.ZNumerator, 1);
			t2[3] = (float)blit.ZDenominator;
			t3[0] = (float)blit.ZWrap;
			t3[1] = (float)blit.ZShape;
			t3[2] = 1.0f;
			t3[3] = op.Depth ? 0.0f : 1.0f;
		}
		Set_Quad(vertex + (i + 1) * 6, area, op.X, op.Y, op.Width, op.Height, t1, t2, t3);
	}
	Set_Map_View(VIEW_MAPPLANE, _PlaneTarget, area, projection);
	bgfx::setVertexBuffer(0, &buffer, 0, (uint32_t)quads * 6);
	Bind_Stores(ring);
	bgfx::setTexture(SLOT_COUNT, _GlowMaskSampler, _GlowMask);
	bgfx::setTexture(SLOT_COUNT + 1, _MaskValueSampler, _MaskValueStore.Handle);
	bgfx::setState(BGFX_STATE_WRITE_R);
	bgfx::submit(VIEW_MAPPLANE, _PlaneProgram);
	return(true);
}


// Starts reading the built plane back for the maskcheck step, unless an earlier read is still pending.
static void Read_Plane_Back(GpuMapPass const & pass)
{
	PlaneRead * read = std::find_if(std::begin(_PlaneReads), std::end(_PlaneReads), [](PlaneRead const & slot) { return(!slot.Pending); });
	if (read == std::end(_PlaneReads) || pass.MaskExpected == NULL) {
		return;
	}
	Rect const & area = pass.Area;
	if (!bgfx::isValid(read->Texture) || area.Width > read->TextureWidth || area.Height > read->TextureHeight) {
		Release_Handle(read->Texture);
		read->TextureWidth = std::max(read->TextureWidth, area.Width);
		read->TextureHeight = std::max(read->TextureHeight, area.Height);
		uint64_t const point = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
		read->Texture = bgfx::createTexture2D((uint16_t)read->TextureWidth, (uint16_t)read->TextureHeight, false, 1, bgfx::TextureFormat::R8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | point);
		read->Pixels.assign((std::size_t)read->TextureWidth * read->TextureHeight, 0);
	}
	if (!bgfx::isValid(read->Texture)) {
		return;
	}
	bgfx::blit(VIEW_MAPREAD, read->Texture, 0, 0, _PlaneTexture, 0, 0, (uint16_t)area.Width, (uint16_t)area.Height);
	read->Ready = bgfx::readTexture(read->Texture, read->Pixels.data());
	read->Width = area.Width;
	read->Expected = *pass.MaskExpected;
	read->Frame = pass.Frame;
	read->Pending = true;
}


// Fills the emissive mask of a pass with GPU voxels: the plane, then, in blit order, the highlight strengths of each
// voxel blit where the plane still holds the mark (GPU_VOXEL_MASK_MARK). A blit zeroes the marked pixels of its
// rectangle and draws its part's layers at scale 1, each pixel taking the voxel the scene shows there. False when the
// mask cannot be drawn.
static bool Submit_Voxel_Mask(GpuMapPass const & pass, float const * projection, float const * ring, bgfx::TextureHandle plane)
{
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	bool const voxels = !list.Voxels.empty();
	if (!Size_Mask_Target(area, voxels || _MaskDepthWanted)) {
		return(false);
	}
	int const quads = Count_Mask_Quads(list);
	if (bgfx::getAvailTransientVertexBuffer((uint32_t)quads * 6, _Layout) < (uint32_t)quads * 6) {
		return(false);
	}
	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, (uint32_t)quads * 6, _Layout);
	MapVertex * vertex = (MapVertex *)buffer.data;
	float const none[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, none, none, none);
	// The blit each quad draws, NULL for the copy of the plane.
	std::vector<GpuBlit const *> blits(quads, NULL);
	int quad = 1;
	for (GpuBlit const & blit : list.Blits) {
		if (blit.Voxel < 0) {
			continue;
		}
		float const zero[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		Set_Quad(vertex + quad * 6, area, blit.X, blit.Y, blit.Width, blit.Height, zero, none, none);
		blits[quad] = &blit;
		quad++;
	}
	Set_Map_View(VIEW_MAPMASK, _MaskTarget, area, projection);
	if (voxels) {
		bgfx::setViewClear(VIEW_MAPMASK, BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
	}
	MapVertex strength = {};
	strength.T3[2] = (float)KIND_VOXEL_STRENGTH;
	// The plane's mark already shows where the blit passed the map's depth test, so the layers skip it.
	strength.T3[3] = 1.0f;
	int slices = 0;
	for (int i = 0; i < quads; i++) {
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, 6);
		bgfx::setTexture(0, _GlowMaskSampler, plane);
		bgfx::setState(BGFX_STATE_WRITE_R);
		bgfx::submit(VIEW_MAPMASK, _MaskProgram);
		GpuBlit const * blit = blits[i];
		// A pass gpucheck compares with the software, whose composites have no highlights, gives them none.
		bool const highlights = pass.Persistent || (blit != NULL && !list.Voxels[blit->Voxel].Scratch);
		if (blit != NULL && blit->Class() == GPU_BLIT_IMAGE && highlights) {
			if (slices == GPU_VOXEL_SLICES) {
				Submit_Depth_Reset(VIEW_MAPMASK, buffer, false);
				slices = 0;
			}
			GpuVoxelDirectBlit drawn = Direct_Blit(*blit, strength, area, 1);
			drawn.Slice = slices++;
			drawn.State = BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL;
			GpuVoxel_Draw_Direct(list, drawn, 1, VIEW_MAPMASK, [ring, plane]() {
				Bind_Stores(ring);
				bgfx::setTexture(DIRECT_PLANE_STAGE, _GlowMaskSampler, plane);
			});
		}
	}
	return(true);
}


// True while shadows go into the mask, blurred or not.
static bool Shadow_Mask_On(void)
{
	return(_SoftShadowRadius > 0 || _UnionShadows);
}


static int Count_Shadow_Blits(GpuDrawList const & list)
{
	return((int)std::count_if(list.Blits.begin(), list.Blits.end(), [](GpuBlit const & blit) {
		return(blit.Class() == GPU_BLIT_SHADOW);
	}));
}


static int Count_Sonic_Waves(GpuDrawList const & list)
{
	return((int)std::count_if(list.Blits.begin(), list.Blits.end(), [](GpuBlit const & blit) {
		return(blit.Line >= 0 && blit.LineKind == GPU_LINE_SONIC);
	}));
}


// The scale a pass over area is drawn at: the one GpuMap_Set_Voxel_Scale asked for, lowered until the scaled textures fit
// TEXTURE_LIMIT and SCALED_PIXEL_LIMIT.
static int Pass_Scale(Rect const & area)
{
	int scale = _VoxelScale;
	while (scale > 1 && (area.Width * scale > TEXTURE_LIMIT || area.Height * scale > TEXTURE_LIMIT || (std::int64_t)area.Width * area.Height * scale * scale > SCALED_PIXEL_LIMIT)) {
		scale--;
	}
	return(scale);
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
	std::size_t const arenarows = Arena_Rows(list.PixelArena.size());
	std::size_t const zshaperows = Arena_Rows(list.ZShapeArena.size());
	if (ATLAS_ROWS + arenarows > (std::size_t)TEXTURE_LIMIT || zshaperows > (std::size_t)TEXTURE_LIMIT) {
		return(false);
	}
	// Arena offsets reach the shader as floats, which hold every whole number only up to 2^24.
	std::size_t const exact = std::size_t(1) << 24;
	if (GPU_ATLAS_BYTES + list.PixelArena.size() > exact || list.ZShapeArena.size() > exact) {
		return(false);
	}
	for (GpuRing const * ring : { &pass.DepthRing, &pass.AlphaRing, &pass.BaseRing }) {
		if (pass.Persistent && ring->Values != NULL && (ring->Width > TEXTURE_LIMIT || ring->Height > TEXTURE_LIMIT)) {
			return(false);
		}
	}
	if (Count_Sonic_Waves(list) > VIEW_MAPWAVE_LAST - VIEW_MAPWAVE + 1) {
		return(false);
	}
	if (!GpuVoxel_Fits(list)) {
		return(false);
	}
	int const shadows = Count_Shadow_Blits(list);
	// At most the vertices Submit_Pass takes, which allocates them later in the same frame.
	uint32_t const vertices = (uint32_t)((list.Blits.size() + 1 + (Shadow_Mask_On() ? shadows + 2 : 0) + (pass.Mask ? Count_Mask_Quads(list) : 0)
		+ (pass.MaskListed ? Count_Plane_Quads(list) : 0)) * 6);
	return(bgfx::getAvailTransientVertexBuffer(vertices, _Layout) >= vertices);
}


// Every translator the list draws through is stacked into one texture of 256-entry rows, and
// every lighting remap into another, 256 rows each.
static void Build_Translator_Stacks(GpuDrawList const & list, std::unordered_map<unsigned short const *, std::pair<int, int>> & lutrow, std::unordered_map<AlphaLightingRemapClass const *, int> & alutbase, std::vector<unsigned short> & lut, std::vector<unsigned short> & alut)
{
	std::unordered_map<AlphaLightingRemapClass const *, int> bandrows;
	for (GpuBlit const & blit : list.Blits) {
		BlitDesc const & desc = blit.Desc;
		if (blit.Class() != GPU_BLIT_IMAGE) {
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
}


// Writes one u_ring entry (mapcommon.sh): the ring's texel of the area's top left pixel and the ring's size.
static void Set_Ring(float * ring, int column, int row, int width, int height)
{
	ring[0] = (float)column;
	ring[1] = (float)row;
	ring[2] = (float)width;
	ring[3] = (float)height;
}


// Uploads the translator stacks and the pass's depth, alpha, shape and base stores. ring receives
// the depth, alpha and base ring addressing that the shadow and blit shaders read. False means a store
// could not take its data.
static bool Upload_Pass_Stores(GpuMapPass const & pass, std::vector<unsigned short> const & lut, std::vector<unsigned short> const & alut, float * ring)
{
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	bool valid = Fill_Index(_Stores[SLOT_INDEX], GpuDraw_Atlas(), list.PixelArena.data(), list.PixelArena.size());
	valid = Fill_R16(_Stores[SLOT_LUT], lut.data(), 256, (int)(lut.size() / 256), false) && valid;
	valid = Fill_R16(_Stores[SLOT_ALUT], alut.data(), 256, (int)(alut.size() / 256), false) && valid;
	// A pass the GPU has not read yet still holds the buffers' memory, so this one is copied, and so is every pass while a render thread can read them after the frame is handed over.
	bool const reference = pass.Persistent && !_Referenced && !RenderThread_Running();
	if (reference) {
		static bool registered = false;
		if (!registered) {
			registered = true;
			std::atexit(GpuMap_Release_References);
		}
		_Referenced = true;
	}
	if (pass.Persistent) {
		GpuRing const * const rings[2] = { &pass.DepthRing, &pass.AlphaRing };
		Store * const stores[2] = { &_Stores[SLOT_ZBUF], &_Stores[SLOT_ABUF] };
		for (int i = 0; i < 2; i++) {
			GpuRing const & source = *rings[i];
			if (source.Values != NULL) {
				valid = Fill_R16_Watched(*stores[i], source.Values, source.Width, source.Height, reference, source.Written) && valid;
				Set_Ring(ring + i * 4, source.Start % source.Width, source.Start / source.Width, source.Width, source.Height);
			} else if (!bgfx::isValid(stores[i]->Handle)) {
				// No blit reads this store, so it only has to exist.
				static unsigned short const unread = 0;
				valid = Fill_R16(*stores[i], &unread, 1, 1, true) && valid;
			}
		}
	} else {
		valid = Fill_R16(_Stores[SLOT_ZBUF], pass.Depth->data(), area.Width, area.Height, true) && valid;
		valid = Fill_R16(_Stores[SLOT_ABUF], pass.Alpha->data(), area.Width, area.Height, true) && valid;
	}
	valid = Fill_Arena(_Stores[SLOT_ZSHAPE], list.ZShapeArena.data(), list.ZShapeArena.size()) && valid;
	// A store filled one way holds nothing the other way can add to.
	bool const inplace = pass.Persistent && pass.BaseRing.Values != NULL;
	if (inplace != _BaseInPlace) {
		_BaseInPlace = inplace;
		_Stores[SLOT_BASE].Tracked = false;
	}
	if (!inplace) {
		_BaseKey = BaseKey();
	}
	if (inplace) {
		GpuRing const & source = pass.BaseRing;
		Use_Base_Store(source.Values);
		valid = Fill_R16_Watched(_Stores[SLOT_BASE], source.Values, source.Width, source.Height, reference, source.Written) && valid;
		Set_Ring(ring + 8, source.Start % source.Width, source.Start / source.Width, source.Width, source.Height);
	} else if (pass.Persistent && pass.BaseChanged != NULL) {
		valid = Fill_R16_Changed(_Stores[SLOT_BASE], pass.Base->data(), area.Width, area.Height, *pass.BaseChanged) && valid;
	} else {
		valid = Fill_R16(_Stores[SLOT_BASE], pass.Base->data(), area.Width, area.Height, true) && valid;
	}
	return(valid);
}


// Writes the base quad, then one quad per blit, into vertex. Returns each blit's shader kind in blit
// order; the draw loop reads these back.
static std::vector<int> Fill_Pass_Vertices(GpuDrawList const & list, Rect const & area, bool shadowed, MapVertex * vertex, std::unordered_map<unsigned short const *, std::pair<int, int>> & lutrow, std::unordered_map<AlphaLightingRemapClass const *, int> & alutbase)
{
	{
		float const t1[4] = { 0.0f, 0.0f, 0.0f, -1.0f };
		float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float const t3[4] = { 0.0f, -1.0f, (float)(shadowed ? KIND_SHADOWED_BASE : KIND_BASE), 0.0f };
		Set_Quad(vertex, area, area.X, area.Y, area.Width, area.Height, t1, t2, t3);
	}
	std::vector<int> kinds(list.Blits.size(), KIND_BASE);
	for (std::size_t i = 0; i < list.Blits.size(); i++) {
		GpuBlit const & blit = list.Blits[i];
		BlitDesc const & desc = blit.Desc;
		GpuBlitClass const blitclass = blit.Class();
		if (blitclass == GPU_BLIT_LIGHT) {
			float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, (float)blit.Brighten, -1.0f };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)KIND_BRIGHTEN, 1.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			kinds[i] = KIND_BRIGHTEN;
			continue;
		}
		if (blitclass == GPU_BLIT_LINE) {
			float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, (float)blit.Line, (float)blit.Fade };
			float t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float t3[4] = { 0.0f, -1.0f, (float)(KIND_LINE + blit.LineKind), 1.0f };
			if (blit.LineKind == GPU_LINE_SONIC && blit.Line >= GPU_WAVE_CUSTOM) {
				// A line has no depth, so a custom wave colour rides in the depth fields: the scales, then the constants.
				t2[0] = (float)blit.Tint[0];
				t2[1] = (float)blit.Tint[1];
				t2[2] = (float)blit.Tint[2];
				t2[3] = (float)blit.Tint[3];
				t3[0] = (float)blit.Tint[4];
				t3[1] = (float)blit.Tint[5];
			}
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			kinds[i] = KIND_LINE + blit.LineKind;
			continue;
		}
		if (blitclass == GPU_BLIT_FILL) {
			float const t1[4] = { (float)blit.Colour, 1.0f, 0.0f, -1.0f };
			float const t2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			float const t3[4] = { 0.0f, -1.0f, (float)KIND_PIXEL, 0.0f };
			Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
			kinds[i] = KIND_PIXEL;
			continue;
		}
		float lutfirst = 0.0f;
		float alutrow = -1.0f;
		if (blitclass == GPU_BLIT_IMAGE) {
			lutfirst = (float)lutrow[desc.Translator].first;
			if (desc.AlphaRemap != NULL) {
				int const level = std::max(blit.AlphaLevel, 0);
				alutrow = (float)(alutbase[desc.AlphaRemap] + std::min((261 * level) >> 11, 254));
			}
		}
		int const kind = Shader_Kind(desc);
		kinds[i] = kind;
		float const t1[4] = { Index_Offset(blit), (float)blit.Pitch, lutfirst, alutrow };
		float const t2[4] = { (float)blit.ZFirst, (float)blit.ZFraction, (float)std::max(blit.ZNumerator, 1), (float)blit.ZDenominator };
		float const t3[4] = { (float)blit.ZWrap, (float)blit.ZShape, (float)kind, desc.ZRead ? 0.0f : 1.0f };
		Set_Quad(vertex + (i + 1) * 6, area, blit.X, blit.Y, blit.Width, blit.Height, t1, t2, t3);
	}
	return(kinds);
}


// The blend state and colour write mask for a quad of the given shader kind. channels is the
// GPU_LINE_* mask of an antialiased line and is ignored for every other kind.
static uint64_t Blend_State_For(int kind, int channels)
{
	uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
	// The shader outputs the source term in rgb and the target's weight in alpha (fs_mapdraw.sc).
	if (kind == KIND_DARKEN) {
		state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA) | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_REVSUB);
	} else if (kind == KIND_SOURCE75 || kind == KIND_HALF || kind == KIND_SOURCE25 || kind == KIND_LINE) {
		state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA);
	} else if (kind == KIND_ADDITIVE_LINE) {
		state = BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
	} else if (kind == KIND_ANTIALIASED_LINE) {
		// The line leaves the channels it does not blend unwritten.
		state = BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_SRC_ALPHA)
			| ((channels & GPU_LINE_RED) ? BGFX_STATE_WRITE_R : 0)
			| ((channels & GPU_LINE_GREEN) ? BGFX_STATE_WRITE_G : 0)
			| ((channels & GPU_LINE_BLUE) ? BGFX_STATE_WRITE_B : 0);
	} else if (kind == KIND_BRIGHTEN) {
		// The shader outputs the brightening factor; the target gains itself times it.
		state = BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ONE);
	}
	return(state);
}


static bool Is_Translucent(GpuBlit const & blit)
{
	return(blit.Class() == GPU_BLIT_IMAGE && blit.Desc.Kind != BlitDesc::XLAT);
}


// The stencil state that lets a draw pass each pixel once, through the next of the pass's stencil values.
static uint32_t Once_Stencil(int & stencils)
{
	return(BGFX_STENCIL_TEST_NOTEQUAL | BGFX_STENCIL_FUNC_REF(++stencils) | BGFX_STENCIL_FUNC_RMASK(0xFF)
		| BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_REPLACE);
}


// A voxel blit drawn into the scene. A copy writes the next depth slice. A translucent blit takes the next slice too
// and blends once a pixel where the depth its pre-pass wrote (Depth_Pre_Pass) is equal, which is its nearest voxel,
// drawing last first so that of voxels at equal depth the one a copy would show blends. A shadow darkens each pixel
// once. slices must be below GPU_VOXEL_SLICES and stencils below GPU_VOXEL_STENCILS.
static GpuVoxelDirectBlit Scene_Blit(GpuBlit const & blit, MapVertex const & quad, Rect const & area, int scale, int & slices, int & stencils)
{
	GpuVoxelDirectBlit direct = Direct_Blit(blit, quad, area, scale);
	if (blit.Class() == GPU_BLIT_SHADOW) {
		direct.State = Blend_State_For(KIND_DARKEN, 0);
		direct.Stencil = Once_Stencil(stencils);
	} else if (Is_Translucent(blit)) {
		direct.Slice = slices++;
		direct.State = Blend_State_For(Shader_Kind(blit.Desc), 0) | BGFX_STATE_DEPTH_TEST_EQUAL;
		direct.Stencil = Once_Stencil(stencils);
		direct.LastFirst = true;
	} else {
		direct.Slice = slices++;
		direct.State = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL;
	}
	return(direct);
}


// A voxel blit of a cloaked object, which GpuDraw_Voxel_Cloak marks.
static bool Is_Cloaked(GpuBlit const & blit)
{
	return(blit.Cloaked && Is_Translucent(blit));
}


// The cloaked voxel blits of the pass, for the check's GPU_VOXEL_CLOAK line; Rippled counts those drawn with the
// ripple.
struct CloakCount
{
	int Blits = 0;
	int Rippled = 0;
};

static CloakCount _CloakCount;


// Starts the pass's next view, which copies the target as the draws before it left it; a draw in the view reads the copy
// as s_snapshot. A sonic wave and a rippled cloak each take one of the VIEW_MAPWAVE views.
static bgfx::ViewId Begin_Snapshot_View(bgfx::ViewId view, Rect const & area, float const * projection, int scale)
{
	view = (bgfx::ViewId)(view == VIEW_MAPDRAW ? VIEW_MAPWAVE : view + 1);
	Set_Map_View(view, _Target, area, projection, scale);
	// A view's blits run before its draws, so the copy holds everything drawn before this view.
	if (Ensure_Snapshot()) {
		bgfx::blit(view, _Snapshot, 0, 0, _TargetTexture);
	}
	return(view);
}


// Makes a cloaked blit draw with the ripple of fs_voxelscene.sc. The shader blends against the view's copy of the
// target and writes the result, so the draw has no blend state.
static void Ripple_Blit(GpuVoxelDirectBlit & direct, GpuBlit const & blit)
{
	direct.Values[15] = (float)(blit.Predator + 1);
	direct.State = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_EQUAL;
}


// The draw that writes a translucent blit's depth, and no colour, before the blit blends.
static GpuVoxelDirectBlit Depth_Pre_Pass(GpuVoxelDirectBlit const & blended)
{
	GpuVoxelDirectBlit depth = blended;
	depth.State = BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL;
	depth.Stencil = 0;
	depth.LastFirst = false;
	return(depth);
}


// Whether the quad of the blit before quad can join a draw of blit quads with the given blend state. Quads drawn in one
// draw blend in their order, as separate draws would, because none of them tests or writes depth or stencil.
static bool Joins_Draw(GpuDrawList const & list, std::vector<int> const & kinds, int quad, uint64_t state, bool shadowed)
{
	GpuBlit const & blit = list.Blits[quad - 1];
	int const kind = kinds[quad - 1];
	if (blit.Voxel >= 0 || kind == KIND_SONIC || (kind == KIND_DARKEN && shadowed)) {
		return(false);
	}
	return(Blend_State_For(kind, blit.Fade) == state);
}


// Draws the base quad and then each blit's quad into the target, in blit order; a voxel blit submits its part's layers
// instead. A sonic wave copies the target first, so it reads everything drawn before it. Neighbouring blit quads with
// the same blend state go in one draw.
static void Submit_Draws(GpuDrawList const & list, int scale, std::vector<int> const & kinds, bgfx::TransientVertexBuffer const & buffer, Rect const & area, float const * projection, float const * ring, float const * world, bool shadowed, bool water, bool faded)
{
	int const quads = (int)list.Blits.size() + 1;
	Set_Map_View(VIEW_MAPDRAW, _Target, area, projection, scale);
	if (!list.Voxels.empty()) {
		bgfx::setViewClear(VIEW_MAPDRAW, BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.0f, 0);
	}
	MapVertex const * const vertices = (MapVertex const *)buffer.data;
	int slices = 0;
	int stencils = 0;
	bgfx::ViewId view = VIEW_MAPDRAW;
	// The views left for rippled cloaks once every sonic wave has its own.
	int ripple_views = GpuVoxel_Cloak_Ripple() ? VIEW_MAPWAVE_LAST - VIEW_MAPWAVE + 1 - Count_Sonic_Waves(list) : 0;
	_CloakCount = CloakCount();
	for (int i = 0; i < quads; i++) {
		int kind = KIND_BASE;
		if (i > 0) {
			kind = kinds[i - 1];
		}
		if (kind == KIND_DARKEN && shadowed) {
			continue;
		}
		if (i > 0 && list.Blits[i - 1].Voxel >= 0) {
			GpuBlit const & blit = list.Blits[i - 1];
			// No blit reads the depth or stencil value an earlier blit left, so emptying both changes no pixel.
			if (slices == GPU_VOXEL_SLICES || stencils == GPU_VOXEL_STENCILS) {
				Submit_Depth_Reset(view, buffer, true);
				slices = 0;
				stencils = 0;
			}
			GpuVoxelDirectBlit drawn = Scene_Blit(blit, vertices[i * 6], area, scale, slices, stencils);
			bool const cloaked = Is_Cloaked(blit);
			bool const rippled = cloaked && blit.Predator >= 0 && ripple_views > 0;
			_CloakCount.Blits += cloaked;
			if (rippled) {
				ripple_views--;
				_CloakCount.Rippled++;
				view = Begin_Snapshot_View(view, area, projection, scale);
				Ripple_Blit(drawn, blit);
			}
			auto const bind = [ring, world, rippled]() {
				Bind_Stores(ring);
				if (rippled) {
					bgfx::setTexture(SLOT_COUNT + 1, _SnapshotSampler, _Snapshot);
					bgfx::setUniform(_WorldUniform, world);
				}
			};
			if (Is_Translucent(blit)) {
				GpuVoxel_Draw_Direct(list, Depth_Pre_Pass(drawn), scale, view, bind);
			}
			GpuVoxel_Draw_Direct(list, drawn, scale, view, bind);
			continue;
		}
		uint64_t const state = Blend_State_For(kind, i > 0 ? list.Blits[i - 1].Fade : 0);
		int joined = 1;
		if (i > 0 && kind != KIND_SONIC) {
			while (i + joined < quads && Joins_Draw(list, kinds, i + joined, state, shadowed)) {
				joined++;
			}
		}
		bgfx::setVertexBuffer(0, &buffer, (uint32_t)i * 6, (uint32_t)joined * 6);
		for (int slot = 0; slot < SLOT_COUNT; slot++) {
			bgfx::setTexture((uint8_t)slot, _Samplers[slot], _Stores[slot].Handle);
		}
		if (i == 0 && shadowed) {
			bgfx::setTexture(SLOT_COUNT, _ShadowSampler, _ShadowTexture[0]);
		}
		if (i == 0) {
			float water_params[4] = { (float)scale, 0.0f, 0.0f, 0.0f };
			if (water) {
				water_params[1] = 1.5f * _WaterStrength / 100.0f;
				water_params[2] = 0.06f * _WaterStrength / 100.0f;
				water_params[3] = 0.4f * _WaterStrength / 100.0f;
				bgfx::setTexture(SLOT_COUNT + 2, _TileShadowSampler, _TileShadow);
			}
			bgfx::setUniform(_WaterUniform, water_params);
			bgfx::setUniform(_WorldUniform, world);
			float const fade_params[4] = { faded ? 1.0f : 0.0f, (float)LAYER_FADE_DIVISOR * _FadeWidth, (float)LAYER_FADE_DIVISOR * _FadeHeight, 65535.0f / LAYER_FADE_UNIT };
			bgfx::setUniform(_FadeUniform, fade_params);
			if (faded) {
				bgfx::setTexture(SLOT_COUNT + 3, _FadeSampler, _Fade, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
			}
		}
		if (kind == KIND_SONIC) {
			view = Begin_Snapshot_View(view, area, projection, scale);
			bgfx::setTexture(SLOT_COUNT + 1, _SnapshotSampler, _Snapshot);
		}
		bgfx::setUniform(_RingUniform, ring, 2);
		bgfx::setUniform(_BaseRingUniform, ring + 8);
		bgfx::setState(state);
		bgfx::submit(view, _Program);
		i += joined - 1;
	}
}


// Draws the ripples, bloom, clouds and weather over the picture the draws left in the target. The
// pass is shown only when one of those effects applies.
static void Submit_Post(GpuMapPass const & pass, int scale, Rect const & area, float const * projection, bgfx::TransientVertexBuffer const & buffer, float const * world, bool glow, bool clouds, bool weather)
{
	_PostShown = false;
	bool const ripple = pass.Ripples != NULL && !pass.Ripples->empty();
	if ((ripple || glow || clouds || weather) && Ensure_Post_Target()) {
		int const count = ripple ? std::min((int)pass.Ripples->size(), RIPPLE_LIMIT) : 0;
		float ripples[RIPPLE_LIMIT * 4] = {};
		for (int i = 0; i < count; i++) {
			GpuRipple const & entry = (*pass.Ripples)[i];
			ripples[i * 4 + 0] = entry.X;
			ripples[i * 4 + 1] = entry.Y;
			ripples[i * 4 + 2] = entry.Radius;
			ripples[i * 4 + 3] = entry.Strength;
		}
		float const info[4] = { (float)count, (float)area.Width, (float)area.Height, glow ? 1.2f * (float)_BloomStrength / 100.0f : 0.0f };
		Set_Map_View(VIEW_MAPPOST, _PostTarget, area, projection, scale);
		bgfx::setUniform(_RippleUniform, ripples, (uint16_t)std::max(count, 1));
		bgfx::setUniform(_RippleInfoUniform, info);
		bgfx::setUniform(_WorldUniform, world);
		float const falling[4] = { weather ? (float)_Weather : 0.0f, _WeatherStrength / 100.0f, pass.Flash, (float)scale };
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
}


static bool Submit_Pass(GpuMapPass const & pass, int scale)
{
	if (!Start()) {
		return(false);
	}
	GpuDrawList const & list = *pass.List;
	Rect const & area = pass.Area;
	if (!Size_Target(area.Width, area.Height, scale, !list.Voxels.empty() || _DepthWanted)) {
		DebugString("GPU_DIFF frame %d: no %dx%d target\n", pass.Frame, area.Width, area.Height);
		return(false);
	}

	std::unordered_map<unsigned short const *, std::pair<int, int>> lutrow;
	std::unordered_map<AlphaLightingRemapClass const *, int> alutbase;
	std::vector<unsigned short> lut;
	std::vector<unsigned short> alut;
	Build_Translator_Stacks(list, lutrow, alutbase, lut, alut);

	if (!list.Voxels.empty() && !GpuVoxel_Submit(list, scale)) {
		DebugString("GPU_DIFF frame %d: the voxel parts cannot be drawn\n", pass.Frame);
		return(false);
	}

	// The shader finds an area pixel's depth and alpha through ring addressing; an unwrapped copy is
	// a ring of the area's size that starts at 0.
	float ring[12];
	Set_Ring(ring, 0, 0, area.Width, area.Height);
	Set_Ring(ring + 4, 0, 0, area.Width, area.Height);
	Set_Ring(ring + 8, 0, 0, area.Width, area.Height);
	bool valid = Upload_Pass_Stores(pass, lut, alut, ring);

	int const shadows = Shadow_Mask_On() ? Count_Shadow_Blits(list) : 0;
	int const waves = Count_Sonic_Waves(list);
	if (waves > VIEW_MAPWAVE_LAST - VIEW_MAPWAVE + 1) {
		DebugString("GPU_DIFF frame %d: %d sonic waves are more than the pass draws\n", pass.Frame, waves);
		return(false);
	}
	bool const plane = pass.TileShadow != NULL && pass.TileShadowChanged != NULL && pass.TileShadow->size() == (std::size_t)area.Width * area.Height
		&& (_SoftShadowRadius > 0 || _WaterStrength > 0) && Ensure_Plane_Texture(_TileShadow, _TileShadowTracked);
	bool const tileshadow = plane && _SoftShadowRadius > 0;
	bool const water = plane && _WaterStrength > 0;
	if (tileshadow || water) {
		Upload_Plane(_TileShadow, _TileShadowTracked, *pass.TileShadow, area.Width, area.Height, *pass.TileShadowChanged);
	}
	bool const faded = Upload_Fade(pass);
	bool const shadowed = (shadows > 0 || tileshadow) && Ensure_Shadow_Targets();
	bool const clouds = _CloudStrength > 0;
	bool const weather = _Weather != GPU_WEATHER_NONE && _WeatherStrength > 0;
	float world[4] = { (float)pass.WorldX, (float)pass.WorldY, 0.0f, clouds ? 0.3f * _CloudStrength / 100.0f : 0.0f };
	if (water || clouds || weather || GpuVoxel_Cloak_Ripple()) {
		// Wrapped every 1,000 seconds, so the float keeps its precision.
		long long const ms = Monotonic_Milliseconds();
		world[2] = (float)(ms % 1000000) / 1000.0f;
	}
	int const quads = (int)list.Blits.size() + 1;
	int const shadowquads = shadowed ? shadows + 1 + (tileshadow ? 1 : 0) : 0;
	bool const planed = pass.Glow != NULL && pass.GlowTiles != NULL && pass.Glow->size() == (std::size_t)area.Width * area.Height && Ensure_Plane_Texture(_GlowMask, _GlowMaskTracked);
	// A listed plane is built before anything is drawn from it, so a pass whose plane cannot be built is not drawn.
	bool const listed = planed && pass.MaskListed;
	int const maskquads = (pass.Mask ? Count_Mask_Quads(list) : 0) + (listed ? Count_Plane_Quads(list) : 0);
	if (valid && bgfx::getAvailTransientVertexBuffer((quads + shadowquads + maskquads) * 6, _Layout) < (uint32_t)(quads + shadowquads + maskquads) * 6) {
		valid = false;
	}
	if (listed && (!bgfx::isValid(_PlaneProgram) || !Size_Plane_Target(area))) {
		valid = false;
	}
	if (planed && !listed) {
		// Made ahead, so the first pass that builds a plane does not draw into a target made in its own frame.
		Size_Plane_Target(area);
	}
	if (!valid) {
		DebugString("GPU_DIFF frame %d: the pass does not fit the GPU limits\n", pass.Frame);
		return(false);
	}

	bgfx::TransientVertexBuffer buffer;
	bgfx::allocTransientVertexBuffer(&buffer, quads * 6, _Layout);
	std::vector<int> const kinds = Fill_Pass_Vertices(list, area, shadowed, (MapVertex *)buffer.data, lutrow, alutbase);

	float projection[16];
	Backend_Build_Ortho_Projection(projection, area.Width, area.Height);
	if (shadowed) {
		Submit_Shadows(list, scale, area, projection, ring, shadowquads, tileshadow);
	}
	Submit_Draws(list, scale, kinds, buffer, area, projection, ring, world, shadowed, water, faded);

	bgfx::TextureHandle emissive = _GlowMask;
	if (planed) {
		Upload_Plane_Tiles(_GlowMask, _GlowMaskTracked, *pass.Glow, area.Width, area.Height, *pass.GlowTiles);
	}
	if (listed && Submit_Plane(pass, projection, ring)) {
		emissive = _PlaneTexture;
		Read_Plane_Back(pass);
	}
	_MaskBuilt = planed && pass.Mask && bgfx::isValid(_MaskProgram) && Submit_Voxel_Mask(pass, projection, ring, emissive);
	bool const glow = planed && _BloomStrength > 0 && bgfx::isValid(_GlowProgram) && Ensure_Glow_Targets();
	if (glow) {
		Submit_Glow(_MaskBuilt ? _MaskTexture : emissive, area, scale);
	}
	Submit_Post(pass, scale, area, projection, buffer, world, glow, clouds, weather);
	return(true);
}


// Only the pass kept last before a frame is presented is drawn, because passes drawn in one frame
// share the views and the uploaded buffers.
static GpuMapPass _Kept;
static std::vector<GpuRipple> _KeptRipples;
static bool _HasKept = false;


bool GpuMap_Draw(GpuMapPass const & pass)
{
	if (!Start()) {
		return(false);
	}
	if (!Pass_Fits(pass)) {
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


bool GpuMap_Check(GpuMapPass const & pass, std::vector<unsigned short> const & expected, std::vector<unsigned char> const * expected_mask)
{
	if (_Pending || !Submit_Pass(pass, 1) || !Ensure_Readback()) {
		return(false);
	}
	if (_CloakCount.Blits > 0) {
		DebugString("GPU_VOXEL_CLOAK frame %d blits %d ripple %d\n", pass.Frame, _CloakCount.Blits, _CloakCount.Rippled);
	}
	bgfx::blit(VIEW_MAPREAD, _Readback, 0, 0, _TargetTexture);
	_ReadyFrame = bgfx::readTexture(_Readback, _Pixels.data());
	_ExpectedMask.clear();
	if (expected_mask != NULL && _MaskBuilt) {
		if (!bgfx::isValid(_MaskReadback)) {
			_MaskReadback = Create_Texture(pass.Area.Width, pass.Area.Height, bgfx::TextureFormat::R8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | POINT_CLAMP);
			_MaskPixels.assign((std::size_t)pass.Area.Width * pass.Area.Height, 0);
		}
		if (bgfx::isValid(_MaskReadback)) {
			bgfx::blit(VIEW_MAPREAD, _MaskReadback, 0, 0, _MaskTexture);
			_ReadyFrame = std::max(_ReadyFrame, bgfx::readTexture(_MaskReadback, _MaskPixels.data()));
			_ExpectedMask = *expected_mask;
		}
	}
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


void GpuMap_Preload_Base(GpuRing const & ring)
{
	if (ring.Values == NULL || !GpuMap_Supported()) {
		return;
	}
	BaseKey const key{ ring.Values, WriteWatch_Serial(ring.Values) };
	if (key == _BaseKey || key == _OtherBaseKey) {
		return;
	}
	Release_Handle(_OtherBase.Handle);
	_OtherBase = Store();
	_OtherBaseKey = key;
	static std::vector<std::pair<std::size_t, std::size_t>> runs;
	static std::vector<Rect> rects;
	bool const listed = WriteWatch_Take(ring.Values, (std::size_t)ring.Width * ring.Height * sizeof(unsigned short), runs);
	if (ring.Written != NULL) {
		ring.Written->Take(rects);
	}
	if (Fill_R16(_OtherBase, ring.Values, ring.Width, ring.Height, true)) {
		_OtherBase.Tracked = listed;
		_UploadCopies.erase(&_OtherBase);
		if (Upload_Is_Checked()) {
			_UploadCopies[&_OtherBase].assign(ring.Values, ring.Values + (std::size_t)ring.Width * ring.Height);
		}
	}
}


void GpuMap_Reserve(int width, int height)
{
	_ReservedWidth = width;
	_ReservedHeight = height;
}


void GpuMap_Set_Soft_Shadows(int radius, bool union_shadows)
{
	_SoftShadowRadius = std::clamp(radius, 0, 16);
	_UnionShadows = union_shadows;
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


void GpuMap_Set_Voxel_Scale(int scale)
{
	_VoxelScale = std::clamp<int>(scale, 1, GPU_VOXEL_SCALE_MAX);
}


int GpuMap_Scale(void)
{
	return(_TargetScale);
}


void GpuMap_Set_Weather(GpuWeather weather, int strength)
{
	_Weather = weather;
	_WeatherStrength = std::clamp(strength, 0, 400);
}


void GpuMap_Count_Shadows(void)
{
	_ShadowCount.Armed = true;
}


// Records the voxel shadow blits of the pass drawn just now and starts reading its picture back.
static void Begin_Shadow_Count(GpuMapPass const & pass, int scale)
{
	Rect const area(0, 0, pass.Area.Width, pass.Area.Height);
	if (!bgfx::isValid(_ShadowCount.Readback)) {
		_ShadowCount.Readback = Create_Texture(area.Width * scale, area.Height * scale, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP);
	}
	if (!bgfx::isValid(_ShadowCount.Readback)) {
		return;
	}
	_ShadowCount.Armed = false;
	_ShadowCount.Width = area.Width;
	_ShadowCount.Height = area.Height;
	_ShadowCount.Scale = scale;
	_ShadowCount.Rects.clear();
	for (GpuBlit const & blit : pass.List->Blits) {
		if (blit.Voxel >= 0 && blit.Class() == GPU_BLIT_SHADOW) {
			_ShadowCount.Rects.push_back(Intersect(Rect(blit.X - pass.Area.X, blit.Y - pass.Area.Y, blit.Width, blit.Height), area));
		}
	}
	_ShadowCount.Base = *pass.Base;
	_ShadowCount.Pixels.assign((std::size_t)area.Width * area.Height * scale * scale * 4, 0);
	bgfx::blit(VIEW_MAPREAD, _ShadowCount.Readback, 0, 0, _TargetTexture);
	_ShadowCount.ReadyFrame = bgfx::readTexture(_ShadowCount.Readback, _ShadowCount.Pixels.data());
	_ShadowCount.Pending = true;
}


// A darkened target channel is half of the base channel; the margin allows for the shader's rounding and 565 to 8-bit.
static bool Is_Darkened(unsigned char const * rgba, unsigned short base)
{
	int const r = (base >> 11) * 255 / 31;
	int const g = ((base >> 5) & 63) * 255 / 63;
	int const b = (base & 31) * 255 / 31;
	return(r + g + b > 60 && std::abs(rgba[0] * 2 - r) <= 16 && std::abs(rgba[1] * 2 - g) <= 16 && std::abs(rgba[2] * 2 - b) <= 16);
}


// True when the target pixel holds, to within one 8-bit step, the base darkened two or three times the way the darken
// blend does it in the 8-bit target (Blend_State_For, fs_mapdraw.sc). An object's pixel is a 565 colour widened to 8
// bits, which such a value rarely equals in all three channels.
static bool Is_Darkened_Again(unsigned char const * rgba, unsigned short base)
{
	int const maximum[3] = { 31, 63, 31 };
	int const ground[3] = { base >> 11, (base >> 5) & 63, base & 31 };
	float value[3];
	for (int c = 0; c < 3; c++) {
		value[c] = std::round(ground[c] * 255.0f / maximum[c]);
	}
	for (int times = 1; times <= 3; times++) {
		bool matches = true;
		for (int c = 0; c < 3; c++) {
			value[c] = std::max(0.0f, std::round(value[c] * 0.5f - 255.0f * 0.25f / maximum[c]));
			matches = matches && std::abs(rgba[c] - value[c]) <= 1.0f;
		}
		if (times >= 2 && matches && value[0] + value[1] + value[2] > 30.0f) {
			return(true);
		}
	}
	return(false);
}


static void Finish_Shadow_Count(void)
{
	// How many shadow rectangles cover each area pixel; ground that only one covers is darkened again only by its blit.
	std::vector<unsigned char> cover((std::size_t)_ShadowCount.Width * _ShadowCount.Height, 0);
	for (Rect const & rect : _ShadowCount.Rects) {
		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				unsigned char & count = cover[(std::size_t)y * _ShadowCount.Width + x];
				count = (unsigned char)std::min(count + 1, 255);
			}
		}
	}
	int pixels = 0;
	int darkened = 0;
	int twice = 0;
	for (Rect const & rect : _ShadowCount.Rects) {
		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				pixels++;
				int const tx = x * _ShadowCount.Scale + _ShadowCount.Scale / 2;
				int const ty = y * _ShadowCount.Scale + _ShadowCount.Scale / 2;
				unsigned char const * rgba = &_ShadowCount.Pixels[((std::size_t)ty * _ShadowCount.Width * _ShadowCount.Scale + tx) * 4];
				std::size_t const at = (std::size_t)y * _ShadowCount.Width + x;
				if (Is_Darkened(rgba, _ShadowCount.Base[at])) {
					darkened++;
				} else if (cover[at] == 1 && Is_Darkened_Again(rgba, _ShadowCount.Base[at])) {
					twice++;
				}
			}
		}
	}
	DebugString("GPU_VOXEL_SHADOW shadows %d pixels %d darkened %d twice %d\n", (int)_ShadowCount.Rects.size(), pixels, darkened, twice);
}


std::uint16_t GpuMap_Texture(void)
{
	if (_HasKept) {
		_HasKept = false;
		int const scale = Pass_Scale(_Kept.Area);
		if (Submit_Pass(_Kept, scale)) {
			_TargetDrawn = true;
			if (_ShadowCount.Armed) {
				Begin_Shadow_Count(_Kept, scale);
			}
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


bool GpuMap_Plane_Ready(Rect const & area, bool voxels)
{
	return(_ReadyFrames >= 2 && _ReadyWidth == area.Width && _ReadyHeight == area.Height && _ReadyScale == Pass_Scale(area)
		&& (_ReadyDepth || !voxels));
}


void GpuMap_Plane_Check_Counts(int & checked, int & differed)
{
	checked = _PlanesChecked;
	differed = _PlanesDiffered;
	_PlanesChecked = 0;
	_PlanesDiffered = 0;
}


std::uint16_t GpuMap_Mask_Texture(void)
{
	return(_MaskBuilt ? _MaskTexture.idx : bgfx::kInvalidHandle);
}


void GpuMap_After_Frame(std::uint32_t frame)
{
	_Referenced = false;
	if (_ReadyFrames < 2) {
		_ReadyFrames++;
	}
	for (PlaneRead & read : _PlaneReads) {
		if (!read.Pending || frame < read.Ready) {
			continue;
		}
		read.Pending = false;
		int differing = 0;
		int largest = 0;
		for (std::size_t i = 0; i < read.Expected.size(); i++) {
			int const gpu = read.Pixels[(i / read.Width) * read.TextureWidth + i % read.Width];
			int const diff = std::abs((int)read.Expected[i] - gpu);
			differing += diff != 0;
			largest = std::max(largest, diff);
		}
		DebugString("MASK_PLANE frame %d pixels %d differing %d maxdiff %d\n", read.Frame, (int)read.Expected.size(), differing, largest);
		_PlanesChecked++;
		_PlanesDiffered += differing != 0;
	}
	if (_ShadowCount.Pending && frame >= _ShadowCount.ReadyFrame) {
		_ShadowCount.Pending = false;
		Finish_Shadow_Count();
	}
	if (!_Pending || frame < _ReadyFrame) {
		return;
	}
	_Pending = false;
	if (!_ExpectedMask.empty()) {
		int masked = 0;
		int differing = 0;
		int largest = 0;
		for (std::size_t i = 0; i < _ExpectedMask.size(); i++) {
			int const software = _ExpectedMask[i];
			int const gpu = _MaskPixels[i];
			masked += (software != 0 || gpu != 0) ? 1 : 0;
			if (software != gpu) {
				differing++;
				largest = std::max(largest, std::abs(software - gpu));
			}
		}
		DebugString("GPU_MASK frame %d pixels %d masked %d differing %d maxdiff %d\n", _PendingFrame, (int)_ExpectedMask.size(), masked, differing, largest);
		_ExpectedMask.clear();
	}

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
	GpuVoxel_Shutdown();
	// The samplers exist only once the main program has built.
	if (bgfx::isValid(_Program)) {
		for (bgfx::UniformHandle & sampler : _Samplers) {
			Release_Handle(sampler);
		}
	}
	Release_Handle(_Program);
	Release_Colour_Target(_PlaneTexture, _PlaneTarget);
	for (PlaneRead & read : _PlaneReads) {
		Release_Handle(read.Texture);
		read = PlaneRead();
	}
	for (bgfx::ProgramHandle * program : { &_BlurProgram, &_PostProgram, &_GlowProgram, &_MaskProgram, &_PlaneProgram, &_ClearProgram }) {
		Release_Handle(*program);
	}
	for (bgfx::UniformHandle * uniform : { &_BlurUniform, &_MaskSampler, &_ShadowSampler, &_SceneSampler, &_RippleUniform, &_RippleInfoUniform,
		&_RingUniform, &_BaseRingUniform, &_SnapshotSampler, &_TileShadowSampler, &_FadeSampler, &_FadeUniform, &_GlowUniform, &_WaterUniform, &_WorldUniform, &_WeatherUniform,
		&_GlowMaskSampler, &_GlowSampler, &_MaskValueSampler }) {
		Release_Handle(*uniform);
	}
	_Ready = false;
	_Failed = false;
	_Pending = false;
}
