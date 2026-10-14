$input v_color0, v_texcoord0

// The composite pass draws the finished frame to the window. Its steps run in this order:
//   1. decode the sRGB frame to linear light,
//   2. apply the 3x3 colour matrix (rows of u_colourMatrix),
//   3. multiply by the brightness and raise to the gamma (u_colourTone),
//   4. HDR only: scale by the paper white, apply Rec.709 to Rec.2020 and limit to the peak,
//      then encode with ST.2084,
//   5. SDR only: encode with the sRGB curve.
// Steps 2 and 3 run only when u_compositeLevels.w is 1, which the backend sets when the
// adjustment is not the identity. Edit this file and regenerate fs_composite.bin.h as
// docs/HDR.md describes.

#include "bgfx_shader.sh"

SAMPLER2D(s_tex, 0);

// x is the paper white and y the peak, both in units of 10000 nits; z is 1 for HDR10 output and 0
// for sRGB; w is 1 when the colour adjustment applies.
uniform vec4 u_compositeLevels;

// Rows of the matrix; x of u_colourTone is the brightness and y the gamma exponent.
uniform vec4 u_colourMatrix[3];
uniform vec4 u_colourTone;

vec3 Srgb_To_Linear(vec3 c)
{
	vec3 low = c / 12.92;
	vec3 high = pow((c + vec3(0.055, 0.055, 0.055)) / vec3(1.055, 1.055, 1.055), vec3(2.4, 2.4, 2.4));
	return mix(high, low, step(c, vec3(0.04045, 0.04045, 0.04045)));
}

vec3 Linear_To_Srgb(vec3 c)
{
	vec3 x = clamp(c, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	vec3 low = x * 12.92;
	vec3 high = 1.055 * pow(x, vec3(0.4166666667, 0.4166666667, 0.4166666667)) - vec3(0.055, 0.055, 0.055);
	return mix(high, low, step(x, vec3(0.0031308, 0.0031308, 0.0031308)));
}

// ST.2084 inverse EOTF. The input is luminance divided by 10000 nits.
vec3 Pq_Encode(vec3 normal)
{
	vec3 y = clamp(normal, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	vec3 p = pow(y, vec3(0.1593017578125, 0.1593017578125, 0.1593017578125));
	vec3 num = vec3(0.8359375, 0.8359375, 0.8359375) + vec3(18.8515625, 18.8515625, 18.8515625) * p;
	vec3 den = vec3(1.0, 1.0, 1.0) + vec3(18.6875, 18.6875, 18.6875) * p;
	return pow(num / den, vec3(78.84375, 78.84375, 78.84375));
}

void main()
{
	vec4 texel = texture2D(s_tex, v_texcoord0) * v_color0;
	vec3 light = Srgb_To_Linear(texel.rgb);

	if (u_compositeLevels.w > 0.5) {
		vec3 mixed;
		mixed.r = dot(u_colourMatrix[0].xyz, light);
		mixed.g = dot(u_colourMatrix[1].xyz, light);
		mixed.b = dot(u_colourMatrix[2].xyz, light);
		vec3 bright = max(mixed * u_colourTone.x, vec3(0.0, 0.0, 0.0));
		light = pow(bright, vec3(u_colourTone.y, u_colourTone.y, u_colourTone.y));
	}

	if (u_compositeLevels.z > 0.5) {
		// The paper white scale is the last step before the Rec.2020 conversion, so SDR white
		// lands at the paper white.
		vec3 nits = light * u_compositeLevels.x;
		vec3 rec2020;
		rec2020.r = dot(vec3(0.627404, 0.329282, 0.043313), nits);
		rec2020.g = dot(vec3(0.069097, 0.919540, 0.011362), nits);
		rec2020.b = dot(vec3(0.016391, 0.088013, 0.895595), nits);
		rec2020 = min(max(rec2020, vec3(0.0, 0.0, 0.0)), vec3(u_compositeLevels.y, u_compositeLevels.y, u_compositeLevels.y));
		gl_FragColor = vec4(Pq_Encode(rec2020), 1.0);
	} else {
		gl_FragColor = vec4(Linear_To_Srgb(light), 1.0);
	}
}
