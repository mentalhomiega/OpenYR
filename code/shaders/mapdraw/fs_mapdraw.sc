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
// 9 the base picture darkened by the shadow mask in s_shadow, 10 a spot light's brightening factor
// (v_texcoord1.z holds the channels it leaves alone), 11 depth-shaded line pixels whose index is the alpha
// buffer value under each (v_texcoord1.z holds the 565 colour, w the fade; the target's weight goes out in alpha),
// 12 additive line pixels whose index is the alpha buffer value (v_texcoord1.z holds 0xRRGGBB), 13 antialiased line
// pixels whose index is the colour weight plus 1 (v_texcoord1.z holds 0xRRGGBB; the target's weight goes out in alpha),
// 14 sonic wave pixels whose index is the amplitude plus 1, shifted and tinted from s_snapshot (v_texcoord1.z holds
// the facing plus 8 for a magnetron beam or 16 for a custom colour, w the tint at amplitude 0).
// u_scale: x the scale the target is drawn at, 1 to 4 (a blit's coordinates stay in map pixels, so the target shows
// each of them as scale by scale pixels).

#include "bgfx_shader.sh"
#include "mapcommon.sh"

SAMPLER2D(s_index, 0);
SAMPLER2D(s_base, 6);
// The base store's ring addressing, as a u_ring entry: the store is the area's picture or the tile layer's memory.
uniform vec4 u_basering;
SAMPLER2D(s_shadow, 7);
SAMPLER2D(s_snapshot, 8);
uniform vec4 u_scale;
// A deferred light fade (code/layerfade.h): x is 1 while s_fade holds the scales of the base picture, yz the size in
// map pixels that s_fade covers, w the scale a full texel stands for.
SAMPLER2D(s_fade, 10);
uniform vec4 u_fade;

void main()
{
	int kind = int(v_texcoord3.z + 0.5);
	ivec2 local = ivec2(floor(v_texcoord0.xy));
	ivec2 area = ivec2(floor(v_texcoord0.zw));

	if (kind == 5 || kind == 9) {
		vec3 base = Rgb565(Fetch16(s_base, Ring(area, u_basering)));
		if (u_fade.x > 0.0) {
			base = min(base * (texture2D(s_fade, (vec2(area) + vec2(0.5, 0.5)) / u_fade.yz).x * u_fade.w), vec3_splat(1.0));
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

	int scale = int(u_scale.x + 0.5);
	ivec2 sub = min(ivec2(floor(fract(v_texcoord0.zw) * float(scale))), ivec2(scale - 1, scale - 1));
	int width = int(v_texcoord1.y + 0.5);
	// Offsets are whole numbers, and adding 0.5 would round them from 2^23 up, where a float spaces by one.
	int offset = int(v_texcoord1.x);
	int index = Fetch8(s_index, offset + local.y * width + local.x);
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
		ivec2 size = ivec2(textureSize(s_snapshot, 0)) / scale;
		if (source.x < 0 || source.y < 0 || source.x >= size.x || source.y >= size.y) {
			source = area;
		}
		vec3 under = texelFetch(s_snapshot, source * scale + sub, 0).xyz;
		ivec3 c8 = ivec3(floor(under * vec3(31.0, 63.0, 31.0) + 0.5)) * ivec3(8, 4, 8);
		int mult = int(v_texcoord1.w + 0.5) + amplitude * 8;
		if (wave >= 16) {
			// WaveClass::Custom_Tint: v_texcoord2.xyz the channel scales, v_texcoord2.w and v_texcoord3.xy the constants.
			ivec3 tintscale = ivec3(floor(v_texcoord2.xyz + 0.5));
			ivec3 tintadd = ivec3(floor(vec3(v_texcoord2.w, v_texcoord3.x, v_texcoord3.y) + 0.5));
			c8 = clamp(c8 + ((c8 * mult * tintscale) >> 16) + ((tintadd * mult) >> 8), ivec3(0, 0, 0), ivec3(255, 255, 255));
		} else if (wave >= 8) {
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

	if (kind != 6 && v_texcoord3.w < 0.5 && !Row_Depth_Passes(local, area, width, v_texcoord2, v_texcoord3)) {
		discard;
	}

	if (kind == 8) {
		gl_FragColor = vec4_splat(1.0);
		return;
	}
	if (kind == 1) {
		gl_FragColor = vec4(0.25 / 31.0, 0.25 / 63.0, 0.25 / 31.0, 0.5);
		return;
	}

	int colour = Translated_Colour(index, area, v_texcoord1);
	if (kind == 0 || kind == 6) {
		gl_FragColor = vec4(Rgb565(colour), 1.0);
		return;
	}
	gl_FragColor = Translucent_Output(kind, colour);
}
