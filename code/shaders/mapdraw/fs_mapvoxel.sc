$input v_texcoord1

// Writes the palette index the vertex shader chose and the voxel's emissive strength into the voxel image (code/gpumap.cpp).

#include "bgfx_shader.sh"

void main()
{
	gl_FragColor = vec4(v_texcoord1.x / 255.0, v_texcoord1.y / 255.0, 0.0, 1.0);
}
