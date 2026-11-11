$input a_position, a_texcoord0, a_texcoord1, a_texcoord2, a_texcoord3
$output v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Puts a quad of the map pass on the far plane, which empties the depth under it (Submit_Depth_Reset in
// code/gpumap.cpp).

#include "bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_viewProj, vec4(a_position, 0.0, 1.0));
	gl_Position.z = gl_Position.w;
	v_texcoord0 = a_texcoord0;
	v_texcoord1 = a_texcoord1;
	v_texcoord2 = a_texcoord2;
	v_texcoord3 = a_texcoord3;
}
