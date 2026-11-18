$input v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3

// Builds the emissive plane of a pass whose plane writes were listed (code/gpudraw.h, GpuMaskOp): the plane as the pass
// found it, then each write in order, as the CPU would have made them.
//   v_texcoord0.xy  pixel position inside the quad, v_texcoord0.zw inside the map area
//   v_texcoord1     x: offset of a shape's colour indices in s_index or of a write's values in s_maskvalues,
//                   y: their row length, z: the value written, w: the shape's width, the row length of its depth shape
//   v_texcoord2     a shape's row depths, as fs_mapdraw.sc describes them
//   v_texcoord3     x: depth step, y: offset of the depth shape in s_zshape (-1 for none), z: kind, w: 1 to skip the
//                   depth test
// Kind: 0 the plane as the pass found it, from s_glowmask; 1 the value at each pixel of a shape that passes the depth
// test; 2 the value; 3 the values in s_maskvalues.

#include "bgfx_shader.sh"
#include "mapcommon.sh"

SAMPLER2D(s_index, 0);
SAMPLER2D(s_glowmask, 7);
SAMPLER2D(s_maskvalues, 8);

void main()
{
	int kind = int(v_texcoord3.z + 0.5);
	ivec2 local = ivec2(floor(v_texcoord0.xy));
	ivec2 area = ivec2(floor(v_texcoord0.zw));
	if (kind == 0) {
		gl_FragColor = vec4_splat(float(int(texelFetch(s_glowmask, area, 0).x * 255.0 + 0.5)) / 255.0);
		return;
	}
	if (kind == 3) {
		int value = Fetch8(s_maskvalues, int(v_texcoord1.x) + local.y * int(v_texcoord1.y + 0.5) + local.x);
		gl_FragColor = vec4_splat(float(value) / 255.0);
		return;
	}
	if (kind == 1) {
		if (Fetch8(s_index, int(v_texcoord1.x) + local.y * int(v_texcoord1.y + 0.5) + local.x) == 0) {
			discard;
		}
		if (v_texcoord3.w < 0.5 && !Row_Depth_Passes(local, area, int(v_texcoord1.w + 0.5), v_texcoord2, v_texcoord3)) {
			discard;
		}
	}
	gl_FragColor = vec4_splat(v_texcoord1.z / 255.0);
}
