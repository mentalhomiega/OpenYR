$output v_texcoord1

// Draws one layer of a voxel part into its tile of the voxel image (code/gpumap.cpp), one quad per voxel.
// The vertices come from gl_VertexID: six a voxel, in the order the software drawer walks the layer. The
// quad lands on the pixel the software drawer writes (code/gpuvoxel.h) and carries the palette index it writes.
// At a scale above 1 the tile is that many times finer and a voxel is the silhouette of its cube instead: the three
// faces that meet at the corner nearest the viewer, eighteen vertices a voxel, at the voxel's unrounded position.
//   s_vmodel  two RGBA8 texels a voxel: (i, j, k, colour) and (normal, 0, 0, 0)
//   s_vpal    VoxelPaletteTranslateTable, 256 colours by 32 lighting rows
//   u_vox[0]  xy: the projection's origin, zw: its step along i
//   u_vox[1]  xy: its step along j, zw: its step along k
//   u_vox[2]  xy: the voxel buffer pixel (0, 0) in the image, z: 1 for a lit layer, which draws two pixels a voxel
//   u_vox[3]  x: the layer's first voxel in s_vmodel, y: the scale, 1 to 4
//   u_vlight  VoxelNormalTranslateTable, four bytes a vector

#include "bgfx_shader.sh"

SAMPLER2D(s_vmodel, 0);
SAMPLER2D(s_vpal, 1);
uniform vec4 u_vox[4];
uniform vec4 u_vlight[64];

#define MODEL_WIDTH 2048

vec2 Signed(vec2 v)
{
	return vec2(v.x > 32767.5 ? v.x - 65536.0 : v.x, v.y > 32767.5 ? v.y - 65536.0 : v.y);
}

void main()
{
	int scale = int(u_vox[3].y + 0.5);
	int perVoxel = scale > 1 ? 18 : 6;
	int id = int(gl_VertexID);
	int voxel = id / perVoxel;
	int corner = id - voxel * perVoxel;
	int texel = (int(u_vox[3].x + 0.5) + voxel) * 2;
	ivec4 a = ivec4(texelFetch(s_vmodel, ivec2(texel % MODEL_WIDTH, texel / MODEL_WIDTH), 0) * 255.0 + 0.5);
	ivec4 b = ivec4(texelFetch(s_vmodel, ivec2((texel + 1) % MODEL_WIDTH, (texel + 1) / MODEL_WIDTH), 0) * 255.0 + 0.5);

	int px = (int(u_vox[0].x + 0.5) + a.x * int(u_vox[0].z + 0.5) + a.y * int(u_vox[1].x + 0.5) + a.z * int(u_vox[1].z + 0.5)) & 65535;
	int py = (int(u_vox[0].y + 0.5) + a.x * int(u_vox[0].w + 0.5) + a.y * int(u_vox[1].y + 0.5) + a.z * int(u_vox[1].w + 0.5)) & 65535;

	bool lit = u_vox[2].z > 0.5;
	int index = a.w;
	if (lit) {
		int row = int(u_vlight[b.x / 4][b.x % 4] + 0.5);
		index = int(texelFetch(s_vpal, ivec2(a.w, row), 0).x * 255.0 + 0.5);
	}

	vec2 at;
	if (scale > 1) {
		// The cube's axes on screen, in pixels, and the centre the software's quad is centred on.
		// The steps are 16-bit values that wrap, so one above 32767 is negative.
		vec2 ex = Signed(vec2(u_vox[0].z, u_vox[0].w)) / 256.0;
		vec2 ey = Signed(vec2(u_vox[1].x, u_vox[1].y)) / 256.0;
		vec2 ez = Signed(vec2(u_vox[1].z, u_vox[1].w)) / 256.0;
		vec2 centre = vec2(float(px), float(py)) / 256.0 + vec2(lit ? 1.0 : 0.5, 0.5);
		// The corner of the cube that lies along the view direction is the one the three faces share.
		vec3 toward = vec3(ey.x * ez.y - ey.y * ez.x, ez.x * ex.y - ez.y * ex.x, ex.x * ey.y - ex.y * ey.x);
		vec3 side = vec3(toward.x < 0.0 ? -0.5 : 0.5, toward.y < 0.0 ? -0.5 : 0.5, toward.z < 0.0 ? -0.5 : 0.5);
		int face = corner / 6;
		int vertex = corner - face * 6;
		vec2 u = face == 0 ? ex : (face == 1 ? ey : ez);
		vec2 v = face == 0 ? ey : (face == 1 ? ez : ex);
		vec2 w = face == 0 ? ez : (face == 1 ? ex : ey);
		float wside = face == 0 ? side.z : (face == 1 ? side.x : side.y);
		vec2 c = vec2(-0.5, -0.5);
		if (vertex == 1) {
			c = vec2(0.5, -0.5);
		} else if (vertex == 2 || vertex == 4) {
			c = vec2(0.5, 0.5);
		} else if (vertex == 5) {
			c = vec2(-0.5, 0.5);
		}
		vec2 place = centre + w * wside + u * c.x + v * c.y;
		at = place * float(scale) + u_vox[2].xy;
	} else {
		int bx = px / 256;
		int by = py / 256;
		float width = lit ? 2.0 : 1.0;
		vec2 c = vec2(0.0, 0.0);
		if (corner == 1) {
			c = vec2(width, 0.0);
		} else if (corner == 2 || corner == 4) {
			c = vec2(width, 1.0);
		} else if (corner == 5) {
			c = vec2(0.0, 1.0);
		}
		at = vec2(float(bx) + u_vox[2].x, float(by) + u_vox[2].y) + c;
	}
	gl_Position = mul(u_viewProj, vec4(at, 0.0, 1.0));
	v_texcoord1 = vec4(float(index), 0.0, 0.0, 0.0);
}
