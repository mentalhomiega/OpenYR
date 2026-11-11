// The uniforms, placement and shading that the voxel walk (voxelwalk.sh) and the face draw (vs_voxelface.sc) share.
// The including shader declares s_vpal.
//   s_vpal    two blocks of 33 rows of 256 colours: VoxelPaletteTranslateTable's 32 lighting rows then the identity row for
//             an unlit layer, and the same rows run through the scratch surface's colour translation; the rows after
//             them are lighting tables, the lighting row of each of 256 normals
//   u_vox[0]  xy: the projection's origin, zw: its step along i
//   u_vox[1]  xy: its step along j, zw: its step along k
//   u_vox[2]  xy: the voxel buffer pixel (0, 0) in the target, in target pixels, z: 1 for a lit layer, which draws two
//             pixels a voxel, w: the first row of the block s_vpal is read from, 0 or 33
//   u_vox[3]  x: the layer's first voxel in s_vmodel, y: the scale, 1 to 4, target pixels to one area pixel,
//             z: the emissive strength of row 31, w: the row of s_vpal that holds the layer's lighting table

uniform vec4 u_vox[4];

vec2 Signed(vec2 v)
{
	return vec2(v.x > 32767.5 ? v.x - 65536.0 : v.x, v.y > 32767.5 ? v.y - 65536.0 : v.y);
}

// The voxel buffer point the projection gives the voxel at cell, in 1/256 pixels, wrapped as the software drawer's is.
ivec2 Voxel_Point(ivec3 cell)
{
	int px = (int(u_vox[0].x + 0.5) + cell.x * int(u_vox[0].z + 0.5) + cell.y * int(u_vox[1].x + 0.5) + cell.z * int(u_vox[1].z + 0.5)) & 65535;
	int py = (int(u_vox[0].y + 0.5) + cell.x * int(u_vox[0].w + 0.5) + cell.y * int(u_vox[1].y + 0.5) + cell.z * int(u_vox[1].w + 0.5)) & 65535;
	return ivec2(px, py);
}

// The target position, in target pixels, of the place offset voxels from the centre of the cube of the voxel whose
// buffer point is spot. The software stamps a voxel at its point, so its cube is centred on the quad drawn there.
vec2 Voxel_Cube_Place(ivec2 spot, vec3 offset)
{
	// The steps are 16-bit values that wrap, so one above 32767 is negative.
	vec2 ex = Signed(vec2(u_vox[0].z, u_vox[0].w)) / 256.0;
	vec2 ey = Signed(vec2(u_vox[1].x, u_vox[1].y)) / 256.0;
	vec2 ez = Signed(vec2(u_vox[1].z, u_vox[1].w)) / 256.0;
	vec2 centre = vec2(float(spot.x), float(spot.y)) / 256.0 + vec2(u_vox[2].z > 0.5 ? 1.0 : 0.5, 0.5);
	vec2 place = centre + ex * offset.x + ey * offset.y + ez * offset.z;
	return place * u_vox[3].y + u_vox[2].xy;
}

// The palette index and the emissive strength a voxel of colour and normal writes.
void Voxel_Shade(int colour, int normal, out int index, out int strength)
{
	bool lit = u_vox[2].z > 0.5;
	int row = int(texelFetch(s_vpal, ivec2(normal, int(u_vox[3].w + 0.5)), 0).x * 255.0 + 0.5);
	index = int(texelFetch(s_vpal, ivec2(colour, int(u_vox[2].w + 0.5) + row), 0).x * 255.0 + 0.5);

	// The strength the software drawers write beside the index (Emissive_Voxel_Highlight); an unlit layer writes none.
	strength = 0;
	if (lit && row > 16) {
		int top = int(u_vox[3].z + 0.5);
		strength = row >= 31 ? top : (row - 16) * top / 15;
	}
}
