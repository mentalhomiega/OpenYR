/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


enum BgfxViewType : unsigned short
{
	// The GPU map passes run first so that the present in the same frame draws their result.
	VIEW_MAPSHADOW,
	VIEW_MAPBLURX,
	VIEW_MAPBLURY,
	VIEW_MAPDRAW,
	// Each sonic wave in a map pass continues the drawing in the next of these views.
	VIEW_MAPWAVE,
	VIEW_MAPWAVE_LAST = VIEW_MAPWAVE + 7,
	VIEW_MAPGLOW,
	VIEW_MAPGLOWX,
	VIEW_MAPGLOWY,
	VIEW_MAPPOST,
	VIEW_PRESCALE,
	VIEW_PRESENT,
	VIEW_UI,
	VIEW_DEV,
	VIEW_MASK,
	VIEW_COMPOSITE,
	VIEW_SHOT,
	VIEW_READBACK,
	VIEW_MAPREAD,
};


// Destroys a valid handle and resets it to invalid. An invalid handle is left as it is.
// Include <bgfx/bgfx.h> before this header.
template<class Handle>
inline void Release_Handle(Handle & handle)
{
	if (bgfx::isValid(handle)) {
		bgfx::destroy(handle);
		handle = BGFX_INVALID_HANDLE;
	}
}


// An embedded-shader table entry that carries a Direct3D 11 blob only, as built by the shader compiler
// into <name>.bin.h. Other renderers get no shader.
#define BGFX_EMBEDDED_SHADER_D3D11(name) 	{ 		#name, 		{ 			{ bgfx::RendererType::Direct3D11, name##_dxbc, sizeof(name##_dxbc) }, 			{ bgfx::RendererType::Count, NULL, 0 }, 		} 	}

