$output v_texcoord1

// Draws the faces of one layer of a direct voxel part that are turned toward the viewer, straight into the map pass's
// scene target (code/gpumapvoxel.cpp), in place of vs_voxelscene.sc's cube outlines. A face is two triangles, six
// vertices from gl_VertexID, on the cube vs_voxelscene.sc gives its voxel, and its depth is that shader's d with no
// step per voxel. Faces that share a corner compute the same position for it, so the surface has no gaps.
//   s_vface      one RG32U texel a face (GpuVoxelFace in code/gpuvoxel.h): r holds x, y << 8, z << 16 and
//                direction << 24, g holds colour and normal << 8
//   u_vox        as vs_mapvoxel.sc describes it, with the projection stated for the layer's own axes
//                (Gpu_Voxel_Model_View) and u_vox[3].x unused
//   u_vdepth     as vs_voxelscene.sc describes it, with the steps along the layer's own axes; [0].w and [2].y are unused
//   u_vface[0]   xyz: the place in s_vface of the first face of each of the three directions drawn
//   u_vface[1]   xyz: the faces of each of them

#include "bgfx_shader.sh"

USAMPLER2D(s_vface, 11);
SAMPLER2D(s_vpal, 12);
uniform vec4 u_vdepth[3];
uniform vec4 u_vface[2];

#include "voxelshade.sh"

#define FACE_WIDTH 2048

void main()
{
	int id = int(gl_VertexID);
	int face = id / 6;
	int corner = id - face * 6;
	ivec3 first = ivec3(u_vface[0].xyz + vec3_splat(0.5));
	ivec3 count = ivec3(u_vface[1].xyz + vec3_splat(0.5));
	if (u_vdepth[2].x > 0.5) {
		face = count.x + count.y + count.z - 1 - face;
	}
	int texel = first.x + face;
	if (face >= count.x + count.y) {
		texel = first.z + face - count.x - count.y;
	} else if (face >= count.x) {
		texel = first.y + face - count.x;
	}
	uvec4 record = texelFetch(s_vface, ivec2(texel % FACE_WIDTH, texel / FACE_WIDTH), 0);
	ivec3 cell = ivec3(int(record.x & 255u), int((record.x >> 8) & 255u), int((record.x >> 16) & 255u));
	int direction = int(record.x >> 24);

	// The face lies half a voxel from the cube's centre along its axis, and its corners half a voxel along the other two.
	float out_of = (direction & 1) != 0 ? 0.5 : -0.5;
	float along = (corner == 1 || corner == 4 || corner == 5) ? 0.5 : -0.5;
	float across = (corner == 2 || corner == 3 || corner == 5) ? 0.5 : -0.5;
	int axis = direction / 2;
	vec3 offset = axis == 0 ? vec3(out_of, along, across) : (axis == 1 ? vec3(across, out_of, along) : vec3(along, across, out_of));

	int index;
	int strength;
	Voxel_Shade(int(record.y & 255u), int((record.y >> 8) & 255u), index, strength);
	vec2 at = Voxel_Cube_Place(Voxel_Point(cell), offset);
	float d = u_vdepth[1].x + dot(vec3(cell) + offset, u_vdepth[1].yzw);
	float depth = (u_vdepth[0].x + clamp(d, 0.0, 1.0)) * u_vdepth[0].y;
	// The corner goes on one of the rasterizer's 1/256-pixel steps before the division, as in vs_voxelscene.sc.
	gl_Position = mul(u_viewProj, vec4(round(at * 256.0) / 256.0 / u_vox[3].y, 0.0, 1.0));
	gl_Position.z = u_vdepth[0].z > 0.5 ? depth * 2.0 - 1.0 : depth;
	v_texcoord1 = vec4(float(index), float(strength), 0.0, 0.0);
}
