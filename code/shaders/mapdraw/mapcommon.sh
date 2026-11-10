// What every fragment shader that draws a recorded blit of the map view's object pass shares (code/gpumap.cpp):
// the stores a blit's depth test and colour read, and the steps that read them. A blit's values are the ones
// fs_mapdraw.sc describes for v_texcoord1 to v_texcoord3.
//   u_ring  the depth and alpha buffers' ring addressing: (start, width, size) each

SAMPLER2D(s_lut, 1);
SAMPLER2D(s_alut, 2);
SAMPLER2D(s_zbuf, 3);
SAMPLER2D(s_abuf, 4);
SAMPLER2D(s_zshape, 5);
uniform vec4 u_ring[2];

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

// True where the blit's pixel at local, over area pixel area, is in front of the depth buffer: its row's depth minus
// its depth shape is below the buffer value. rows and shape are the blit's v_texcoord2 and v_texcoord3, width its pitch.
bool Row_Depth_Passes(ivec2 local, ivec2 area, int width, vec4 rows, vec4 shape)
{
	int z = int(rows.x);
	int denominator = int(rows.w + 0.5);
	if (denominator != 0) {
		z += int(shape.x) * ((int(rows.y + 0.5) + local.y * denominator) / int(rows.z + 0.5));
	}
	int offset = 0;
	if (shape.y >= 0.0) {
		offset = Fetch8(s_zshape, int(shape.y + 0.5) + local.y * width + local.x);
		if (offset > 127) {
			offset -= 256;
		}
	}
	return (z - offset) < Fetch16(s_zbuf, Ring(area, u_ring[0]));
}

// The output of a translucent kind (2 75% source, 3 half, 4 25% source) for 565 colour, which the blend
// (Blend_State_For in code/gpumap.cpp) finishes: the source term in rgb and the target's weight in alpha. The software
// blends floor each halved or quartered term. The source term is floored here; the blend scales the target by alpha and
// takes off rgb, which is centred on the flooring loss of the target term.
vec4 Translucent_Output(int kind, int colour)
{
	ivec3 channel = ivec3((colour / 2048) % 32, (colour / 32) % 64, colour % 32);
	vec3 unit = vec3(1.0 / 31.0, 1.0 / 63.0, 1.0 / 31.0);
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
	return vec4(max(source - loss, vec3_splat(0.0)) * unit, keep);
}

// The 565 colour the blit writes for palette index at area pixel area: the index plus its lighting band, through the
// blit's translator. source is the blit's v_texcoord1.
int Translated_Colour(int index, ivec2 area, vec4 source)
{
	int band = 0;
	if (source.w >= 0.0) {
		// The alpha buffer value indexes on past the row's end the way the software lookup does.
		int entry = int(source.w + 0.5) * 256 + Fetch16(s_abuf, Ring(area, u_ring[1]));
		band = Fetch16(s_alut, ivec2(entry % 256, entry / 256));
	}
	int value = index + band;
	return Fetch16(s_lut, ivec2(value % 256, int(source.z + 0.5) + value / 256));
}
