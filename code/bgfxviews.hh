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
// Include <bgfx/bgfx.h> and <bgfx/embedded_shader.h> before this header.
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


// Builds a program from one vertex and one fragment shader of the embedded tables for the running renderer.
// Returns an invalid handle, with nothing left allocated, when either shader is missing.
inline bgfx::ProgramHandle Create_Embedded_Program(bgfx::EmbeddedShader const * vertex_table, char const * vertex_name, bgfx::EmbeddedShader const * fragment_table, char const * fragment_name)
{
	bgfx::RendererType::Enum const type = bgfx::getRendererType();
	bgfx::ShaderHandle vertexshader = bgfx::createEmbeddedShader(vertex_table, type, vertex_name);
	bgfx::ShaderHandle fragmentshader = bgfx::createEmbeddedShader(fragment_table, type, fragment_name);

	if (!bgfx::isValid(vertexshader) || !bgfx::isValid(fragmentshader)) {
		Release_Handle(vertexshader);
		Release_Handle(fragmentshader);
		bgfx::ProgramHandle invalid = BGFX_INVALID_HANDLE;
		return(invalid);
	}
	return(bgfx::createProgram(vertexshader, fragmentshader, true));
}


// The vertex layout of the overlay and ImGui draws: position, texture coordinate, packed colour.
inline bgfx::VertexLayout Make_Ui_Vertex_Layout(void)
{
	bgfx::VertexLayout layout;
	layout.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();
	return(layout);
}
