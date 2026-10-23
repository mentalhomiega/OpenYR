/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The bgfx side of the presenter. This is the only translation unit that includes bgfx,
// which keeps the library's headers and build settings away from the rest of the engine.

#include "perfstat.h"
#include "bgfxbackend.h"

#include "dbgprint.h"
#include "except.h"

#include "bgfxviews.hh"
#include "hdrcolour.h"

#include <algorithm>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <bx/allocator.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fs_ocornut_imgui.bin.h>
#include <malloc.h>
#include <vs_ocornut_imgui.bin.h>
#include "shaders/fs_composite.bin.h"


static const bgfx::EmbeddedShader _EmbeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER_END()
};


// The composite shader ships for Direct3D 11 only. Other renderers get no program, so HDR output
// falls back to SDR and the colour adjustment has no effect.
static const bgfx::EmbeddedShader _CompositeShaders[] = {
	{
		"fs_composite",
		{
			{ bgfx::RendererType::Direct3D11, fs_composite_dxbc, sizeof(fs_composite_dxbc) },
			{ bgfx::RendererType::Count, NULL, 0 },
		}
	},
	BGFX_EMBEDDED_SHADER_END()
};


static bool _Initialized = false;

static bgfx::TextureHandle _FrameTexture = BGFX_INVALID_HANDLE;
static bgfx::ProgramHandle _Program = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _TextureSampler = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _PrescaleTarget = BGFX_INVALID_HANDLE;
static bgfx::VertexLayout _VertexLayout;

static int _FrameWidth = 0;
static int _FrameHeight = 0;

static bool _FrameUploaded = false;

static bool _FramePending = false;
static bool _FramePointSampled = false;

static int _PrescaleWidth = 0;
static int _PrescaleHeight = 0;
static int _DrawableWidth = 0;
static int _DrawableHeight = 0;
static unsigned int _ResetFlags = BGFX_RESET_FLIP_AFTER_RENDER;

// True while the frame texture holds the game's own 565 layout. When the hardware cannot
// sample that format the frame is widened to 32 bits on the way in instead.
static bool _FrameIs565 = false;
static unsigned int * _ConvertBuffer = NULL;
static unsigned int _ConvertTable[65536];

// The layer under the frame, and the frame copy with see-through key pixels drawn over it.
static bgfx::TextureHandle _LayerTexture = BGFX_INVALID_HANDLE;
static int _LayerWidth = 0;
static int _LayerHeight = 0;
static bool _LayerUploaded = false;
static bool _KeyedFrameUploaded = false;
static unsigned int * _LayerConvertBuffer = NULL;
static bgfx::TextureHandle _KeyedFrameTexture = BGFX_INVALID_HANDLE;
static unsigned int * _KeyedFrameBuffer = NULL;

// HDR output and the colour adjustment. Frames are routed through the scene target when HDR10 output
// is on or the colour adjustment is not the identity; the composite pass then draws them to the window.
static bool _HdrActive = false;
static bool _Routed = false;
static bool _ShotRequested = false;
static bgfx::TextureHandle _ReadbackTexture = BGFX_INVALID_HANDLE;
static std::vector<unsigned char> _ReadbackPixels;
static std::string _ReadbackPath;
static bool _ReadbackPending = false;
static uint32_t _ReadbackFrame = 0;
static uint32_t _LastFrame = 0;
static bgfx::FrameBufferHandle _SceneTarget = BGFX_INVALID_HANDLE;
static bgfx::FrameBufferHandle _ShotTarget = BGFX_INVALID_HANDLE;
static bgfx::ProgramHandle _CompositeProgram = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _CompositeLevels = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _ColourMatrix = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _ColourTone = BGFX_INVALID_HANDLE;
static float _PaperWhiteNits = (float)HDR_PAPER_WHITE_DEFAULT;
static float _PeakNits = (float)HDR_PEAK_FALLBACK_NITS;
static HDRColourAdjust _ColourAdjust = {{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 1.0f};

// An emissive mask uploaded as an R8 texture. Uploaded says whether the texture holds a mask for
// the picture it follows, so the presenter draws it only then.
struct BackendMask
{
	bgfx::TextureHandle Handle;
	int Width;
	int Height;
	bool Uploaded;
};

static BackendMask _FrameMask = {BGFX_INVALID_HANDLE, 0, 0, false};
static BackendMask _LayerMask = {BGFX_INVALID_HANDLE, 0, 0, false};
static bool _MaskView = false;

// The emissive mask target is drawn at the drawable size, in the same places as the colour, so the
// composite reads the mask at its own UV. The keyed mask is the frame mask with alpha zero where the
// frame has the layer key, so the layer mask shows through there, as the layer colour does.
static bgfx::FrameBufferHandle _MaskTarget = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _MaskSampler = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle _MaskLevels = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle _KeyedMaskTexture = BGFX_INVALID_HANDLE;
static unsigned int * _KeyedMaskBuffer = NULL;
static bool _KeyedMaskUploaded = false;
// Set by any mask draw this frame. Without one the composite reads no mask.
static bool _MaskDrawn = false;


struct BackendVertex
{
	float X;
	float Y;
	float U;
	float V;
	unsigned int Color;
};


// bgfx reports lost devices and shader failures through this rather than a return code,
// so the engine would otherwise present to a black window with no explanation.
class BackendCallback : public bgfx::CallbackI
{
	public:
		virtual ~BackendCallback(void) override {}

		virtual void fatal(const char * filepath, uint16_t line, bgfx::Fatal::Enum code, const char * str) override
		{
			// A debug check is the library's own assertion, not a renderer failure. The ones it
			// runs while shutting down compare reference counts on interfaces that an overlay
			// or the Direct3D debug layer is free to hold, so ending the process over one would
			// report somebody else's reference as a crash.
			if (code == bgfx::Fatal::DebugCheck) {
				DebugString("Renderer check failed at %s(%u): %s\n",
							filepath != NULL ? filepath : "", (unsigned)line, str != NULL ? str : "");
				return;
			}

			Fatal("Renderer error %d at %s(%u): %s", (int)code,
						filepath != NULL ? filepath : "", (unsigned)line, str != NULL ? str : "");
		}

		virtual void traceVargs(const char * filepath, uint16_t line, const char * format, va_list argList) override
		{
			char message[1024];
			vsnprintf(message, sizeof(message), format, argList);
			OutputDebugString(message);
		}

		virtual void profilerBegin(const char *, uint32_t, const char *, uint16_t) override {}
		virtual void profilerBeginLiteral(const char *, uint32_t, const char *, uint16_t) override {}
		virtual void profilerEnd(void) override {}
		virtual uint32_t cacheReadSize(uint64_t) override { return(0); }
		virtual bool cacheRead(uint64_t, void *, uint32_t) override { return(false); }
		virtual void cacheWrite(uint64_t, const void *, uint32_t) override {}
		// Writes a capture of the window as a 32-bit TGA file.
		virtual void screenShot(const char * filepath, uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum, const void * data, uint32_t, bool yflip) override
		{
			FILE * file = std::fopen(filepath, "wb");
			if (file == NULL) {
				return;
			}
			unsigned char header[18] = {};
			header[2] = 2;
			header[12] = (unsigned char)(width & 0xFF);
			header[13] = (unsigned char)(width >> 8);
			header[14] = (unsigned char)(height & 0xFF);
			header[15] = (unsigned char)(height >> 8);
			header[16] = 32;
			header[17] = yflip ? 0x08 : 0x28;
			std::fwrite(header, 1, sizeof(header), file);
			for (uint32_t y = 0; y < height; y++) {
				std::fwrite((unsigned char const *)data + y * pitch, 4, width, file);
			}
			std::fclose(file);
		}
		virtual void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
		virtual void captureEnd(void) override {}
		virtual void captureFrame(const void *, uint32_t) override {}
};

static BackendCallback _Callback;


// bgfx contains cache-line-aligned render records but requests their backing arrays with
// the allocator's default alignment. The Win32 CRT only guarantees eight-byte alignment,
// which is insufficient when clang-cl copies those records with aligned SSE instructions.
class BackendAllocator : public bx::AllocatorI
{
	public:
		virtual ~BackendAllocator(void) override {}

		virtual void * realloc(void * ptr, size_t size, size_t alignment, const char *, uint32_t) override
		{
			if (size == 0) {
				_aligned_free(ptr);
				return(NULL);
			}

			const size_t cachelinealignment = BX_CACHE_LINE_SIZE;
			alignment = std::max(alignment, cachelinealignment);
			return(_aligned_realloc(ptr, size, alignment));
		}
};

static BackendAllocator _Allocator;


/// <summary>
/// Builds the table that widens a 565 pixel to the 32 bit color the fallback path uploads.
/// </summary>
static void Build_Convert_Table(void)
{
	for (int pixel = 0; pixel < 65536; pixel++) {
		unsigned int red = (unsigned int)(((pixel >> 11) & 0x1F) * 255 / 31);
		unsigned int green = (unsigned int)(((pixel >> 5) & 0x3F) * 255 / 63);
		unsigned int blue = (unsigned int)((pixel & 0x1F) * 255 / 31);

		_ConvertTable[pixel] = 0xFF000000 | (red << 16) | (green << 8) | blue;
	}
}


/// <summary>
/// Submits one textured rectangle covering the given destination. False means the
/// transient vertex memory ran out and nothing was submitted.
/// </summary>
static bool Submit_Quad(bgfx::ViewId view, bgfx::TextureHandle texture, float x, float y, float width, float height, unsigned int samplerflags, bool flipv = false, bool blend = false, bgfx::ProgramHandle program = BGFX_INVALID_HANDLE, unsigned int colour = 0xFFFFFFFF)
{
	bgfx::TransientVertexBuffer buffer;

	if (bgfx::getAvailTransientVertexBuffer(6, _VertexLayout) < 6) {
		return(false);
	}

	bgfx::allocTransientVertexBuffer(&buffer, 6, _VertexLayout);

	BackendVertex * vertex = (BackendVertex *)buffer.data;

	const float vtop = flipv ? 1.0f : 0.0f;
	const float vbottom = flipv ? 0.0f : 1.0f;

	vertex[0] = { x, y, 0.0f, vtop, colour };
	vertex[1] = { x + width, y, 1.0f, vtop, colour };
	vertex[2] = { x + width, y + height, 1.0f, vbottom, colour };
	vertex[3] = { x, y, 0.0f, vtop, colour };
	vertex[4] = { x + width, y + height, 1.0f, vbottom, colour };
	vertex[5] = { x, y + height, 0.0f, vbottom, colour };

	bgfx::setVertexBuffer(0, &buffer);
	bgfx::setTexture(0, _TextureSampler, texture, samplerflags);
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | (blend ? BGFX_STATE_BLEND_ALPHA : 0));
	bgfx::submit(view, bgfx::isValid(program) ? program : _Program);
	return(true);
}


/// <summary>
/// Builds an orthographic projection over a target measured in pixels, with the origin in
/// its top left corner.
/// </summary>
void Backend_Build_Ortho_Projection(float * result, int width, int height)
{
	const float depthnear = 0.0f;
	const float depthfar = 1000.0f;
	const bool homogeneous = bgfx::getCaps()->homogeneousDepth;

	memset(result, 0, sizeof(float) * 16);

	result[0] = 2.0f / (float)width;
	result[5] = -2.0f / (float)height;
	result[10] = homogeneous ? 2.0f / (depthfar - depthnear) : 1.0f / (depthfar - depthnear);
	result[12] = -1.0f;
	result[13] = 1.0f;
	result[14] = homogeneous ? -(depthfar + depthnear) / (depthfar - depthnear) : -depthnear / (depthfar - depthnear);
	result[15] = 1.0f;
}


/// <summary>
/// Sets a view to draw into a target of the given size using pixel coordinates.
/// </summary>
static void Set_View_Transform(bgfx::ViewId view, int width, int height)
{
	float projection[16];
	bgfx::setViewRect(view, 0, 0, (uint16_t)width, (uint16_t)height);
	Backend_Build_Ortho_Projection(projection, width, height);
	bgfx::setViewTransform(view, NULL, projection);
}


/// <summary>
/// Discards the intermediate target the pixel art filter magnifies through.
/// </summary>
static void Destroy_Prescale_Target(void)
{
	if (bgfx::isValid(_PrescaleTarget)) {
		bgfx::destroy(_PrescaleTarget);
		_PrescaleTarget = BGFX_INVALID_HANDLE;
	}
	_PrescaleWidth = 0;
	_PrescaleHeight = 0;
}


/// <summary>
/// Makes sure the pixel art filter has an intermediate target of the requested size.
/// </summary>
/// <returns>bool; Is a target of that size ready to render into?</returns>
static bool Ensure_Prescale_Target(int width, int height)
{
	if (bgfx::isValid(_PrescaleTarget) && _PrescaleWidth == width && _PrescaleHeight == height) {
		return(true);
	}

	Destroy_Prescale_Target();

	const bgfx::Caps * caps = bgfx::getCaps();
	if (width <= 0 || height <= 0 || width > caps->limits.maxTextureSize || height > caps->limits.maxTextureSize) {
		return(false);
	}

	_PrescaleTarget = bgfx::createFrameBuffer((uint16_t)width, (uint16_t)height, bgfx::TextureFormat::BGRA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
	if (!bgfx::isValid(_PrescaleTarget)) {
		return(false);
	}

	_PrescaleWidth = width;
	_PrescaleHeight = height;
	return(true);
}


static void Destroy_Mask(BackendMask & mask)
{
	if (bgfx::isValid(mask.Handle)) {
		bgfx::destroy(mask.Handle);
	}
	mask.Handle = BGFX_INVALID_HANDLE;
	mask.Width = 0;
	mask.Height = 0;
	mask.Uploaded = false;
}


// Counts one texture upload of the given bytes for the perf log.
static void Count_Upload(std::uint64_t bytes)
{
	Perf_Count(PERF_COUNT_UPLOAD_BYTES, bytes);
	Perf_Count(PERF_COUNT_UPLOADS);
}


/// <summary>
/// Uploads one mask, making its R8 texture first when the size changed. Only the readable part of the
/// source is copied; texels beyond it are zero. False means nothing was uploaded: the mask is not
/// available, such as when the hardware cannot sample R8, or the source has no readable pixels.
/// </summary>
static bool Upload_Mask(BackendMask & mask, unsigned char const * pixels, int pitch, int readwidth, int readheight, int width, int height)
{
	if (pixels == NULL || readwidth <= 0 || readheight <= 0 || width <= 0 || height <= 0) {
		return(false);
	}
	if (!bgfx::isValid(mask.Handle) || mask.Width != width || mask.Height != height) {
		Destroy_Mask(mask);
		const bgfx::Caps * caps = bgfx::getCaps();
		if ((caps->formats[bgfx::TextureFormat::R8] & BGFX_CAPS_FORMAT_TEXTURE_2D) == 0) {
			return(false);
		}
		if (width > caps->limits.maxTextureSize || height > caps->limits.maxTextureSize) {
			return(false);
		}
		mask.Handle = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::R8);
		if (!bgfx::isValid(mask.Handle)) {
			return(false);
		}
		mask.Width = width;
		mask.Height = height;
	}

	// The rows are packed to the texture width, since the pitch of a mask may be wider than the picture.
	// The source may be smaller than the texture, so the texels it does not reach stay zero.
	bgfx::Memory const * memory = bgfx::alloc((uint32_t)(width * height));
	std::memset(memory->data, 0, (size_t)(width * height));
	int const copywidth = std::min(width, readwidth);
	int const copyheight = std::min(height, readheight);
	for (int y = 0; y < copyheight; y++) {
		std::memcpy(memory->data + y * width, pixels + y * pitch, (size_t)copywidth);
	}
	Count_Upload((std::uint64_t)width * height);
	bgfx::updateTexture2D(mask.Handle, 0, 0, 0, 0, (uint16_t)width, (uint16_t)height, memory, (uint16_t)width);
	mask.Uploaded = true;
	return(true);
}


/// <summary>
/// Sets the mask target as the view the emissive masks are drawn into. It is cleared to zero first, so a
/// pixel that no mask covers reads zero in the composite.
/// </summary>
static void Begin_Mask_View(void)
{
	bgfx::setViewFrameBuffer(VIEW_MASK, _MaskTarget);
	bgfx::setViewClear(VIEW_MASK, BGFX_CLEAR_COLOR, 0x00000000);
	bgfx::setViewMode(VIEW_MASK, bgfx::ViewMode::Sequential);
	Set_View_Transform(VIEW_MASK, _DrawableWidth, _DrawableHeight);
}


/// <summary>
/// Draws a mask into the mask target over a rectangle of the window, where the colour picture is drawn.
/// The texture is an R8 mask or a keyed mask, and the draw reads its red channel. A zero draw writes zero
/// over the rectangle instead, using the texture only for its shape. The mask is point sampled.
/// </summary>
static bool Submit_Mask(bgfx::TextureHandle texture, float x, float y, float width, float height, bool blend, bool zero)
{
	unsigned int const sampler = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_POINT;
	_MaskDrawn = true;
	return(Submit_Quad(VIEW_MASK, texture, x, y, width, height, sampler, false, blend, BGFX_INVALID_HANDLE, zero ? 0x00000000u : 0xFFFFFFFFu));
}


static void Destroy_Layer(void)
{
	Destroy_Mask(_LayerMask);
	if (bgfx::isValid(_LayerTexture)) {
		bgfx::destroy(_LayerTexture);
		_LayerTexture = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_KeyedFrameTexture)) {
		bgfx::destroy(_KeyedFrameTexture);
		_KeyedFrameTexture = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_KeyedMaskTexture)) {
		bgfx::destroy(_KeyedMaskTexture);
		_KeyedMaskTexture = BGFX_INVALID_HANDLE;
	}
	delete [] _LayerConvertBuffer;
	_LayerConvertBuffer = NULL;
	delete [] _KeyedFrameBuffer;
	_KeyedFrameBuffer = NULL;
	delete [] _KeyedMaskBuffer;
	_KeyedMaskBuffer = NULL;
	_LayerWidth = 0;
	_LayerHeight = 0;
	_LayerUploaded = false;
	_KeyedFrameUploaded = false;
	_KeyedMaskUploaded = false;
}


/// <summary>
/// Uploads a new layer picture, making its texture first when the size changed. False means the
/// layer cannot be drawn.
/// </summary>
static bool Upload_Layer(BackendLayer const & layer)
{
	if (layer.Width <= 0 || layer.Height <= 0) {
		return(false);
	}

	if (!bgfx::isValid(_LayerTexture) || _LayerWidth != layer.Width || _LayerHeight != layer.Height) {
		if (bgfx::isValid(_LayerTexture)) {
			bgfx::destroy(_LayerTexture);
		}
		delete [] _LayerConvertBuffer;
		_LayerConvertBuffer = NULL;
		Destroy_Mask(_LayerMask);

		const bgfx::Caps * caps = bgfx::getCaps();
		if (layer.Width > caps->limits.maxTextureSize || layer.Height > caps->limits.maxTextureSize) {
			_LayerTexture = BGFX_INVALID_HANDLE;
			return(false);
		}
		_LayerTexture = bgfx::createTexture2D((uint16_t)layer.Width, (uint16_t)layer.Height, false, 1, _FrameIs565 ? bgfx::TextureFormat::B5G6R5 : bgfx::TextureFormat::BGRA8);
		if (!bgfx::isValid(_LayerTexture)) {
			return(false);
		}
		if (!_FrameIs565) {
			_LayerConvertBuffer = new unsigned int[layer.Width * layer.Height];
		}
		_LayerWidth = layer.Width;
		_LayerHeight = layer.Height;
		_LayerUploaded = false;
	}

	if (_FrameIs565) {
		// The last row is read only up to the picture's edge, which may be the end of its surface,
		// but the upload is padded to a whole pitch because Vulkan copies every row at full pitch.
		uint32_t const size = (uint32_t)((layer.Height - 1) * layer.Pitch + layer.Width * 2);
		bgfx::Memory const * memory = bgfx::alloc((uint32_t)(layer.Height * layer.Pitch));
		std::memcpy(memory->data, layer.Pixels, size);
		std::memset(memory->data + size, 0, memory->size - size);
		Count_Upload((std::uint64_t)layer.Pitch * layer.Height);
		bgfx::updateTexture2D(_LayerTexture, 0, 0, 0, 0, (uint16_t)layer.Width, (uint16_t)layer.Height, memory, (uint16_t)layer.Pitch);
	} else {
		for (int y = 0; y < layer.Height; y++) {
			unsigned short const * source = (unsigned short const *)((char const *)layer.Pixels + y * layer.Pitch);
			unsigned int * dest = _LayerConvertBuffer + y * layer.Width;
			for (int x = 0; x < layer.Width; x++) {
				dest[x] = _ConvertTable[source[x]];
			}
		}
		Count_Upload((std::uint64_t)layer.Width * layer.Height * 4);
		bgfx::updateTexture2D(_LayerTexture, 0, 0, 0, 0, (uint16_t)layer.Width, (uint16_t)layer.Height, bgfx::copy(_LayerConvertBuffer, (uint32_t)(layer.Width * layer.Height * 4)), (uint16_t)(layer.Width * 4));
	}
	_LayerUploaded = true;
	return(true);
}


/// <summary>
/// Uploads the frame with its key pixels made see-through. False means it could not be.
/// </summary>
static bool Upload_Keyed_Frame(void const * pixels, int pitch)
{
	if (!bgfx::isValid(_KeyedFrameTexture)) {
		_KeyedFrameTexture = bgfx::createTexture2D((uint16_t)_FrameWidth, (uint16_t)_FrameHeight, false, 1, bgfx::TextureFormat::BGRA8);
		if (!bgfx::isValid(_KeyedFrameTexture)) {
			return(false);
		}
		delete [] _KeyedFrameBuffer;
		_KeyedFrameBuffer = new unsigned int[_FrameWidth * _FrameHeight];
	}
	if (_ConvertTable[0xFFFF] == 0) {
		Build_Convert_Table();
	}

	for (int y = 0; y < _FrameHeight; y++) {
		unsigned short const * source = (unsigned short const *)((char const *)pixels + y * pitch);
		unsigned int * dest = _KeyedFrameBuffer + y * _FrameWidth;
		for (int x = 0; x < _FrameWidth; x++) {
			unsigned short const pixel = source[x];
			dest[x] = (pixel == BACKEND_LAYER_KEY) ? 0 : _ConvertTable[pixel];
		}
	}
	Count_Upload((std::uint64_t)_FrameWidth * _FrameHeight * 4);
	bgfx::updateTexture2D(_KeyedFrameTexture, 0, 0, 0, 0, (uint16_t)_FrameWidth, (uint16_t)_FrameHeight, bgfx::copy(_KeyedFrameBuffer, (uint32_t)(_FrameWidth * _FrameHeight * 4)), (uint16_t)(_FrameWidth * 4));
	return(true);
}


/// <summary>
/// Uploads the keyed mask for a frame: the frame's emissive mask in the red channel, and alpha zero where
/// the frame has the layer key. Red is zero wherever no mask is given or the mask's readable size does
/// not reach. False means it could not be made.
/// </summary>
static bool Upload_Keyed_Mask(void const * pixels, int pitch, unsigned char const * mask, int maskpitch, int maskwidth, int maskheight)
{
	_KeyedMaskUploaded = false;
	if (!bgfx::isValid(_KeyedMaskTexture)) {
		_KeyedMaskTexture = bgfx::createTexture2D((uint16_t)_FrameWidth, (uint16_t)_FrameHeight, false, 1, bgfx::TextureFormat::BGRA8);
		if (!bgfx::isValid(_KeyedMaskTexture)) {
			return(false);
		}
		delete [] _KeyedMaskBuffer;
		_KeyedMaskBuffer = new unsigned int[_FrameWidth * _FrameHeight];
	}

	// A BGRA8 pixel is blue, green, red, alpha from its lowest byte up, so red sits at bit 16 and alpha at bit 24.
	for (int y = 0; y < _FrameHeight; y++) {
		unsigned short const * source = (unsigned short const *)((char const *)pixels + y * pitch);
		unsigned char const * value = (mask != NULL && y < maskheight) ? mask + y * maskpitch : NULL;
		unsigned int * dest = _KeyedMaskBuffer + y * _FrameWidth;
		for (int x = 0; x < _FrameWidth; x++) {
			unsigned int const alpha = (source[x] == BACKEND_LAYER_KEY) ? 0u : 255u;
			unsigned int const red = (value != NULL && x < maskwidth) ? value[x] : 0u;
			dest[x] = (alpha << 24) | (red << 16);
		}
	}
	Count_Upload((std::uint64_t)_FrameWidth * _FrameHeight * 4);
	bgfx::updateTexture2D(_KeyedMaskTexture, 0, 0, 0, 0, (uint16_t)_FrameWidth, (uint16_t)_FrameHeight, bgfx::copy(_KeyedMaskBuffer, (uint32_t)(_FrameWidth * _FrameHeight * 4)), (uint16_t)(_FrameWidth * 4));
	_KeyedMaskUploaded = true;
	return(true);
}


/// <summary>
/// Reports whether the renderer can present an HDR10 back buffer.
/// </summary>
/// <returns>bool; Can the renderer present RGB10A2 with HDR10 metadata?</returns>
static bool HDR_Caps_Available(void)
{
	bgfx::Caps const * caps = bgfx::getCaps();
	if ((caps->supported & BGFX_CAPS_HDR10) == 0) {
		DebugString("HDR: the renderer reports no HDR10 support\n");
		return(false);
	}
	if ((caps->formats[bgfx::TextureFormat::RGB10A2] & BGFX_CAPS_FORMAT_TEXTURE_BACKBUFFER) == 0) {
		DebugString("HDR: RGB10A2 is not a back buffer format on this renderer\n");
		return(false);
	}
	return(true);
}


/// <summary>
/// Builds the reset flags, which carry HDR10 when the back buffer is HDR10.
/// </summary>
static unsigned int Reset_Flags(bool vsync, bool hdr)
{
	return(BGFX_RESET_FLIP_AFTER_RENDER | (vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE) | (hdr ? BGFX_RESET_HDR10 : BGFX_RESET_NONE));
}


/// <summary>
/// Frees the scene and shot targets. The next Ensure_Targets makes them again at the current size. A
/// capture still being read keeps its readback texture until it is written.
/// </summary>
static void Destroy_Targets(void)
{
	if (bgfx::isValid(_SceneTarget)) {
		bgfx::destroy(_SceneTarget);
		_SceneTarget = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_ShotTarget)) {
		bgfx::destroy(_ShotTarget);
		_ShotTarget = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_MaskTarget)) {
		bgfx::destroy(_MaskTarget);
		_MaskTarget = BGFX_INVALID_HANDLE;
	}
	if (!_ReadbackPending && bgfx::isValid(_ReadbackTexture)) {
		bgfx::destroy(_ReadbackTexture);
		_ReadbackTexture = BGFX_INVALID_HANDLE;
		_ReadbackPixels.clear();
	}
}


/// <summary>
/// Makes the targets the current routing needs, at the drawable size.
/// </summary>
/// <returns>bool; Does every target the routing needs exist?</returns>
static bool Ensure_Targets(void)
{
	if (_Routed && !bgfx::isValid(_SceneTarget)) {
		_SceneTarget = bgfx::createFrameBuffer((uint16_t)_DrawableWidth, (uint16_t)_DrawableHeight, bgfx::TextureFormat::BGRA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		if (!bgfx::isValid(_SceneTarget)) {
			DebugString("Video: could not create the scene target at %dx%d\n", _DrawableWidth, _DrawableHeight);
			return(false);
		}
	}
	if (_HdrActive && !bgfx::isValid(_MaskTarget)) {
		_MaskTarget = bgfx::createFrameBuffer((uint16_t)_DrawableWidth, (uint16_t)_DrawableHeight, bgfx::TextureFormat::BGRA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		if (!bgfx::isValid(_MaskTarget)) {
			DebugString("Video: could not create the emissive mask target at %dx%d\n", _DrawableWidth, _DrawableHeight);
			return(false);
		}
	}
	if (_HdrActive && !bgfx::isValid(_ShotTarget)) {
		_ShotTarget = bgfx::createFrameBuffer((uint16_t)_DrawableWidth, (uint16_t)_DrawableHeight, bgfx::TextureFormat::BGRA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		if (!bgfx::isValid(_ShotTarget)) {
			DebugString("Video: could not create the screenshot target at %dx%d\n", _DrawableWidth, _DrawableHeight);
			return(false);
		}
	}
	if (_HdrActive && !bgfx::isValid(_ReadbackTexture)) {
		uint64_t const flags = BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
		_ReadbackTexture = bgfx::createTexture2D((uint16_t)_DrawableWidth, (uint16_t)_DrawableHeight, false, 1, bgfx::TextureFormat::BGRA8, flags);
		if (!bgfx::isValid(_ReadbackTexture)) {
			DebugString("Video: could not create the screenshot readback at %dx%d\n", _DrawableWidth, _DrawableHeight);
			return(false);
		}
		_ReadbackPixels.assign((size_t)_DrawableWidth * (size_t)_DrawableHeight * 4, 0);
	}
	return(true);
}


/// <summary>
/// Decides whether frames reach the window through the scene target. They do under HDR10 output and
/// whenever the colour adjustment is not the identity.
/// </summary>
static void Update_Routing(void)
{
	_Routed = _Initialized && bgfx::isValid(_CompositeProgram) && (_HdrActive || !HDR_Colour_Adjust_Is_Identity(_ColourAdjust));
}


/// <summary>
/// Returns the target the frame is drawn into: the scene target when frames are routed, else the window.
/// </summary>
static bgfx::FrameBufferHandle Frame_Target(void)
{
	bgfx::FrameBufferHandle const window = BGFX_INVALID_HANDLE;
	if (_Routed) {
		return(_SceneTarget);
	}
	return(window);
}


/// <summary>
/// Draws the scene target into a target through the composite shader, applying the colour adjustment.
/// </summary>
/// <param name="view">The view to draw with.</param>
/// <param name="target">The target to draw into; the window when invalid.</param>
/// <param name="hdr">True to encode ST.2084 with Rec.2020 primaries; false to encode sRGB.</param>
static void Submit_Composite(bgfx::ViewId view, bgfx::FrameBufferHandle target, bool hdr)
{
	bgfx::setViewFrameBuffer(view, target);
	bgfx::setViewClear(view, BGFX_CLEAR_COLOR, 0x000000FF);
	Set_View_Transform(view, _DrawableWidth, _DrawableHeight);

	float const levels[4] = {
		_PaperWhiteNits / HDR_PQ_PEAK_NITS,
		_PeakNits / HDR_PQ_PEAK_NITS,
		hdr ? 1.0f : 0.0f,
		HDR_Colour_Adjust_Is_Identity(_ColourAdjust) ? 0.0f : 1.0f,
	};
	bgfx::setUniform(_CompositeLevels, levels);

	float rows[12];
	for (int row = 0; row < 3; row++) {
		for (int column = 0; column < 3; column++) {
			rows[row * 4 + column] = _ColourAdjust.Matrix[row * 3 + column];
		}
		rows[row * 4 + 3] = 0.0f;
	}
	bgfx::setUniform(_ColourMatrix, rows, 3);

	float const tone[4] = {_ColourAdjust.Brightness, _ColourAdjust.Gamma, 0.0f, 0.0f};
	bgfx::setUniform(_ColourTone, tone);

	// The mask is bound only to an HDR composite that has masks this frame. SDR never reads it.
	bool const masked = hdr && _MaskDrawn && bgfx::isValid(_MaskTarget);
	if (masked) {
		bgfx::setTexture(1, _MaskSampler, bgfx::getTexture(_MaskTarget), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_POINT);
	}
	float const maskflags[4] = {masked ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f};
	bgfx::setUniform(_MaskLevels, maskflags);

	unsigned int const sampler = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_POINT;
	Submit_Quad(view, bgfx::getTexture(_SceneTarget), 0.0f, 0.0f, (float)_DrawableWidth, (float)_DrawableHeight, sampler, false, false, _CompositeProgram);
}


/// <summary>
/// Writes the capture that the readback has finished, as a 32-bit TGA of the drawable area.
/// </summary>
static void Write_Capture(void)
{
	_ReadbackPending = false;
	FILE * file = std::fopen(_ReadbackPath.c_str(), "wb");
	if (file == NULL) {
		return;
	}
	unsigned char header[18] = {};
	header[2] = 2;
	header[12] = (unsigned char)(_DrawableWidth & 0xFF);
	header[13] = (unsigned char)((_DrawableWidth >> 8) & 0xFF);
	header[14] = (unsigned char)(_DrawableHeight & 0xFF);
	header[15] = (unsigned char)((_DrawableHeight >> 8) & 0xFF);
	header[16] = 32;
	header[17] = 0x28;
	std::fwrite(header, 1, sizeof(header), file);
	std::fwrite(_ReadbackPixels.data(), 1, _ReadbackPixels.size(), file);
	std::fclose(file);
}


/// <summary>
/// Copies the SDR picture of the frame into the readback texture and starts reading it back. The picture is
/// the scene target, or a composite of it when a colour adjustment applies.
/// </summary>
static void Start_Capture(void)
{
	bgfx::TextureHandle source = bgfx::getTexture(_SceneTarget);
	if (!HDR_Colour_Adjust_Is_Identity(_ColourAdjust)) {
		Submit_Composite(VIEW_SHOT, _ShotTarget, false);
		source = bgfx::getTexture(_ShotTarget);
	}
	bgfx::blit(VIEW_READBACK, _ReadbackTexture, 0, 0, source);
	_ReadbackFrame = bgfx::readTexture(_ReadbackTexture, _ReadbackPixels.data());
	_ReadbackPending = true;
	_ShotRequested = false;
}


/// <summary>
/// Waits for a pending capture to finish reading and writes it, so its buffer is free before the targets change size.
/// </summary>
static void Finish_Capture(void)
{
	for (int attempt = 0; attempt < 4 && _ReadbackPending; attempt++) {
		_LastFrame = bgfx::frame();
		if (_LastFrame >= _ReadbackFrame) {
			Write_Capture();
		}
	}
}


/// <summary>
/// Starts the renderer on the window, with an HDR10 back buffer when hdr is set.
/// </summary>
/// <returns>bool; Did the renderer start with the back buffer asked for? Nothing is left running on false.</returns>
static bool Start_Renderer(NativeWindow const & window, int drawablewidth, int drawableheight, BackendRenderer renderer, bool vsync, bool hdr)
{
	_HdrActive = false;
	_Routed = false;
	_ResetFlags = Reset_Flags(vsync, hdr);

	bgfx::Init init;
	init.platformData.ndt = window.Display;
	init.platformData.nwh = window.Handle;
	init.platformData.type = window.Type == NATIVE_WINDOW_WAYLAND
		? bgfx::NativeWindowHandleType::Wayland
		: bgfx::NativeWindowHandleType::Default;
	init.resolution.width = (uint32_t)drawablewidth;
	init.resolution.height = (uint32_t)drawableheight;
	init.resolution.reset = _ResetFlags;
	if (hdr) {
		init.resolution.formatColor = bgfx::TextureFormat::RGB10A2;
	}
	init.callback = &_Callback;
	init.allocator = &_Allocator;

	switch (renderer) {
		case BACKEND_RENDERER_D3D11:
			init.type = bgfx::RendererType::Direct3D11;
			break;

		case BACKEND_RENDERER_D3D12:
			init.type = bgfx::RendererType::Direct3D12;
			break;

		case BACKEND_RENDERER_VULKAN:
			init.type = bgfx::RendererType::Vulkan;
			break;

		case BACKEND_RENDERER_OPENGL:
			init.type = bgfx::RendererType::OpenGL;
			break;

		default:
			init.type = bgfx::RendererType::Count;
			break;
	}

	if (!bgfx::init(init)) {
		if (hdr) {
			DebugString("HDR: the renderer did not start with an RGB10A2 back buffer\n");
		}
		return(false);
	}

	if (hdr && !HDR_Caps_Available()) {
		bgfx::shutdown();
		return(false);
	}

	_VertexLayout.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	bgfx::RendererType::Enum type = bgfx::getRendererType();
	bgfx::ShaderHandle vertexshader = bgfx::createEmbeddedShader(_EmbeddedShaders, type, "vs_ocornut_imgui");
	bgfx::ShaderHandle fragmentshader = bgfx::createEmbeddedShader(_EmbeddedShaders, type, "fs_ocornut_imgui");

	if (!bgfx::isValid(vertexshader) || !bgfx::isValid(fragmentshader)) {
		bgfx::shutdown();
		return(false);
	}

	_Program = bgfx::createProgram(vertexshader, fragmentshader, true);
	_TextureSampler = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

	if (!bgfx::isValid(_Program) || !bgfx::isValid(_TextureSampler)) {
		bgfx::shutdown();
		return(false);
	}

	// The composite pass draws the scene target to the window. Renderers without its shader keep the direct path.
	bgfx::ShaderHandle compositevs = bgfx::createEmbeddedShader(_EmbeddedShaders, type, "vs_ocornut_imgui");
	bgfx::ShaderHandle compositefs = bgfx::createEmbeddedShader(_CompositeShaders, type, "fs_composite");
	if (bgfx::isValid(compositevs) && bgfx::isValid(compositefs)) {
		_CompositeProgram = bgfx::createProgram(compositevs, compositefs, true);
	}
	_CompositeLevels = bgfx::createUniform("u_compositeLevels", bgfx::UniformType::Vec4);
	_ColourMatrix = bgfx::createUniform("u_colourMatrix", bgfx::UniformType::Vec4, 3);
	_ColourTone = bgfx::createUniform("u_colourTone", bgfx::UniformType::Vec4);
	_MaskSampler = bgfx::createUniform("s_mask", bgfx::UniformType::Sampler);
	_MaskLevels = bgfx::createUniform("u_maskLevels", bgfx::UniformType::Vec4);

	if (hdr && !bgfx::isValid(_CompositeProgram)) {
		DebugString("HDR: the composite shader is not available for this renderer\n");
		bgfx::shutdown();
		return(false);
	}

	_Initialized = true;
	_HdrActive = hdr;
	Update_Routing();
	if (!Ensure_Targets()) {
		Backend_Shutdown();
		return(false);
	}
	return(true);
}


/// <summary>
/// Starts the renderer on an existing window. An HDR10 request that the renderer cannot honour starts
/// the SDR back buffer instead.
/// </summary>
/// <param name="window">The window the frame is presented into.</param>
/// <param name="drawablewidth">The drawable area's width in physical pixels.</param>
/// <param name="drawableheight">The drawable area's height in physical pixels.</param>
/// <param name="renderer">Which graphics API to ask for, or auto to let bgfx decide.</param>
/// <param name="vsync">Should presents wait for the display's refresh?</param>
/// <param name="hdr">Should the back buffer be HDR10?</param>
/// <returns>bool; Did the renderer start?</returns>
bool Backend_Init(NativeWindow const & window, int drawablewidth, int drawableheight, BackendRenderer renderer, bool vsync, bool hdr)
{
	if (_Initialized) {
		return(true);
	}

	bgfx::renderFrame();

	_DrawableWidth = drawablewidth;
	_DrawableHeight = drawableheight;

	if (hdr && Start_Renderer(window, drawablewidth, drawableheight, renderer, vsync, true)) {
		return(true);
	}
	if (hdr) {
		DebugString("HDR: starting the SDR back buffer instead\n");
	}
	return(Start_Renderer(window, drawablewidth, drawableheight, renderer, vsync, false));
}


/// <summary>
/// Shuts the renderer down and releases everything it created.
/// </summary>
void Backend_Shutdown(void)
{
	if (!_Initialized) {
		return;
	}

	Destroy_Prescale_Target();
	Destroy_Layer();
	Destroy_Targets();
	Destroy_Mask(_FrameMask);

	if (bgfx::isValid(_FrameTexture)) {
		bgfx::destroy(_FrameTexture);
		_FrameTexture = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_TextureSampler)) {
		bgfx::destroy(_TextureSampler);
		_TextureSampler = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_Program)) {
		bgfx::destroy(_Program);
		_Program = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_CompositeProgram)) {
		bgfx::destroy(_CompositeProgram);
		_CompositeProgram = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_CompositeLevels)) {
		bgfx::destroy(_CompositeLevels);
		_CompositeLevels = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_ColourMatrix)) {
		bgfx::destroy(_ColourMatrix);
		_ColourMatrix = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_ColourTone)) {
		bgfx::destroy(_ColourTone);
		_ColourTone = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_MaskSampler)) {
		bgfx::destroy(_MaskSampler);
		_MaskSampler = BGFX_INVALID_HANDLE;
	}
	if (bgfx::isValid(_MaskLevels)) {
		bgfx::destroy(_MaskLevels);
		_MaskLevels = BGFX_INVALID_HANDLE;
	}

	delete [] _ConvertBuffer;
	_ConvertBuffer = NULL;

	bgfx::shutdown();

	_FrameWidth = 0;
	_FrameHeight = 0;
	_FrameUploaded = false;
	_FramePending = false;
	_HdrActive = false;
	_Routed = false;
	_ShotRequested = false;
	_ReadbackPending = false;
	_Initialized = false;
}


/// <summary>
/// Points the renderer at a frame of the given size, replacing any earlier one.
/// </summary>
/// <returns>bool; Is a texture of that size ready to receive frames?</returns>
bool Backend_Set_Frame_Size(int width, int height)
{
	if (!_Initialized || width <= 0 || height <= 0) {
		return(false);
	}

	if (bgfx::isValid(_FrameTexture) && _FrameWidth == width && _FrameHeight == height) {
		return(true);
	}

	if (bgfx::isValid(_FrameTexture)) {
		bgfx::destroy(_FrameTexture);
		_FrameTexture = BGFX_INVALID_HANDLE;
	}
	Destroy_Layer();
	Destroy_Mask(_FrameMask);

	// bgfx names packed formats from their low bits up, so its B5G6R5 is the layout the
	// game already draws in. Emulated support would convert every upload on the way
	// through, which is what the fallback below does more cheaply.
	const bgfx::Caps * caps = bgfx::getCaps();
	_FrameIs565 = (caps->formats[bgfx::TextureFormat::B5G6R5] & BGFX_CAPS_FORMAT_TEXTURE_2D) != 0;

	_FrameTexture = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, _FrameIs565 ? bgfx::TextureFormat::B5G6R5 : bgfx::TextureFormat::BGRA8);
	_FrameUploaded = false;
	if (!bgfx::isValid(_FrameTexture)) {
		return(false);
	}

	delete [] _ConvertBuffer;
	_ConvertBuffer = NULL;

	if (!_FrameIs565) {
		if (_ConvertTable[0xFFFF] == 0) {
			Build_Convert_Table();
		}
		_ConvertBuffer = new unsigned int[width * height];
	}

	_FrameWidth = width;
	_FrameHeight = height;
	return(true);
}


/// <summary>
/// Tells the renderer the drawable area changed size.
/// </summary>
void Backend_On_Resize(int drawablewidth, int drawableheight)
{
	if (!_Initialized || drawablewidth <= 0 || drawableheight <= 0) {
		return;
	}

	if (_DrawableWidth == drawablewidth && _DrawableHeight == drawableheight) {
		return;
	}

	_DrawableWidth = drawablewidth;
	_DrawableHeight = drawableheight;
	Finish_Capture();
	bgfx::reset((uint32_t)drawablewidth, (uint32_t)drawableheight, _ResetFlags);

	// The targets are sized to the drawable area, so they are made again at the new size.
	Destroy_Targets();
	Ensure_Targets();
}


void Backend_Set_VSync(bool vsync)
{
	unsigned int const flags = Reset_Flags(vsync, _HdrActive);
	if (flags == _ResetFlags) {
		return;
	}

	_ResetFlags = flags;
	if (_Initialized) {
		bgfx::reset((uint32_t)_DrawableWidth, (uint32_t)_DrawableHeight, _ResetFlags);
	}
}


/// <summary>
/// Submits the layer and then the frame over it, with the frame's key pixels see-through. False
/// means nothing was submitted and the frame should be presented without the layer.
/// </summary>
static bool Present_With_Layer(void const * pixels, int pitch, int destx, int desty, int destwidth, int destheight, BackendScaleMode mode, BackendLayer const & layer, BackendEmissive const * emissive)
{
	if (layer.Pixels != NULL) {
		if (!Upload_Layer(layer)) {
			return(false);
		}
		_LayerMask.Uploaded = false;
		if (emissive != NULL && emissive->Layer != NULL) {
			Upload_Mask(_LayerMask, emissive->Layer, emissive->LayerPitch, emissive->LayerWidth, emissive->LayerHeight, layer.Width, layer.Height);
		}
	} else if (!_LayerUploaded) {
		return(false);
	}

	if (pixels != NULL) {
		if (!Upload_Keyed_Frame(pixels, pitch)) {
			return(false);
		}
		_KeyedFrameUploaded = true;
		// The plain frame texture now holds an older picture than the screen shows.
		_FrameUploaded = false;
		// The keyed mask is made from the same pixels, so it keys out exactly where the keyed frame does.
		if (_HdrActive) {
			Upload_Keyed_Mask(pixels, pitch, emissive != NULL ? emissive->Frame : NULL, emissive != NULL ? emissive->FramePitch : 0, emissive != NULL ? emissive->FrameWidth : 0, emissive != NULL ? emissive->FrameHeight : 0);
		}
	} else if (!_KeyedFrameUploaded) {
		return(false);
	}

	bgfx::setViewFrameBuffer(VIEW_PRESENT, Frame_Target());
	bgfx::setViewClear(VIEW_PRESENT, BGFX_CLEAR_COLOR, 0x000000FF);
	bgfx::setViewMode(VIEW_PRESENT, bgfx::ViewMode::Sequential);
	Set_View_Transform(VIEW_PRESENT, _DrawableWidth, _DrawableHeight);

	unsigned int const clamp = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

	// The map keeps whole pixels while it is drawn at its own size or larger, and is smoothed when shrunk.
	bool const layer_shrunk = layer.DestWidth < _LayerWidth || layer.DestHeight < _LayerHeight;
	// The debug view shows the layer's mask alone, since it covers the same area as the frame.
	// Masks follow the colour in the same order: the layer's mask, or zero, over the layer, then the
	// keyed mask over the frame, which blends so that keyed pixels keep the layer's mask.
	bool const masks = _HdrActive && emissive != NULL;
	if (masks) {
		Begin_Mask_View();
		if (_LayerMask.Uploaded) {
			Submit_Mask(_LayerMask.Handle, (float)layer.DestX, (float)layer.DestY, (float)layer.DestWidth, (float)layer.DestHeight, false, false);
		} else {
			Submit_Mask(_FrameTexture, (float)layer.DestX, (float)layer.DestY, (float)layer.DestWidth, (float)layer.DestHeight, false, true);
		}
	}

	bool const layer_masked = _MaskView && _LayerMask.Uploaded;
	if (!Submit_Quad(VIEW_PRESENT, layer_masked ? _LayerMask.Handle : _LayerTexture, (float)layer.DestX, (float)layer.DestY, (float)layer.DestWidth, (float)layer.DestHeight, clamp | (layer_shrunk ? 0 : BGFX_SAMPLER_POINT))) {
		return(false);
	}
	if (layer_masked) {
		return(true);
	}

	unsigned int const framesampler = clamp | (mode == BACKEND_SCALE_LINEAR ? 0 : BGFX_SAMPLER_POINT);
	_FramePointSampled = (framesampler & BGFX_SAMPLER_POINT) != 0;
	if (masks && _KeyedMaskUploaded) {
		Submit_Mask(_KeyedMaskTexture, (float)destx, (float)desty, (float)destwidth, (float)destheight, true, false);
	}
	return(Submit_Quad(VIEW_PRESENT, _KeyedFrameTexture, (float)destx, (float)desty, (float)destwidth, (float)destheight, framesampler, false, true));
}


/// <summary>
/// Submits the frame to the window, uploading new pixels first when given any.
/// The frame reaches the screen when Backend_End_Frame runs; whatever is submitted in
/// between draws over it.
/// </summary>
/// <param name="pixels">The frame's top left pixel, in 16 bit 565, or NULL to present the
/// frame uploaded last.</param>
/// <param name="pitch">The bytes between one row of that frame and the next.</param>
/// <param name="destx">Where the left edge of the frame lands in the window.</param>
/// <param name="desty">Where the top edge of the frame lands in the window.</param>
/// <param name="destwidth">How wide the frame is drawn.</param>
/// <param name="destheight">How tall the frame is drawn.</param>
/// <param name="mode">How the frame is filtered when it is drawn larger than it is.</param>
/// <param name="layer">A picture to draw under the frame, or NULL for none.</param>
/// <returns>bool; Was a frame submitted? When not, the window is unchanged and nothing should
/// be drawn over it.</returns>
bool Backend_Present(void const * pixels, int pitch, int destx, int desty, int destwidth, int destheight, BackendScaleMode mode, BackendLayer const * layer, BackendEmissive const * emissive)
{
	if (!_Initialized || !bgfx::isValid(_FrameTexture)) {
		return(false);
	}

	// While the map is drawn as a layer, the frame lives in the keyed texture, so a present that
	// only redraws the overlay, such as a menu opened over a zoomed-out map, uses that one.
	bool const keyed = layer != NULL && _KeyedFrameUploaded;
	if (pixels == NULL && !_FrameUploaded && !keyed) {
		return(false);
	}

	// A minimized window has no client area to present into.
	if (_DrawableWidth <= 0 || _DrawableHeight <= 0) {
		return(false);
	}

	if (!Ensure_Targets()) {
		return(false);
	}

	_FramePending = true;

	// A frame with no mask this present leaves the frame mask cleared, so the debug view shows nothing stale.
	if (pixels != NULL) {
		_FrameMask.Uploaded = false;
		if (emissive != NULL && emissive->Frame != NULL) {
			Upload_Mask(_FrameMask, emissive->Frame, emissive->FramePitch, emissive->FrameWidth, emissive->FrameHeight, _FrameWidth, _FrameHeight);
		}
	}

	// Masks are drawn only for an HDR composite that has emissive planes this present.
	bool const masks = _HdrActive && emissive != NULL;

	if (layer != NULL && Present_With_Layer(pixels, pitch, destx, desty, destwidth, destheight, mode, *layer, emissive)) {
		return(true);
	}

	if (pixels == NULL && !_FrameUploaded) {
		return(false);
	}

	if (pixels != NULL) {
		if (_FrameIs565) {
			Count_Upload((std::uint64_t)_FrameHeight * pitch);
			bgfx::updateTexture2D(_FrameTexture, 0, 0, 0, 0, (uint16_t)_FrameWidth, (uint16_t)_FrameHeight, bgfx::copy(pixels, (uint32_t)(_FrameHeight * pitch)), (uint16_t)pitch);
			_FrameUploaded = true;
		} else if (_ConvertBuffer != NULL) {
			for (int y = 0; y < _FrameHeight; y++) {
				unsigned short const * source = (unsigned short const *)((char const *)pixels + y * pitch);
				unsigned int * dest = _ConvertBuffer + y * _FrameWidth;
				for (int x = 0; x < _FrameWidth; x++) {
					dest[x] = _ConvertTable[source[x]];
				}
			}
			Count_Upload((std::uint64_t)_FrameWidth * _FrameHeight * 4);
			bgfx::updateTexture2D(_FrameTexture, 0, 0, 0, 0, (uint16_t)_FrameWidth, (uint16_t)_FrameHeight, bgfx::copy(_ConvertBuffer, (uint32_t)(_FrameWidth * _FrameHeight * 4)), (uint16_t)(_FrameWidth * 4));
			_FrameUploaded = true;
		}
	}

	// The debug view draws the frame mask where the picture would be.
	bgfx::TextureHandle const picture = (_MaskView && _FrameMask.Uploaded) ? _FrameMask.Handle : _FrameTexture;
	bgfx::TextureHandle source = picture;
	unsigned int samplerflags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	bool from_prescale = false;

	if (mode == BACKEND_SCALE_NEAREST) {
		samplerflags |= BGFX_SAMPLER_POINT;
	}

	// The pixel art filter keeps whole pixels whole. An exact multiple needs nothing but
	// point sampling; anything else is magnified to the next whole multiple with point
	// sampling and then shrunk to the window smoothly, which keeps edges sharp without
	// the uneven pixel sizes that point sampling alone would give.
	if (mode == BACKEND_SCALE_PIXELART && destwidth > _FrameWidth && destheight > _FrameHeight) {
		if ((destwidth % _FrameWidth) == 0 && (destheight % _FrameHeight) == 0) {
			samplerflags |= BGFX_SAMPLER_POINT;
		} else {
			int scale = (destwidth + _FrameWidth - 1) / _FrameWidth;
			int scaley = (destheight + _FrameHeight - 1) / _FrameHeight;
			if (scaley > scale) {
				scale = scaley;
			}

			if (Ensure_Prescale_Target(_FrameWidth * scale, _FrameHeight * scale)) {
				bgfx::setViewFrameBuffer(VIEW_PRESCALE, _PrescaleTarget);
				bgfx::setViewClear(VIEW_PRESCALE, BGFX_CLEAR_COLOR, 0x000000FF);
				Set_View_Transform(VIEW_PRESCALE, _PrescaleWidth, _PrescaleHeight);
				if (Submit_Quad(VIEW_PRESCALE, picture, 0.0f, 0.0f, (float)_PrescaleWidth, (float)_PrescaleHeight, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_POINT)) {
					source = bgfx::getTexture(_PrescaleTarget);
					from_prescale = true;
				}
			}
		}
	}

	// Clearing the whole window is what paints the bars beside a frame that does not
	// share the window's shape.
	bgfx::setViewFrameBuffer(VIEW_PRESENT, Frame_Target());
	bgfx::setViewClear(VIEW_PRESENT, BGFX_CLEAR_COLOR, 0x000000FF);
	Set_View_Transform(VIEW_PRESENT, _DrawableWidth, _DrawableHeight);

	_FramePointSampled = (samplerflags & BGFX_SAMPLER_POINT) != 0;

	bool flipv = from_prescale && bgfx::getCaps()->originBottomLeft;
	bool const shown = Submit_Quad(VIEW_PRESENT, source, (float)destx, (float)desty, (float)destwidth, (float)destheight, samplerflags, flipv);

	// The frame mask covers the rectangle the picture covers. Without one, that rectangle is zeroed.
	if (masks) {
		Begin_Mask_View();
		if (_FrameMask.Uploaded) {
			Submit_Mask(_FrameMask.Handle, (float)destx, (float)desty, (float)destwidth, (float)destheight, false, false);
		} else {
			Submit_Mask(_FrameTexture, (float)destx, (float)desty, (float)destwidth, (float)destheight, false, true);
		}
	}
	return(shown);
}


void Backend_Set_Emissive_View(bool masks)
{
	_MaskView = masks;
}


bool Backend_Frame_Is_Point_Sampled(void)
{
	return(_FramePointSampled);
}


void Backend_End_Frame(void)
{
	if (!_Initialized || !_FramePending) {
		return;
	}

	_FramePending = false;

	if (_ReadbackPending && _LastFrame >= _ReadbackFrame) {
		Write_Capture();
	}
	if (_Routed) {
		// The mask target is cleared on every HDR frame, even one with no masks, so the composite reads zero there.
		if (_HdrActive) {
			Begin_Mask_View();
			bgfx::touch(VIEW_MASK);
		}
		Submit_Composite(VIEW_COMPOSITE, BGFX_INVALID_HANDLE, _HdrActive);
	}
	if (_ShotRequested && !_ReadbackPending) {
		Start_Capture();
	}
	_MaskDrawn = false;
	_LastFrame = bgfx::frame();
}


bool Backend_HDR_Active(void)
{
	return(_HdrActive);
}


void Backend_Set_HDR_Levels(float paperwhite, float peak)
{
	_PaperWhiteNits = paperwhite;
	_PeakNits = peak;
}


void Backend_Set_Colour_Adjust(float const * matrix, float brightness, float gamma)
{
	for (int index = 0; index < 9; index++) {
		_ColourAdjust.Matrix[index] = matrix[index];
	}
	_ColourAdjust.Brightness = brightness;
	_ColourAdjust.Gamma = gamma;

	if (!_Initialized) {
		return;
	}
	Update_Routing();
	Ensure_Targets();
}


void Backend_Set_View_Target(unsigned short view)
{
	bgfx::setViewFrameBuffer((bgfx::ViewId)view, Frame_Target());
}


void Backend_Request_Window_Capture(char const * filepath)
{
	if (!_Initialized) {
		return;
	}

	// Under HDR10 the window holds PQ values, so the capture reads the SDR picture back instead of the window.
	if (!_HdrActive) {
		bgfx::requestScreenShot(BGFX_INVALID_HANDLE, filepath);
		return;
	}
	_ReadbackPath = filepath;
	_ShotRequested = true;
}


/// <summary>
/// Names the graphics API the renderer settled on.
/// </summary>
char const * Backend_Renderer_Name(void)
{
	if (!_Initialized) {
		return("none");
	}
	return(bgfx::getRendererName(bgfx::getRendererType()));
}


/// <summary>
/// Logs the live texture, frame buffer and buffer counts and the memory they hold, tagged with a label.
/// </summary>
void Backend_Log_Resource_Counts(char const * when)
{
	if (!_Initialized) {
		return;
	}
	bgfx::Stats const * stats = bgfx::getStats();
	DebugString("GPU: %s; %u textures (%lld bytes), %u frame buffers, %u index buffers, %u vertex buffers, %u dynamic index buffers, %u dynamic vertex buffers, %lld bytes in render targets\n",
				when, (unsigned int)stats->numTextures, (long long)stats->textureMemoryUsed, (unsigned int)stats->numFrameBuffers,
				(unsigned int)stats->numIndexBuffers, (unsigned int)stats->numVertexBuffers, (unsigned int)stats->numDynamicIndexBuffers,
				(unsigned int)stats->numDynamicVertexBuffers, (long long)stats->rtMemoryUsed);
}
