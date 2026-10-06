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
#include <cstdlib>
#include <cstring>
#include <string>
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


UIImageResult UI_Load_Image(char const * name, std::vector<unsigned char> & rgba, int & width, int & height, bool premultiply)
{
	rgba.clear();
	width = 0;
	height = 0;

	if (name == NULL) {
		return(UI_IMAGE_MISSING);
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
