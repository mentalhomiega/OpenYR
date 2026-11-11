$output v_texcoord1

// Draws one layer of a voxel part into its tile of the voxel image (code/gpumap.cpp), one primitive per voxel.
// The vertices come from gl_VertexID, in the order the software drawer walks the layer.
// At scale 1 a voxel is a horizontal line over the pixels the software drawer writes (code/gpuvoxel.h), two vertices,
// carrying the palette index it writes; a line covers the pixel its start is in and stops before the pixel its end is in.
// At a scale above 1 the tile is that many times finer and a voxel is the outline of its cube, the six corners other than
// the nearest and farthest, as a triangle strip of eight vertices at the voxel's unrounded position; the first and last
// vertices are doubled so that the strips of successive voxels join with empty triangles.
//   s_vmodel  two RGBA8 texels a voxel: (i, j, k, colour) and (normal, 0, 0, 0)
//   s_vpal    two blocks of 33 rows of 256 colours: VoxelPaletteTranslateTable's 32 lighting rows then the identity row for
//             an unlit layer, and the same rows run through the scratch surface's colour translation; the rows after
//             them are lighting tables, the lighting row of each of 256 normals
//   u_vox[0]  xy: the projection's origin, zw: its step along i
//   u_vox[1]  xy: its step along j, zw: its step along k
//   u_vox[2]  xy: the voxel buffer pixel (0, 0) in the image, z: 1 for a lit layer, which draws two pixels a voxel,
//             w: the first row of the block s_vpal is read from, 0 or 33
//   u_vox[3]  x: the layer's first voxel in s_vmodel, y: the scale, 1 to 4, z: the emissive strength of row 31,
//             w: the row of s_vpal that holds the layer's lighting table

#include "bgfx_shader.sh"

SAMPLER2D(s_vmodel, 0);
SAMPLER2D(s_vpal, 1);

#include "voxelwalk.sh"

void main()
{
	vec2 at;
	vec3 cell;
	int voxel;
	int index;
	int strength;
	Voxel_Vertex(int(gl_VertexID), at, cell, voxel, index, strength);
	gl_Position = mul(u_viewProj, vec4(at, 0.0, 1.0));
	v_texcoord1 = vec4(float(index), float(strength), 0.0, 0.0);
}
