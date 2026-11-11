$input v_texcoord1

// Writes a fragment of a voxel part into the map pass's scene target the way fs_mapdraw.sc writes a blit's pixel:
// index 0 is transparent and the blit's row depth test applies; a copy's index goes through the blit's lighting band
// and translator, then is written by a copy or, for the translucent kinds 2 to 4, blended (Translucent_Output); a
// shadow (kind 1) darkens, and a soft shadow (kind 8) writes 1 into the shadow mask. Kind 16 writes the voxel's
// highlight strength into the emissive mask where the CPU plane s_glowmask holds the mark, for index 0 too.
//   v_texcoord1.x  the palette index vs_voxelscene.sc chose, y: its highlight strength
//   u_vblit[0..2]  the blit's values, as fs_mapdraw.sc describes v_texcoord1 to v_texcoord3, with u_vblit[0].y the
//                  blit's width
//   u_vblit[3]     xy: the blit's top left in area pixels, z: the scale the target is drawn at, w: for a cloaked object
//                  drawn with the ripple, its predator offset plus 1, else 0. Such a blit blends in the shader: it reads
//                  the target from s_snapshot, a copy taken before the blit, and the translucent kinds add their source
//                  term to the colour under a pixel shifted by a travelling wave.
//   u_world        z: the time in seconds, which moves the ripple

#include "bgfx_shader.sh"
#include "mapcommon.sh"

SAMPLER2D(s_glowmask, 6);
SAMPLER2D(s_snapshot, 8);
uniform vec4 u_vblit[4];
uniform vec4 u_world;

// GPU_VOXEL_MASK_MARK in code/gpumapvoxel.h.
#define VOXEL_MARK 1

// The translucent output of a cloaked fragment drawn with the ripple: the pixel under it is the one a wave of the
// object's phase and the time moves up to two map pixels from, so the target is shown through the object, bent.
vec4 Cloak_Ripple(vec4 translucent, vec2 fragment, int scale)
{
	float phase = u_vblit[3].w - 1.0;
	vec2 wave = fragment / float(scale) * 0.45 + vec2_splat(u_world.z * 7.0 + phase * 0.35);
	ivec2 shift = ivec2(vec2(sin(wave.y), cos(wave.x)) * 2.99) * scale;
	ivec2 size = ivec2(textureSize(s_snapshot, 0));
	ivec2 source = clamp(ivec2(floor(fragment)) + shift, ivec2(0, 0), size - ivec2(1, 1));
	vec3 under = texelFetch(s_snapshot, source, 0).xyz;
	return vec4(translucent.xyz + under * translucent.w, 1.0);
}

void main()
{
	int index = int(v_texcoord1.x + 0.5);
	int kind = int(u_vblit[2].z + 0.5);
	if (index == 0 && kind != 16) {
		discard;
	}
	int scale = int(u_vblit[3].z + 0.5);
	ivec2 area = ivec2(floor(gl_FragCoord.xy / float(scale)));
	ivec2 local = area - ivec2(floor(u_vblit[3].xy + vec2_splat(0.5)));
	if (u_vblit[2].w < 0.5 && !Row_Depth_Passes(local, area, int(u_vblit[0].y + 0.5), u_vblit[1], u_vblit[2])) {
		discard;
	}
	if (kind == 1) {
		// A shadow darkens the target in the blend, as fs_mapdraw.sc's darken kind does.
		gl_FragColor = vec4(0.25 / 31.0, 0.25 / 63.0, 0.25 / 31.0, 0.5);
		return;
	}
	if (kind == 8) {
		gl_FragColor = vec4_splat(1.0);
		return;
	}
	if (kind == 16) {
		if (int(texelFetch(s_glowmask, area, 0).x * 255.0 + 0.5) != VOXEL_MARK) {
			discard;
		}
		gl_FragColor = vec4_splat(v_texcoord1.y / 255.0);
		return;
	}
	int colour = Translated_Colour(index, area, u_vblit[0]);
	if (kind >= 2 && kind <= 4) {
		vec4 translucent = Translucent_Output(kind, colour);
		gl_FragColor = u_vblit[3].w > 0.5 ? Cloak_Ripple(translucent, gl_FragCoord.xy, scale) : translucent;
		return;
	}
	gl_FragColor = vec4(Rgb565(colour), 1.0);
}
