// One vertex of a voxel layer's walk, which vs_mapvoxel.sc and vs_voxelscene.sc draw the same way. The including
// shader declares s_vmodel and s_vpal; vs_mapvoxel.sc describes them, the uniforms and the primitives.

uniform vec4 u_vox[4];
uniform vec4 u_vlight[64];

#define MODEL_WIDTH 2048

vec2 Signed(vec2 v)
{
	return vec2(v.x > 32767.5 ? v.x - 65536.0 : v.x, v.y > 32767.5 ? v.y - 65536.0 : v.y);
}

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

	int px = (int(u_vox[0].x + 0.5) + a.x * int(u_vox[0].z + 0.5) + a.y * int(u_vox[1].x + 0.5) + a.z * int(u_vox[1].z + 0.5)) & 65535;
	int py = (int(u_vox[0].y + 0.5) + a.x * int(u_vox[0].w + 0.5) + a.y * int(u_vox[1].y + 0.5) + a.z * int(u_vox[1].w + 0.5)) & 65535;

	bool lit = u_vox[2].z > 0.5;
	int row = 32;
	if (lit) {
		row = int(u_vlight[b.x / 4][b.x % 4] + 0.5);
	}
	index = int(texelFetch(s_vpal, ivec2(a.w, int(u_vox[2].w + 0.5) + row), 0).x * 255.0 + 0.5);

	// The strength the software drawers write beside the index (Emissive_Voxel_Highlight); an unlit layer writes none.
	strength = 0;
	if (lit && row > 16) {
		int top = int(u_vox[3].z + 0.5);
		strength = row >= 31 ? top : (row - 16) * top / 15;
	}

	// The software stamps a voxel at its walk point, so its cube is centred there.
	cell = vec3(a.xyz);
	if (scale > 1) {
		// The cube's axes on screen, in pixels, and the centre the software's quad is centred on.
		// The steps are 16-bit values that wrap, so one above 32767 is negative.
		vec2 ex = Signed(vec2(u_vox[0].z, u_vox[0].w)) / 256.0;
		vec2 ey = Signed(vec2(u_vox[1].x, u_vox[1].y)) / 256.0;
		vec2 ez = Signed(vec2(u_vox[1].z, u_vox[1].w)) / 256.0;
		vec2 centre = vec2(float(px), float(py)) / 256.0 + vec2(lit ? 1.0 : 0.5, 0.5);
		// The corners along the view direction are the nearest and farthest; the other six are the outline, in order around it.
		vec3 toward = vec3(ey.x * ez.y - ey.y * ez.x, ez.x * ex.y - ez.y * ex.x, ex.x * ey.y - ex.y * ey.x);
		vec3 side = vec3(toward.x < 0.0 ? -0.5 : 0.5, toward.y < 0.0 ? -0.5 : 0.5, toward.z < 0.0 ? -0.5 : 0.5);
		int h = corner <= 1 ? 0 : (corner == 2 ? 1 : (corner == 3 ? 5 : (corner == 4 ? 2 : (corner == 5 ? 4 : 3))));
		vec3 outline = vec3((h == 0 || h == 1 || h == 5) ? side.x : -side.x, (h == 1 || h == 2 || h == 3) ? side.y : -side.y, (h == 3 || h == 4 || h == 5) ? side.z : -side.z);
		vec2 place = centre + ex * outline.x + ey * outline.y + ez * outline.z;
		at = place * float(scale) + u_vox[2].xy;
		cell += outline;
	} else {
		int bx = px / 256;
		int by = py / 256;
		float width = lit ? 2.0 : 1.0;
		at = vec2(float(bx) + u_vox[2].x + 0.5 + (corner == 1 ? width : 0.0), float(by) + u_vox[2].y + 0.5);
	}
}
