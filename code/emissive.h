/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The emissive mask: one byte per pixel beside a map surface, saying how far a light source
// should be pushed above paper white when the composite is shown in HDR. It exists only while
// Enhanced highlights are active, so the classic path allocates and writes nothing.

#pragma once


// One byte per pixel, row by row. Zero is an ordinary pixel and 255 is the strongest boost.
struct EmissivePlane
{
	unsigned char * Pixels;
	int Width;
	int Height;
	int Pitch;
	// One byte per row, zero only where the whole row is zero; NULL treats every row as lit.
	unsigned char * Lit = nullptr;
};

// Whether a row may hold a value other than zero.
inline bool Emissive_Row_Lit(EmissivePlane const & plane, int row) { return(plane.Lit == nullptr || plane.Lit[row] != 0); }

// Marks rows as possibly holding values. A caller that writes Pixels directly marks the rows it wrote.
void Emissive_Mark_Rows(EmissivePlane & plane, int top, int count);


// Each row of the default table. Strength is the mask value 0-255 that the kind writes.
enum EmissiveKind
{
	EMISSIVE_ANIM_TRANSLUCENT,		// Anim with Translucent=yes.
	EMISSIVE_ANIM_SELF_LIT,			// Anim with UseNormalLight=yes, drawn at full brightness instead of the tile's light.
	EMISSIVE_ANIM_TRANSLUCENCY,		// Anim with Translucency=25, 50 or 75.
	EMISSIVE_ANIM_TIBERIUM,			// Anim with IsAnimatedTiberium=yes.
	EMISSIVE_LASER,					// Beam drawn for a weapon with IsLaser=yes.
	EMISSIVE_ELECTRIC_BOLT,			// Arc drawn for a weapon with IsElectricBolt=yes.
	EMISSIVE_ANIM_LIGHT_POST,		// Anim named NAPOST_A, NAPOST_AD or NAPOST_B, the glow of a light post.
	EMISSIVE_VOXEL_SPECULAR,		// Voxel specular highlight at its brightest row (31).
	EMISSIVE_KIND_COUNT
};


// The single table of defaults. Name is the INI spelling used in the manual; Strength is the
// default mask value; Applies says which draws the row covers.
struct EmissiveDefault
{
	char const * Name;
	unsigned char Strength;
	char const * Applies;
};

extern EmissiveDefault const EmissiveDefaults[EMISSIVE_KIND_COUNT];

// The strength for an anim from its flags. The strongest matching default wins, and 0 means
// the anim is not emissive.
unsigned char Emissive_Anim_Default(bool translucent, bool self_lit, int translucency, bool tiberium);

// Reads an Emissive= value: yes is 255, no is 0, and a number is clamped to 0-255. Anything
// else gives the fallback.
unsigned char Emissive_Parse_Strength(char const * text, unsigned char fallback);

// The strength for an anim by its name, for the anims that flags cannot identify. Zero is not emissive.
unsigned char Emissive_Anim_Name_Default(char const * name);

// Voxel highlight strength for a shade row. Rows 16 and below are not highlights; row 31 gives
// the table strength.
unsigned char Emissive_Voxel_Highlight(int shade_row);

// Gain on paper white for one pixel: 1 + mask/255 * (peak/paper - 1). Peak and paper white are
// in the same unit and peak is at least paper white.
float Emissive_Gain(unsigned char mask, float peak_over_paper);

// Output luminance for one linear channel value, clamped to the display peak:
// min(peak, paper * linear * gain(mask)).
float Emissive_Output_Nits(float linear, unsigned char mask, float paper_white, float peak);

// The mask for a draw that is drawn at the given opacity, from 0 for none to 255 for the whole picture.
// A translucent draw keeps only its share of the strength, so a faint anim does not glow at full strength.
unsigned char Emissive_Scale_By_Opacity(unsigned char strength, int opacity);

// Mask value that the draws in progress write. Zero outside an emissive draw.
extern unsigned char Emissive_Draw_Strength;

// Sets the draw strength for the scope and restores the previous value when it ends.
class EmissiveDrawScope
{
	public:
		explicit EmissiveDrawScope(unsigned char strength) : Saved(Emissive_Draw_Strength) { Emissive_Draw_Strength = strength; }
		~EmissiveDrawScope(void) { Emissive_Draw_Strength = Saved; }
		EmissiveDrawScope(EmissiveDrawScope const &) = delete;
		EmissiveDrawScope & operator=(EmissiveDrawScope const &) = delete;

	private:
		unsigned char Saved;
};

// Whether masks are being kept. Off means no plane is allocated and no pixel is written.
extern bool EmissiveActive;
inline bool Emissive_Is_Enabled(void) { return(EmissiveActive); }
void Emissive_Set_Enabled(bool enabled);

// Whether the presenter shows the masks in place of the picture, for checking the classification.
bool Emissive_Debug_View(void);
void Emissive_Set_Debug_View(bool debug);

// The plane bound to a surface, or NULL when the surface has none. A plane is bound at the surface's own size
// and is freed by Emissive_Forget when the surface is deleted, so the size always matches the surface.
EmissivePlane * Emissive_Plane_For(void const * surface);

// Binds a plane whose pixels the caller owns for the whole run, such as the voxel draw surface. Retain and
// Release_All never free it, and Emissive_Plane_For returns it only while masks are on.
void Emissive_Bind_Fixed(void const * surface, unsigned char * pixels, int width, int height);

// Binds a plane of the given size to a surface, reusing the existing one when the size matches.
EmissivePlane * Emissive_Bind(void const * surface, int width, int height);

// Frees every plane whose surface is not listed in keep. Called when the surfaces are replaced.
void Emissive_Retain(void const * const * keep, int count);

// Frees the plane bound to a surface, if it has one. Called as a surface is deleted, since the allocator may give
// a new surface the same address and it must not inherit the old plane.
void Emissive_Forget(void const * surface);

// Frees every plane. Called when masks are switched off or the surfaces are replaced.
void Emissive_Release_All(void);

// Writes value over a rectangle of the plane, clipped to the plane.
void Emissive_Fill_Rect(EmissivePlane & plane, int x, int y, int width, int height, unsigned char value);

// Copies a rectangle from one plane to another, clipped to both. Overlapping copies are safe.
void Emissive_Copy_Rect(EmissivePlane & dest, int dx, int dy, EmissivePlane const & source, int sx, int sy, int width, int height);

// Writes value at each pixel of the line from (x0, y0) to (x1, y1) that lies inside the clip rectangle.
void Emissive_Stamp_Segment(EmissivePlane & plane, int clipx, int clipy, int clipwidth, int clipheight, int x0, int y0, int x1, int y1, unsigned char value);
