$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Draws one recorded blit of the map view's object pass (code/gpudraw.h) the way the software
// blitters do; darken and the translucent kinds finish in the hardware blend (gpumap.cpp).
//   v_texcoord0.xy  pixel position inside the blit (x, y), interpolated
//   v_texcoord0.zw  pixel position inside the map area, interpolated
//   v_texcoord1     x: offset of the blit's colour indices in s_index, y: blit width,
//                   z: first row of its translator in s_lut, w: row of s_alut for its lighting level, -1 for none
//   v_texcoord2     x: depth of the first row, y: fraction, z: numerator, w: denominator (0 when every row has x)
//   v_texcoord3     x: depth step, y: offset of its depth shape in s_zshape (-1 for none), z: kind,
//                   w: 1 to skip the depth test
// Kind: 0 copy, 1 darken, 2 75% source, 3 half, 4 25% source, 5 the area's base picture, 6 copy without depth test,
// 7 a solid fill of the 565 colour in v_texcoord1.x, 8 a shadow's coverage written as 1 into the shadow mask,
// 9 the base picture darkened by the blurred shadow mask in s_shadow, 10 a spot light's brightening factor
// (v_texcoord1.z holds the channels it leaves alone), 11 depth-shaded line pixels whose index is the alpha
// buffer value under each (v_texcoord1.z holds the 565 colour, w the fade; the target's weight goes out in alpha),
// 12 additive line pixels whose index is the alpha buffer value (v_texcoord1.z holds 0xRRGGBB), 13 antialiased line
// pixels whose index is the colour weight plus 1 (v_texcoord1.z holds 0xRRGGBB; the target's weight goes out in alpha),
// 14 sonic wave pixels whose index is the amplitude plus 1, shifted and tinted from s_snapshot (v_texcoord1.z holds
// the facing plus 8 for a magnetron beam, w the tint at amplitude 0), 15 the tile layer's shadows from s_tileshadow
// written as 1 into the shadow mask.
// u_water: x the time in seconds, y how far water ripples shift the base in pixels (0 for none), z how much they
// brighten and darken it, w how bright the glints of sunlight on them are.

#include "bgfx_shader.sh"

SAMPLER2D(s_index, 0);
SAMPLER2D(s_lut, 1);
SAMPLER2D(s_alut, 2);
SAMPLER2D(s_zbuf, 3);
SAMPLER2D(s_abuf, 4);
SAMPLER2D(s_zshape, 5);
SAMPLER2D(s_base, 6);
SAMPLER2D(s_shadow, 7);
SAMPLER2D(s_snapshot, 8);
SAMPLER2D(s_tileshadow, 9);
uniform vec4 u_ring[2];
uniform vec4 u_water;

#define ARENA_WIDTH 4096

int Fetch8(sampler2D s, int offset)
{
	return int(texelFetch(s, ivec2(offset % ARENA_WIDTH, offset / ARENA_WIDTH), 0).x * 255.0 + 0.5);
}

int Fetch16(sampler2D s, ivec2 p)
{
	return int(texelFetch(s, p, 0).x * 65535.0 + 0.5);
}

// The texel of a ring buffer holding area pixel p; ring is u_ring[i] (start, width, size).
ivec2 Ring(ivec2 p, vec4 ring)
{
	int width = int(ring.y + 0.5);
	int index = (int(ring.x + 0.5) + p.y * width + p.x) % int(ring.z + 0.5);
	return ivec2(index % width, index / width);
}

vec3 Rgb565(int c)
{
	return vec3(float((c / 2048) % 32) / 31.0, float((c / 32) % 64) / 63.0, float(c % 32) / 31.0);
}

// The bits of the tile layer's plane at p: 128 a shadow, 1 water (code/shadowplane.h).
int Plane(ivec2 p)
{
	return int(texelFetch(s_tileshadow, p, 0).x * 255.0 + 0.5);
}

bool Water(ivec2 p)
{
	return Plane(p) % 2 == 1;
}

// The base at water pixel p, shifted and shaded by two waves crossing the ground. A neighbour that
// is not water reads as p itself, so the shore never pulls land into the water.
vec3 Water_Base(ivec2 p, vec3 still)
{
	vec2 at = vec2(p);
	// The ground is drawn at half height, so the waves run twice as fast down the screen.
	float a = at.x * 0.045 + at.y * 0.09 + u_water.x * 1.6;
	float b = at.x * -0.06 + at.y * 0.12 + u_water.x * 1.1;
	vec2 shift = vec2(sin(a) + 0.6 * sin(b), 0.5 * (cos(a) - 0.6 * cos(b))) * u_water.y;
	vec2 q = at + shift;
	ivec2 q0 = ivec2(floor(q));
	vec2 f = q - floor(q);
	ivec2 last = textureSize(s_base, 0) - ivec2(1, 1);
	vec3 c[4];
	for (int i = 0; i < 4; i++) {
		ivec2 t = clamp(q0 + ivec2(i % 2, i / 2), ivec2(0, 0), last);
		c[i] = Water(t) ? Rgb565(Fetch16(s_base, t)) : still;
	}
	vec3 colour = mix(mix(c[0], c[1], f.x), mix(c[2], c[3], f.x), f.y);
	colour *= 1.0 + u_water.z * (cos(a) + 0.6 * cos(b));
	// Three finer waves at unrelated angles meet in short-lived peaks, which show as glints along the
	// crests of the swell; a slow wide wave thins them out in drifting patches.
	float g = sin(at.x * 0.17 + at.y * 0.29 + u_water.x * 2.9) * sin(at.x * 0.21 - at.y * 0.37 - u_water.x * 2.3)
		* sin(at.x * -0.26 + at.y * 0.13 + u_water.x * 1.7);
	float crest = max(cos(a) + 0.6 * cos(b), 0.0) / 1.6;
	float patches = max(sin(at.x * 0.013 - at.y * 0.021 + u_water.x * 0.4) + 0.3 * sin(at.x * 0.029 + at.y * 0.011 - u_water.x * 0.3), 0.0);
	float glint = pow(max(g, 0.0) * 1.3, 6.0) * crest * crest * crest * patches;
	return min(colour + glint * u_water.w, vec3_splat(1.0));
}

void main()
{
	int kind = int(v_texcoord3.z + 0.5);
	ivec2 local = ivec2(floor(v_texcoord0.xy));
	ivec2 area = ivec2(floor(v_texcoord0.zw));

	if (kind == 5 || kind == 9) {
		vec3 base = Rgb565(Fetch16(s_base, area));
		if (u_water.y > 0.0 && Water(area)) {
			base = Water_Base(area, base);
		}
		if (kind == 9) {
			base *= 1.0 - 0.5 * texelFetch(s_shadow, area, 0).x;
		}
		gl_FragColor = vec4(base, 1.0);
		return;
	}

	if (kind == 7) {
		gl_FragColor = vec4(Rgb565(int(v_texcoord1.x + 0.5)), 1.0);
		return;
	}

	if (kind == 15) {
		if (Plane(area) < 128) {
			discard;
		}
		gl_FragColor = vec4_splat(1.0);
		return;
	}

	int width = int(v_texcoord1.y + 0.5);
	int index = Fetch8(s_index, int(v_texcoord1.x + 0.5) + local.y * width + local.x);
	if (index == 0) {
		discard;
	}

	if (kind == 11) {
		int c = int(v_texcoord1.z + 0.5);
		int fade = int(v_texcoord1.w + 0.5);
		ivec3 c8 = ivec3(((c / 2048) % 32) * 8, ((c / 32) % 64) * 4, (c % 32) * 8);
		vec3 scale = vec3(1.0 / (8.0 * 31.0), 1.0 / (4.0 * 63.0), 1.0 / (8.0 * 31.0));
		if (fade >= 256) {
			if (index == 127) {
				gl_FragColor = vec4(Rgb565(c), 0.0);
				return;
			}
			ivec3 shaded = (c8 * index) / 128;
			gl_FragColor = vec4(min(vec3(shaded / ivec3(8, 4, 8)) * vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0), vec3_splat(1.0)), 0.0);
			return;
		}
		int amount = max(fade, 0);
		vec3 source = vec3((c8 * amount) / 256) * (float(index) / 128.0);
		gl_FragColor = vec4(source * scale, float((256 - amount) * index) / 32768.0);
		return;
	}

	if (kind == 14) {
		// WaveClass::Sonic_Shift and WaveClass::Sonic_Tint.
		int amplitude = index - 1;
		int wave = int(v_texcoord1.z + 0.5);
		int facing = wave % 8;
		int count = amplitude < 2 ? 0 : (amplitude < 7 ? 1 : (amplitude < 10 ? 2 : 3));
		ivec2 step = ivec2(facing == 0 || facing == 4 ? 0 : (facing < 4 ? 1 : -1), facing == 2 || facing == 6 ? 0 : (facing > 2 && facing < 6 ? 1 : -1));
		ivec2 source = area + step * count;
		ivec2 size = ivec2(textureSize(s_snapshot, 0));
		if (source.x < 0 || source.y < 0 || source.x >= size.x || source.y >= size.y) {
			source = area;
		}
		vec3 under = texelFetch(s_snapshot, source, 0).xyz;
		ivec3 c8 = ivec3(floor(under * vec3(31.0, 63.0, 31.0) + 0.5)) * ivec3(8, 4, 8);
		int mult = int(v_texcoord1.w + 0.5) + amplitude * 8;
		if (wave >= 8) {
			c8.x = min(255, c8.x + ((mult / 2) * c8.x) / 256);
			c8.z = min(255, c8.z + (c8.z * mult * 4) / 256);
		} else {
			c8.y = min(255, c8.y + (c8.y * mult) / 256);
			c8.z = min(255, c8.z + (c8.z * mult) / 256);
		}
		gl_FragColor = vec4(vec3(c8 / ivec3(8, 4, 8)) * vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0), 1.0);
		return;
	}

	if (kind == 12 || kind == 13) {
		int c = int(v_texcoord1.z + 0.5);
		ivec3 c8 = ivec3((c / 65536) % 256, (c / 256) % 256, c % 256);
		vec3 scale = vec3(1.0 / (8.0 * 31.0), 1.0 / (4.0 * 63.0), 1.0 / (8.0 * 31.0));
		if (kind == 12) {
			ivec3 added = (c8 * index) / 128;
			gl_FragColor = vec4(vec3(added / ivec3(8, 4, 8)) * vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0), 0.0);
			return;
		}
		int weight = index - 1;
		float t = float(weight) / 128.0 - 1.0;
		float front = floor((t * t * t + 1.0) * 128.0);
		float back = floor((1.0 - t * t * t) * 128.0);
		float cov = float(Fetch16(s_abuf, Ring(area, u_ring[1])));
		gl_FragColor = vec4(vec3(c8) * (cov * front / 32768.0) * scale, cov * back / 32768.0);
		return;
	}

	if (kind == 10) {
		int flags = int(v_texcoord1.z + 0.5);
		vec3 enabled = vec3(float((flags / 2) % 2 == 0), float((flags / 4) % 2 == 0), float((flags / 8) % 2 == 0));
		gl_FragColor = vec4(enabled * (float(index) / 256.0), 0.0);
		return;
	}

	if (kind != 6 && v_texcoord3.w < 0.5) {
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
		if (!((z - shape) < Fetch16(s_zbuf, Ring(area, u_ring[0])))) {
			discard;
		}
	}

	// The software blends floor each halved or quartered term. The source term is floored here; the
	// blend scales the target by alpha and takes off rgb, which is centred on the flooring loss of the
	// target term.
	if (kind == 8) {
		gl_FragColor = vec4_splat(1.0);
		return;
	}
	vec3 unit = vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0);
	if (kind == 1) {
		gl_FragColor = vec4(0.25 * unit, 0.5);
		return;
	}

	int band = 0;
	if (v_texcoord1.w >= 0.0) {
		// The alpha buffer value indexes on past the row's end the way the software lookup does.
		int entry = int(v_texcoord1.w + 0.5) * 256 + Fetch16(s_abuf, Ring(area, u_ring[1]));
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
