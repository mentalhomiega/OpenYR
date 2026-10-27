$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Draws one recorded blit of the map view's object pass (code/gpudraw.h) the way the software
// blitters do; darken and the translucent kinds finish in the hardware blend (gpumap.cpp).
//   v_texcoord0.xy  pixel position inside the blit (x, y), interpolated
//   v_texcoord0.zw  pixel position inside the map area, interpolated
//   v_texcoord1     x: offset of the blit's colour indices in s_index, y: blit width,
//                   z: first row of its translator in s_lut, w: row of s_alut for its lighting level, -1 for none
//   v_texcoord2     x: depth of the first row, y: fraction, z: numerator, w: denominator (0 when every row has x)
//   v_texcoord3     x: depth step, y: offset of its depth shape in s_zshape (-1 for none), z: kind
// Kind: 0 copy, 1 darken, 2 75% source, 3 half, 4 25% source, 5 the area's base picture, 6 copy without depth test,
// 7 one pixel of the 565 colour in v_texcoord1.x.

#include "bgfx_shader.sh"

SAMPLER2D(s_index, 0);
SAMPLER2D(s_lut, 1);
SAMPLER2D(s_alut, 2);
SAMPLER2D(s_zbuf, 3);
SAMPLER2D(s_abuf, 4);
SAMPLER2D(s_zshape, 5);
SAMPLER2D(s_base, 6);

#define ARENA_WIDTH 4096

int Fetch8(sampler2D s, int offset)
{
	return int(texelFetch(s, ivec2(offset % ARENA_WIDTH, offset / ARENA_WIDTH), 0).x * 255.0 + 0.5);
}

int Fetch16(sampler2D s, ivec2 p)
{
	return int(texelFetch(s, p, 0).x * 65535.0 + 0.5);
}

vec3 Rgb565(int c)
{
	return vec3(float((c / 2048) % 32) / 31.0, float((c / 32) % 64) / 63.0, float(c % 32) / 31.0);
}

void main()
{
	int kind = int(v_texcoord3.z + 0.5);
	ivec2 local = ivec2(floor(v_texcoord0.xy));
	ivec2 area = ivec2(floor(v_texcoord0.zw));

	if (kind == 5) {
		gl_FragColor = vec4(Rgb565(Fetch16(s_base, area)), 1.0);
		return;
	}

	if (kind == 7) {
		gl_FragColor = vec4(Rgb565(int(v_texcoord1.x + 0.5)), 1.0);
		return;
	}

	int width = int(v_texcoord1.y + 0.5);
	int index = Fetch8(s_index, int(v_texcoord1.x + 0.5) + local.y * width + local.x);
	if (index == 0) {
		discard;
	}

	if (kind != 6) {
		int z = int(v_texcoord2.x);
		int denominator = int(v_texcoord2.w + 0.5);
		if (denominator != 0) {
			z += int(v_texcoord3.x) * ((int(v_texcoord2.y + 0.5) + local.y * denominator) / int(v_texcoord2.z + 0.5));
		}
		int shape = 0;
		if (v_texcoord3.y >= 0.0) {
			shape = Fetch8(s_zshape, int(v_texcoord3.y + 0.5) + local.y * width + local.x);
			if (shape > 127) {
				shape -= 256;
			}
		}
		if (!((z - shape) < Fetch16(s_zbuf, area))) {
			discard;
		}
	}

	// The software blends floor each halved or quartered term. The source term is floored here; the
	// blend scales the target by alpha and takes off rgb, which is centred on the flooring loss of the
	// target term.
	vec3 unit = vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0);
	if (kind == 1) {
		gl_FragColor = vec4(0.25 * unit, 0.5);
		return;
	}

	int band = 0;
	if (v_texcoord1.w >= 0.0) {
		// The alpha buffer value indexes on past the row's end the way the software lookup does.
		int entry = int(v_texcoord1.w + 0.5) * 256 + Fetch16(s_abuf, area);
		band = Fetch16(s_alut, ivec2(entry % 256, entry / 256));
	}
	int value = index + band;
	int colour = Fetch16(s_lut, ivec2(value % 256, int(v_texcoord1.z + 0.5) + value / 256));
	if (kind == 0 || kind == 6) {
		gl_FragColor = vec4(Rgb565(colour), 1.0);
		return;
	}
	ivec3 channel = ivec3((colour / 2048) % 32, (colour / 32) % 64, colour % 32);
	vec3 source;
	vec3 loss;
	float keep;
	if (kind == 2) {
		source = vec3(channel / 4) * 3.0;
		loss = vec3_splat(0.375);
		keep = 0.25;
	} else if (kind == 3) {
		source = vec3(channel / 2);
		loss = vec3_splat(0.25);
		keep = 0.5;
	} else {
		source = vec3(channel / 4);
		loss = vec3_splat(1.125);
		keep = 0.75;
	}
	gl_FragColor = vec4(max(source - loss, vec3_splat(0.0)) * unit, keep);
}
