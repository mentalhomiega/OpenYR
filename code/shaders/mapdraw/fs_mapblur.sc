$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Blurs the shadow mask along one axis with a Gaussian for the soft shadows (code/gpumap.cpp). The mask texture may be larger
// than the map area; taps beyond the area read as zero.
//   v_texcoord0.zw  pixel position inside the map area
//   u_blur          xy: the step between taps, z: the radius in pixels (at most 16), w: the area's size along the step

#include "bgfx_shader.sh"

SAMPLER2D(s_mask, 0);
uniform vec4 u_blur;

void main()
{
	ivec2 area = ivec2(floor(v_texcoord0.zw));
	ivec2 step = ivec2(u_blur.xy);
	int radius = int(u_blur.z + 0.5);
	int limit = int(u_blur.w + 0.5);
	int along = step.x != 0 ? area.x : area.y;
	float sigma = max(u_blur.z * 0.5, 0.5);
	float sum = 0.0;
	float total = 0.0;
	for (int i = -radius; i <= radius; i++) {
		float weight = exp(-float(i * i) / (2.0 * sigma * sigma));
		if (along + i >= 0 && along + i < limit) {
			sum += weight * texelFetch(s_mask, area + step * i, 0).x;
		}
		total += weight;
	}
	gl_FragColor = vec4_splat(sum / total);
}
