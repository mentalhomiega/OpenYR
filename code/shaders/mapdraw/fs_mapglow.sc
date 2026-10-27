$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Builds the glow for bloom (code/gpumap.cpp). Mode 0 reduces each 4 by 4 block of the drawn map,
// weighted by its emissive mask, into a picture a quarter of the size; mode 1 blurs that picture
// along one axis with a Gaussian.
//   v_texcoord0.zw  pixel position inside the quarter-size picture
//   u_glow          mode 0: xy the map area size; mode 1: xy the step between taps, z the radius in
//                   pixels (at most 16); w: the mode

#include "bgfx_shader.sh"

SAMPLER2D(s_scene, 0);
SAMPLER2D(s_glowmask, 1);
uniform vec4 u_glow;

void main()
{
	ivec2 at = ivec2(floor(v_texcoord0.zw));
	if (u_glow.w < 0.5) {
		ivec2 last = ivec2(u_glow.xy) - ivec2(1, 1);
		vec3 sum = vec3_splat(0.0);
		vec3 peak = vec3_splat(0.0);
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				ivec2 p = min(at * 4 + ivec2(x, y), last);
				// Only bright pixels glow, so smoke and other dim translucent effects in the mask do not.
				vec3 colour = texelFetch(s_scene, p, 0).rgb;
				float bright = smoothstep(0.7, 0.95, max(colour.r, max(colour.g, colour.b)));
				vec3 lit = colour * texelFetch(s_glowmask, p, 0).x * bright;
				sum += lit;
				peak = max(peak, lit);
			}
		}
		// Half of the block's brightest pixel keeps a beam one pixel wide glowing.
		gl_FragColor = vec4(sum / 32.0 + peak * 0.5, 1.0);
		return;
	}
	ivec2 step = ivec2(u_glow.xy);
	int radius = int(u_glow.z + 0.5);
	float sigma = max(u_glow.z * 0.5, 0.5);
	vec3 sum = vec3_splat(0.0);
	float total = 0.0;
	for (int i = -16; i <= 16; i++) {
		if (i >= -radius && i <= radius) {
			float weight = exp(-float(i * i) / (2.0 * sigma * sigma));
			sum += weight * texelFetch(s_scene, at + step * i, 0).rgb;
			total += weight;
		}
	}
	gl_FragColor = vec4(sum / total, 1.0);
}
