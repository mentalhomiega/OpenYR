$input a_position, a_texcoord0, a_texcoord1, a_texcoord2, a_texcoord3
$output v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Passes one recorded blit's quad through; the per-blit values are the same at every corner.

#include "bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_viewProj, vec4(a_position, 0.0, 1.0));
	v_texcoord0 = a_texcoord0;
	v_texcoord1 = a_texcoord1;
	v_texcoord2 = a_texcoord2;
	v_texcoord3 = a_texcoord3;
}
