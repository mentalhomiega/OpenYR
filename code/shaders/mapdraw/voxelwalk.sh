// One vertex of a voxel layer's walk, which vs_mapvoxel.sc and vs_voxelscene.sc draw the same way. The including
// shader declares s_vmodel and s_vpal; vs_mapvoxel.sc describes them, the uniforms and the primitives.

#include "voxelshade.sh"

#define MODEL_WIDTH 2048

// Places vertex id of the layer: at is its position in target pixels, cell its position along the walk's three axes
// in voxels, counted from the layer's first corner, voxel its voxel's place in the walk, and index and strength the
// palette index and emissive strength the voxel writes.
void Voxel_Vertex(int id, out vec2 at, out vec3 cell, out int voxel, out int index, out int strength)
{
	int scale = int(u_vox[3].y + 0.5);
	int perVoxel = scale > 1 ? 8 : 2;
	voxel = id / perVoxel;
	int corner = id - voxel * perVoxel;
	int texel = (int(u_vox[3].x + 0.5) + voxel) * 2;
	ivec4 a = ivec4(texelFetch(s_vmodel, ivec2(texel % MODEL_WIDTH, texel / MODEL_WIDTH), 0) * 255.0 + 0.5);
	ivec4 b = ivec4(texelFetch(s_vmodel, ivec2((texel + 1) % MODEL_WIDTH, (texel + 1) / MODEL_WIDTH), 0) * 255.0 + 0.5);

	ivec2 spot = Voxel_Point(a.xyz);
	Voxel_Shade(a.w, b.x, index, strength);

	cell = vec3(a.xyz);
	if (scale > 1) {
		// The corners along the view direction are the nearest and farthest; the other six are the outline, in order around it.
		vec2 ex = Signed(vec2(u_vox[0].z, u_vox[0].w));
		vec2 ey = Signed(vec2(u_vox[1].x, u_vox[1].y));
		vec2 ez = Signed(vec2(u_vox[1].z, u_vox[1].w));
		vec3 toward = vec3(ey.x * ez.y - ey.y * ez.x, ez.x * ex.y - ez.y * ex.x, ex.x * ey.y - ex.y * ey.x);
		vec3 side = vec3(toward.x < 0.0 ? -0.5 : 0.5, toward.y < 0.0 ? -0.5 : 0.5, toward.z < 0.0 ? -0.5 : 0.5);
		int h = corner <= 1 ? 0 : (corner == 2 ? 1 : (corner == 3 ? 5 : (corner == 4 ? 2 : (corner == 5 ? 4 : 3))));
		vec3 outline = vec3((h == 0 || h == 1 || h == 5) ? side.x : -side.x, (h == 1 || h == 2 || h == 3) ? side.y : -side.y, (h == 3 || h == 4 || h == 5) ? side.z : -side.z);
		at = Voxel_Cube_Place(spot, outline);
		cell += outline;
	} else {
		int bx = spot.x / 256;
		int by = spot.y / 256;
		float width = u_vox[2].z > 0.5 ? 2.0 : 1.0;
		at = vec2(float(bx) + u_vox[2].x + 0.5 + (corner == 1 ? width : 0.0), float(by) + u_vox[2].y + 0.5);
	}
}
