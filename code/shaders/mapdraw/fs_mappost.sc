$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Bends the drawn map picture around shockwave rings, adds the bloom glow and shades it under drifting
// clouds (code/gpumap.cpp).
//   v_texcoord0.zw  pixel position inside the map area
//   u_ripples[i]    xy: ring centre in area pixels, z: ring radius, w: strength in pixels
//   u_rippleinfo    x: ring count, yz: area size, w: glow strength (0 for none)
//   u_world         xy: map position of the area's top left pixel, z: time in seconds, w: how much a cloud
//                   darkens the picture (0 for no clouds)

#include "bgfx_shader.sh"

SAMPLER2D(s_scene, 0);
SAMPLER2D(s_glow, 1);
uniform vec4 u_ripples[16];
uniform vec4 u_rippleinfo;
uniform vec4 u_world;

float Hash(vec2 cell)
{
	return fract(sin(dot(cell, vec2(127.1, 311.7))) * 43758.5453);
}

float Noise(vec2 q)
{
	vec2 cell = floor(q);
	vec2 f = q - cell;
	f = f * f * (3.0 - 2.0 * f);
	float top = mix(Hash(cell), Hash(cell + vec2(1.0, 0.0)), f.x);
	float bottom = mix(Hash(cell + vec2(0.0, 1.0)), Hash(cell + vec2(1.0, 1.0)), f.x);
	return mix(top, bottom, f.y);
}

// How much of the sky over map pixel world is cloud, from 0 to 1. The clouds drift with the wind and
// change shape slowly as they go.
float Cloud(vec2 world, float t)
{
	// The ground is drawn at half height, so clouds over it look half as tall.
	vec2 q = world * vec2(1.0, 2.0) / 1000.0 + vec2(t * 0.01, t * 0.004);
	float n = 0.0;
	float weight = 0.5;
	for (int i = 0; i < 4; i++) {
		n += Noise(q + vec2(t * 0.003 * float(i), 0.0)) * weight;
		q = q * 2.03 + vec2(17.0, 9.0);
		weight *= 0.5;
	}
	return smoothstep(0.49, 0.57, n);
}

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
	vec3 rgb = colour.rgb * (1.0 + 0.2 * shade);
	if (u_rippleinfo.w > 0.0) {
		vec2 glowsize = floor((u_rippleinfo.yz + 3.0) / 4.0);
		rgb += texture2D(s_glow, (p - offset) / (glowsize * 4.0)).rgb * u_rippleinfo.w;
	}
	if (u_world.w > 0.0) {
		rgb *= 1.0 - u_world.w * Cloud(p + u_world.xy, u_world.z);
	}
	gl_FragColor = vec4(rgb, colour.a);
}
