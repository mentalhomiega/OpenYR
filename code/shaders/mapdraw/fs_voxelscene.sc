$input v_texcoord1

// Writes a fragment of a direct voxel part into the map pass's scene target the way fs_mapdraw.sc writes the blit's
// pixel from the part's tile: index 0 is transparent and the blit's row depth test applies; a copy's index goes through
// the blit's lighting band and translator, then is written by a copy or, for the translucent kinds 2 to 4, blended
// (Translucent_Output); a shadow (kind 1) darkens, and a soft shadow (kind 8) writes 1 into the shadow mask. Kind 16 writes the voxel's highlight strength into the emissive mask where the CPU plane s_glowmask
// holds the mark, as the voxel image's tile holds it, so index 0 writes too.
//   v_texcoord1.x  the palette index vs_voxelscene.sc chose, y: its highlight strength
//   u_vblit[0..2]  the blit's values, as fs_mapdraw.sc describes v_texcoord1 to v_texcoord3, with u_vblit[0].y the
//                  blit's width
//   u_vblit[3]     xy: the blit's top left in area pixels, z: the scale the target is drawn at

#include "bgfx_shader.sh"
#include "mapcommon.sh"

SAMPLER2D(s_glowmask, 6);
uniform vec4 u_vblit[4];

// GPU_VOXEL_MASK_MARK in code/gpumapvoxel.h.
#define VOXEL_MARK 1

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
		gl_FragColor = Translucent_Output(kind, colour);
		return;
	}
	gl_FragColor = vec4(Rgb565(colour), 1.0);
}
