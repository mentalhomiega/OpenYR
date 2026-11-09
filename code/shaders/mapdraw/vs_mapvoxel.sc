$output v_texcoord1

// Draws one layer of a voxel part into its tile of the voxel image (code/gpumap.cpp), one quad per voxel.
// The vertices come from gl_VertexID: six a voxel, in the order the software drawer walks the layer. The
// quad lands on the pixel the software drawer writes (code/gpuvoxel.h) and carries the palette index it writes.
//   s_vmodel  two RGBA8 texels a voxel: (i, j, k, colour) and (normal, 0, 0, 0)
//   s_vpal    two blocks of 33 rows of 256 colours: VoxelPaletteTranslateTable's 32 lighting rows then the identity row for
//             an unlit layer, and the same rows run through the scratch surface's colour translation
//   u_vox[0]  xy: the projection's origin, zw: its step along i
//   u_vox[1]  xy: its step along j, zw: its step along k
//   u_vox[2]  xy: the voxel buffer pixel (0, 0) in the image, z: 1 for a lit layer, which draws two pixels a voxel,
//             w: the first row of the block s_vpal is read from, 0 or 33
//   u_vox[3]  x: the layer's first voxel in s_vmodel
//   u_vlight  VoxelNormalTranslateTable, four bytes a vector

#include "bgfx_shader.sh"

SAMPLER2D(s_vmodel, 0);
SAMPLER2D(s_vpal, 1);
uniform vec4 u_vox[4];
uniform vec4 u_vlight[64];

#define MODEL_WIDTH 2048

void main()
{
	int id = int(gl_VertexID);
	int voxel = id / 6;
	int corner = id - voxel * 6;
	int texel = (int(u_vox[3].x + 0.5) + voxel) * 2;
	ivec4 a = ivec4(texelFetch(s_vmodel, ivec2(texel % MODEL_WIDTH, texel / MODEL_WIDTH), 0) * 255.0 + 0.5);
	ivec4 b = ivec4(texelFetch(s_vmodel, ivec2((texel + 1) % MODEL_WIDTH, (texel + 1) / MODEL_WIDTH), 0) * 255.0 + 0.5);

	int px = (int(u_vox[0].x + 0.5) + a.x * int(u_vox[0].z + 0.5) + a.y * int(u_vox[1].x + 0.5) + a.z * int(u_vox[1].z + 0.5)) & 65535;
	int py = (int(u_vox[0].y + 0.5) + a.x * int(u_vox[0].w + 0.5) + a.y * int(u_vox[1].y + 0.5) + a.z * int(u_vox[1].w + 0.5)) & 65535;
	int bx = px / 256;
	int by = py / 256;

	bool lit = u_vox[2].z > 0.5;
	int row = 32;
	if (lit) {
		row = int(u_vlight[b.x / 4][b.x % 4] + 0.5);
	}
	int index = int(texelFetch(s_vpal, ivec2(a.w, int(u_vox[2].w + 0.5) + row), 0).x * 255.0 + 0.5);

	float w = lit ? 2.0 : 1.0;
	vec2 c = vec2(0.0, 0.0);
	if (corner == 1) {
		c = vec2(w, 0.0);
	} else if (corner == 2 || corner == 4) {
		c = vec2(w, 1.0);
	} else if (corner == 5) {
		c = vec2(0.0, 1.0);
	}
	vec2 at = vec2(float(bx) + u_vox[2].x, float(by) + u_vox[2].y) + c;
	gl_Position = mul(u_viewProj, vec4(at, 0.0, 1.0));
	v_texcoord1 = vec4(float(index), 0.0, 0.0, 0.0);
}
