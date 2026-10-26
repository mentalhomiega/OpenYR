/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/rml/rmltexture.h"

#include "ccfile.h"
#include "dbgprint.h"
#include "rawfile.h"
#include "ui/rml/rmlimage.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_NO_STDIO
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include <stb_image.h>


static bool Has_Extension(char const * name, char const * extension)
{
	size_t length = strlen(name);
	size_t extensionlength = strlen(extension);

	return(length >= extensionlength && _stricmp(name + length - extensionlength, extension) == 0);
}


static bool Read_Whole_File(char const * name, std::vector<unsigned char> & bytes)
{
	bytes.clear();

	CCFileClass file(name);
	if (!file.Is_Available()) {
		return(false);
	}

	int size = file.Size();
	if (size <= 0) {
		return(false);
	}

	bytes.resize((size_t)size);
	if (!file.Open(FileClass::READ) || file.Read(bytes.data(), size) != size) {
		bytes.clear();
		return(false);
	}
	file.Close();

	return(true);
}


bool UI_Read_File(char const * path, std::vector<unsigned char> & bytes)
{
	bytes.clear();

	if (path == NULL) {
		return(false);
	}

	RawFileClass file(path);
	if (!file.Is_Available()) {
		return(false);
	}

	int size = file.Size();
	if (size <= 0) {
		return(false);
	}

	bytes.resize((size_t)size);
	if (!file.Open(FileClass::READ) || file.Read(bytes.data(), size) != size) {
		bytes.clear();
		return(false);
	}
	file.Close();

	return(true);
}


bool UI_Load_Indexed_Image(char const * name, UIImageIndexed & image)
{
	image = UIImageIndexed();

	if (name == NULL || !Has_Extension(name, ".pcx")) {
		return(false);
	}

	std::vector<unsigned char> encoded;
	if (!Read_Whole_File(name, encoded)) {
		return(false);
	}

	return(UI_Decode_PCX(std::span<std::uint8_t const>(encoded.data(), encoded.size()), image));
}


namespace
{

// The logo sits in the top left of the stock 800 by 600 title picture, GLSLMD.SHP.
constexpr int LOGO_ART_WIDTH = 800;
constexpr int LOGO_ART_HEIGHT = 600;
constexpr int LOGO_X = 12;
constexpr int LOGO_Y = 10;
constexpr int LOGO_WIDTH = 392;
constexpr int LOGO_HEIGHT = 118;

// Dark pixels and the teal and green of the lightning and haze count as background; the logo's
// silver, gold and red do not.
bool Is_Logo_Background(unsigned char const * pixel)
{
	int const r = pixel[0];
	int const g = pixel[1];
	int const b = pixel[2];
	if (pixel[3] == 0 || std::max(r, std::max(g, b)) < 70) {
		return(true);
	}
	return((b > r + 25 && g > r + 10) || (g > r + 25 && g > b));
}

// Dim green-grey haze lies beside the right wing tip, the TM and the right end of the lower bar.
bool Is_Logo_Haze(unsigned char const * pixel)
{
	int const r = pixel[0];
	int const g = pixel[1];
	int const b = pixel[2];
	return(std::max(r, std::max(g, b)) < 112 && g > r + 8 && g >= b);
}


void Spread_Mask(std::vector<unsigned char> & mask, int radius, bool grow)
{
	std::vector<unsigned char> source = mask;
	for (int y = 0; y < LOGO_HEIGHT; y++) {
		for (int x = 0; x < LOGO_WIDTH; x++) {
			unsigned char value = grow ? 0 : 255;
			for (int dy = -radius; dy <= radius; dy++) {
				for (int dx = -radius; dx <= radius; dx++) {
					int const u = std::clamp(x + dx, 0, LOGO_WIDTH - 1);
					int const v = std::clamp(y + dy, 0, LOGO_HEIGHT - 1);
					unsigned char const other = source[(size_t)v * LOGO_WIDTH + u];
					value = grow ? std::max(value, other) : std::min(value, other);
				}
			}
			mask[(size_t)y * LOGO_WIDTH + x] = value;
		}
	}
}


/// <summary>
/// Cuts the game logo out of the installed title picture, with the painted background made
/// transparent. It stands in for a drawn logo, so nothing from the picture is shipped.
/// </summary>
UIImageResult Make_Title_Logo(std::vector<unsigned char> & rgba, int & width, int & height, bool premultiply)
{
	std::vector<unsigned char> art;
	int art_width = 0;
	int art_height = 0;
	UIImageResult const result = UI_Load_Image("GLSLMD.SHP@GLSMD.PAL", art, art_width, art_height, false);
	if (result != UI_IMAGE_LOADED) {
		return(result);
	}
	// A replaced title picture has no logo at the stock place.
	if (art_width != LOGO_ART_WIDTH || art_height != LOGO_ART_HEIGHT) {
		return(UI_IMAGE_MISSING);
	}

	auto pixel = [&art](int x, int y) {
		return(&art[((size_t)(LOGO_Y + y) * LOGO_ART_WIDTH + (LOGO_X + x)) * 4]);
	};

	// The background is what the crop's edge reaches through background pixels.
	std::vector<unsigned char> background((size_t)LOGO_WIDTH * LOGO_HEIGHT, 0);
	std::vector<std::pair<int, int>> stack;
	for (int x = 0; x < LOGO_WIDTH; x++) {
		stack.emplace_back(x, 0);
		stack.emplace_back(x, LOGO_HEIGHT - 1);
	}
	for (int y = 0; y < LOGO_HEIGHT; y++) {
		stack.emplace_back(0, y);
		stack.emplace_back(LOGO_WIDTH - 1, y);
	}
	while (!stack.empty()) {
		auto const [x, y] = stack.back();
		stack.pop_back();
		if (x < 0 || y < 0 || x >= LOGO_WIDTH || y >= LOGO_HEIGHT) {
			continue;
		}
		unsigned char & seen = background[(size_t)y * LOGO_WIDTH + x];
		if (seen != 0 || !Is_Logo_Background(pixel(x, y))) {
			continue;
		}
		seen = 1;
		stack.emplace_back(x + 1, y);
		stack.emplace_back(x - 1, y);
		stack.emplace_back(x, y + 1);
		stack.emplace_back(x, y - 1);
	}

	// The rest splits into pieces. Specks, lightning at the sides, and the soldiers' heads under
	// the wings (pieces that start in the lowest rows) are dropped.
	std::vector<unsigned char> mask((size_t)LOGO_WIDTH * LOGO_HEIGHT, 0);
	std::vector<int> label((size_t)LOGO_WIDTH * LOGO_HEIGHT, 0);
	int pieces = 0;
	for (int y = 0; y < LOGO_HEIGHT; y++) {
		for (int x = 0; x < LOGO_WIDTH; x++) {
			size_t const at = (size_t)y * LOGO_WIDTH + x;
			if (background[at] != 0 || label[at] != 0) {
				continue;
			}
			pieces++;
			std::vector<size_t> piece;
			std::vector<size_t> open{at};
			label[at] = pieces;
			int top = y;
			int left = x;
			int right = x;
			while (!open.empty()) {
				size_t const here = open.back();
				open.pop_back();
				piece.push_back(here);
				int const px = (int)(here % LOGO_WIDTH);
				int const py = (int)(here / LOGO_WIDTH);
				top = std::min(top, py);
				left = std::min(left, px);
				right = std::max(right, px);
				int const steps[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
				for (auto const & step : steps) {
					int const u = px + step[0];
					int const v = py + step[1];
					if (u < 0 || v < 0 || u >= LOGO_WIDTH || v >= LOGO_HEIGHT) {
						continue;
					}
					size_t const next = (size_t)v * LOGO_WIDTH + u;
					if (background[next] == 0 && label[next] == 0) {
						label[next] = pieces;
						open.push_back(next);
					}
				}
			}
			if (piece.size() >= 10 && top < 80 && left > 1 && right < LOGO_WIDTH - 2) {
				for (size_t const kept : piece) {
					int const kx = (int)(kept % LOGO_WIDTH);
					int const ky = (int)(kept / LOGO_WIDTH);
					// Haze and lightning join the logo's right end; the final E's outline ends at column 371.
					if ((kx < 373 || ky < 56) && (kx < 340 || !Is_Logo_Haze(pixel(kx, ky)))) {
						mask[kept] = 255;
					}
				}
			}
		}
	}

	// Close small holes, take in the dark outline, then soften the edge.
	Spread_Mask(mask, 2, true);
	Spread_Mask(mask, 2, false);
	Spread_Mask(mask, 1, true);
	std::vector<unsigned char> soft = mask;
	for (int y = 0; y < LOGO_HEIGHT; y++) {
		for (int x = 0; x < LOGO_WIDTH; x++) {
			int sum = 0;
			int weight = 0;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					int const u = std::clamp(x + dx, 0, LOGO_WIDTH - 1);
					int const v = std::clamp(y + dy, 0, LOGO_HEIGHT - 1);
					int const w = (dx == 0 ? 2 : 1) * (dy == 0 ? 2 : 1);
					sum += mask[(size_t)v * LOGO_WIDTH + u] * w;
					weight += w;
				}
			}
			soft[(size_t)y * LOGO_WIDTH + x] = (unsigned char)(sum / weight);
		}
	}

	rgba.resize((size_t)LOGO_WIDTH * LOGO_HEIGHT * 4);
	for (int y = 0; y < LOGO_HEIGHT; y++) {
		for (int x = 0; x < LOGO_WIDTH; x++) {
			unsigned char const * from = pixel(x, y);
			unsigned char * to = &rgba[((size_t)y * LOGO_WIDTH + x) * 4];
			unsigned int const alpha = soft[(size_t)y * LOGO_WIDTH + x];
			for (int channel = 0; channel < 3; channel++) {
				to[channel] = premultiply ? (unsigned char)(from[channel] * alpha / 255) : from[channel];
			}
			to[3] = (unsigned char)alpha;
		}
	}

	width = LOGO_WIDTH;
	height = LOGO_HEIGHT;
	return(UI_IMAGE_LOADED);
}

}


UIImageResult UI_Load_Image(char const * name, std::vector<unsigned char> & rgba, int & width, int & height, bool premultiply)
{
	rgba.clear();
	width = 0;
	height = 0;

	if (name == NULL) {
		return(UI_IMAGE_MISSING);
	}

	if (Has_Extension(name, "titlelogo")) {
		return(Make_Title_Logo(rgba, width, height, premultiply));
	}

	// A shape names its palette, and optionally a frame: NAME.SHP@PALETTE.PAL@frame.
	char const * at = std::strchr(name, '@');
	if (at != NULL) {
		std::string const shape(name, at);
		std::string palette = at + 1;
		int frame = 0;
		std::string::size_type const second = palette.find('@');
		if (second != std::string::npos) {
			frame = std::atoi(palette.c_str() + second + 1);
			palette.erase(second);
		}

		std::vector<unsigned char> encoded;
		std::vector<unsigned char> pal;
		if (!Has_Extension(shape.c_str(), ".shp") || !Read_Whole_File(shape.c_str(), encoded) || !Read_Whole_File(palette.c_str(), pal)) {
			return(UI_IMAGE_MISSING);
		}

		UIImageIndexed image;
		if (!UI_Decode_SHP(std::span<std::uint8_t const>(encoded.data(), encoded.size()), frame, image)
			|| !UI_Apply_PAL(std::span<std::uint8_t const>(pal.data(), pal.size()), image)
			|| !UI_Indexed_To_RGBA(image, rgba)) {
			DebugString("UI: %s did not decode as a shape frame with its palette\n", name);
			rgba.clear();
			return(UI_IMAGE_UNREADABLE);
		}

		width = image.Width;
		height = image.Height;
		return(UI_IMAGE_LOADED);
	}

	bool pcx = Has_Extension(name, ".pcx");
	if (!pcx && !Has_Extension(name, ".png") && !Has_Extension(name, ".tga")) {
		return(UI_IMAGE_UNREADABLE);
	}

	std::vector<unsigned char> encoded;
	if (!Read_Whole_File(name, encoded)) {
		return(UI_IMAGE_MISSING);
	}

	if (pcx) {
		UIImageIndexed image;
		if (!UI_Decode_PCX(std::span<std::uint8_t const>(encoded.data(), encoded.size()), image)
			|| !UI_Indexed_To_RGBA(image, rgba)) {
			DebugString("UI: %s is not an 8-bit run-length PCX\n", name);
			rgba.clear();
			return(UI_IMAGE_UNREADABLE);
		}

		width = image.Width;
		height = image.Height;
		return(UI_IMAGE_LOADED);
	}

	int channels = 0;
	unsigned char * pixels = stbi_load_from_memory(encoded.data(), (int)encoded.size(), &width, &height, &channels, 4);
	if (pixels == NULL) {
		DebugString("UI: %s did not decode: %s\n", name, stbi_failure_reason());
		width = 0;
		height = 0;
		return(UI_IMAGE_UNREADABLE);
	}

	rgba.assign(pixels, pixels + (size_t)width * (size_t)height * 4);
	stbi_image_free(pixels);

	for (size_t index = 0; premultiply && index < rgba.size(); index += 4) {
		unsigned int alpha = rgba[index + 3];
		if (alpha != 255) {
			rgba[index] = (unsigned char)(rgba[index] * alpha / 255);
			rgba[index + 1] = (unsigned char)(rgba[index + 1] * alpha / 255);
			rgba[index + 2] = (unsigned char)(rgba[index + 2] * alpha / 255);
		}
	}

	return(UI_IMAGE_LOADED);
}
