$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Builds the emissive mask of a pass with GPU voxels (code/gpumap.cpp): the CPU plane copied unchanged, then 0 under
// each voxel blit where the plane still holds the mark the blit left. fs_voxelscene.sc then writes the highlight
// strengths of the blit's part there.
//   v_texcoord0.zw  pixel position inside the map area
//   v_texcoord1     z: 0 for the copy of the plane, 1 for a voxel blit

#include "bgfx_shader.sh"

SAMPLER2D(s_glowmask, 0);

// GPU_VOXEL_MASK_MARK in code/gpumapvoxel.h.
#define VOXEL_MARK 1

void main()
{
	ivec2 area = ivec2(floor(v_texcoord0.zw));
	int plane = int(texelFetch(s_glowmask, area, 0).x * 255.0 + 0.5);
	if (v_texcoord1.z < 0.5) {
		gl_FragColor = vec4_splat(float(plane) / 255.0);
		return;
	}
	if (plane != VOXEL_MARK) {
		discard;
	}
	gl_FragColor = vec4_splat(0.0);
}
