$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Bends the drawn map picture around shockwave rings (code/gpumap.cpp).
//   v_texcoord0.zw  pixel position inside the map area
//   u_ripples[i]    xy: ring centre in area pixels, z: ring radius, w: strength in pixels
//   u_rippleinfo    x: ring count, yz: area size

#include "bgfx_shader.sh"

SAMPLER2D(s_scene, 0);
uniform vec4 u_ripples[16];
uniform vec4 u_rippleinfo;

void main()
{
	vec2 p = floor(v_texcoord0.zw) + 0.5;
	vec2 offset = vec2_splat(0.0);
	float shade = 0.0;
	int count = int(u_rippleinfo.x + 0.5);
	for (int i = 0; i < 16; i++) {
		if (i < count) {
			vec4 ripple = u_ripples[i];
			// The ground is drawn at half height, so a ring on it is twice as wide as it is tall.
			vec2 d = (p - ripple.xy) * vec2(1.0, 2.0);
			float distance = length(d);
			float band = (distance - ripple.z) / 20.0;
			if (abs(band) < 1.0) {
				float wave = sin(band * 3.14159265) * (1.0 - band * band);
				offset += d / max(distance, 1.0) * vec2(1.0, 0.5) * wave * ripple.w;
				shade += wave * ripple.w / 16.0;
			}
		}
	}
	ivec2 size = ivec2(u_rippleinfo.yz);
	ivec2 at = clamp(ivec2(floor(p - offset)), ivec2(0, 0), size - ivec2(1, 1));
	// The ring's outer slope is lit and its inner slope shaded, so it reads as a raised wave.
	vec4 colour = texelFetch(s_scene, at, 0);
	gl_FragColor = vec4(colour.rgb * (1.0 + 0.2 * shade), colour.a);
}
