/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the scenario readers to what map editors write. Each case is a small synthetic
// scenario fragment. Needs no game data.

#include "overlaylayer.h"
#include "previewtext.h"

#include <cstdio>
#include <string>

namespace {

int Failures = 0;


void Report(char const * what, bool ok)
{
	std::printf("%-72s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok) Failures++;
}


// A byte of 255 is an empty cell, and a byte past the rules' last overlay type is an overlay the
// game cannot place. Neither may index the type list.
void Test_Overlay_Bytes(void)
{
	bool ok = true;

	ok &= Overlay_Index_From_Byte(0, 200) == 0;
	ok &= Overlay_Index_From_Byte(199, 200) == 199;
	ok &= Overlay_Index_From_Byte(200, 200) == -1;
	ok &= Overlay_Index_From_Byte(254, 200) == -1;
	ok &= Overlay_Index_From_Byte(254, 255) == 254;
	ok &= Overlay_Index_From_Byte(255, 255) == -1;
	ok &= Overlay_Index_From_Byte(255, 256) == -1;
	ok &= Overlay_Index_From_Byte(0, 0) == -1;

	Report("an overlay byte past the defined types places nothing", ok);
}

// The preview may stand before [Map], as the game writes it, or anywhere after it, as editors
// write it. Either way only the two preview sections come out, in file order.
void Test_Preview_Sections(void)
{
	bool ok = true;

	std::string const westwood =
		"[Basic]\nName=Test\n[Preview]\nSize=0,0,10,5\n[PreviewPack]\n1=AAAA\n2=BBBB\n"
		"[Map]\nTheater=TEMPERATE\n[IsoMapPack5]\n1=CCCC\n";
	ok &= Preview_Sections_Text(westwood) == "[Preview]\nSize=0,0,10,5\n[PreviewPack]\n1=AAAA\n2=BBBB\n";

	std::string const editor =
		"[Basic]\nName=Test\n[Map]\nTheater=TEMPERATE\n[IsoMapPack5]\n1=CCCC\n"
		"[Preview]\nSize=0,0,10,5\n[Lighting]\nAmbient=1\n[PreviewPack]\n1=AAAA\n[OverlayPack]\n1=DDDD\n";
	ok &= Preview_Sections_Text(editor) == "[Preview]\nSize=0,0,10,5\n[PreviewPack]\n1=AAAA\n";

	// Carriage returns stay with their lines, a byte order mark is dropped, headers may be
	// indented, and the last line needs no newline.
	std::string const dos = "\xEF\xBB\xBF[Preview]\r\nSize=0,0,2,2\r\n  [Map]\r\nX=1\r\n\t[PreviewPack]\r\n1=AAAA";
	ok &= Preview_Sections_Text(dos) == "[Preview]\r\nSize=0,0,2,2\r\n\t[PreviewPack]\r\n1=AAAA\n";

	// A header with a trailing comment still names its section.
	ok &= Preview_Sections_Text("[Preview] ; size\nSize=0,0,1,1\n") == "[Preview] ; size\nSize=0,0,1,1\n";

	Report("the preview sections are found before or after [Map]", ok);
}


// Sections that only look like the preview ones, and files without any, give nothing.
void Test_No_Preview_Sections(void)
{
	bool ok = true;

	ok &= Preview_Sections_Text("").empty();
	ok &= Preview_Sections_Text("[Basic]\nName=Test\n[Map]\nSize=0,0,5,5\n").empty();
	ok &= Preview_Sections_Text("[Preview2]\nA=1\n[PreviewPackX]\n1=AAAA\n[preview]\nSize=0,0,1,1\n").empty();
	ok &= Preview_Sections_Text("Size=0,0,1,1\n; [Preview]\nPreview=1\n").empty();

	Report("lookalike sections and files without a preview give nothing", ok);
}

}


int main(void)
{
	Test_Overlay_Bytes();
	Test_Preview_Sections();
	Test_No_Preview_Sections();

	std::printf("custommaps: %s\n", Failures == 0 ? "all checks passed" : "FAILURES");
	return Failures == 0 ? 0 : 1;
}
