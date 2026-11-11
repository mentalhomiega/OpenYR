$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// The depth clear writes no colour (Submit_Depth_Reset in code/gpumap.cpp).

#include "bgfx_shader.sh"

void main()
{
	gl_FragColor = vec4_splat(0.0);
}
