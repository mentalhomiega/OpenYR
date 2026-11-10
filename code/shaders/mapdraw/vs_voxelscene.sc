$output v_texcoord1

// Draws one layer of a direct voxel part straight into the map pass's scene target (code/gpumapvoxel.cpp), with the
// primitives and uniforms of vs_mapvoxel.sc, where u_vox[2].xy is now the voxel buffer pixel (0, 0) in the target,
// in target pixels, scale of which make one area pixel.
// Each vertex gets a depth in its blit's slice (GPU_VOXEL_SLICES in code/gpuvoxel.h):
// (slice base + clamp(d, 0, 1)) / slices, with d = u_vdepth[1].x + voxel * u_vdepth[0].w + dot(cell, u_vdepth[1].yzw).
//   u_vdepth[0]  x: GPU_VOXEL_SLICES - 1 minus the slice, y: 1 / GPU_VOXEL_SLICES, z: 1 when clip depth runs from -1,
//                w: the step of d per voxel along the walk
//   u_vdepth[1]  x: d at the layer's first corner, yzw: the steps of d per voxel along the walk's three axes
//   u_vdepth[2]  x: 1 to draw the layer's voxels last first, y: the layer's voxels
// The samplers sit above the fragment shader's, because a stage binds to both shaders.

#include "bgfx_shader.sh"

SAMPLER2D(s_vmodel, 11);
SAMPLER2D(s_vpal, 12);
uniform vec4 u_vdepth[3];

#include "voxelwalk.sh"

void main()
{
	vec2 at;
	vec3 cell;
	int voxel;
	int index;
	int strength;
	int id = int(gl_VertexID);
	if (u_vdepth[2].x > 0.5) {
		// The voxels are taken last first, each with its corners in their usual order, so a line covers the same pixels.
		int perVoxel = u_vox[3].y > 1.5 ? 8 : 2;
		id = (int(u_vdepth[2].y + 0.5) - 1 - id / perVoxel) * perVoxel + id % perVoxel;
	}
	Voxel_Vertex(id, at, cell, voxel, index, strength);
	float d = u_vdepth[1].x + float(voxel) * u_vdepth[0].w + dot(cell, u_vdepth[1].yzw);
	float depth = (u_vdepth[0].x + clamp(d, 0.0, 1.0)) * u_vdepth[0].y;
	// The map view's projection maps area pixels. At scale 3 the division is inexact and a corner can fall halfway
	// between two of the rasterizer's 1/256-pixel steps, so the corner is put on a step first, rounding a tie to even
	// as the rasterizer does for the voxel image.
	gl_Position = mul(u_viewProj, vec4(round(at * 256.0) / 256.0 / u_vox[3].y, 0.0, 1.0));
	gl_Position.z = u_vdepth[0].z > 0.5 ? depth * 2.0 - 1.0 : depth;
	v_texcoord1 = vec4(float(index), float(strength), 0.0, 0.0);
}
