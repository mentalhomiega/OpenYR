/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/Library/PCX.cpp                              $*
 *                                                                                             *
 *                      $Author:: Greg_h                                                      $*
 *                                                                                             *
 *                     $Modtime:: 9/28/98 12:06p                                              $*
 *                                                                                             *
 *                    $Revision:: 2                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "pcx.h"

#include "dbgprint.h"
#include "dsurface.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>

/// <summary>
/// Reads how large a picture is without decoding it.
/// </summary>
/// <returns>bool; Was a picture found to measure?</returns>
bool Read_PCX_Size(FileClass & file, int & width, int & height)
{
	PCX_HEADER header;

	if (!file.Is_Available()) {
		return(false);
	}

	if (!file.Open(FileClass::READ)) {
		return(false);
	}

	int read = file.Read(&header, sizeof(header));
	file.Close();

	if (read != sizeof(header) || header.id != 10) {
		return(false);
	}

	width = header.width - header.x + 1;
	height = header.height - header.y + 1;

	return(width > 0 && height > 0);
}


namespace {

// The largest side of a picture the reader builds, in pixels.
constexpr int PCX_MAX_SIDE = 4096;


/// <summary>
/// Hands out the bytes of a file one at a time. Decoding stops when the file runs out.
/// </summary>
class PCXByteReader
{
	public:
		explicit PCXByteReader(FileClass & file) : File(file) {}

		bool Get(unsigned char & value)
		{
			if (At == Have) {
				Have = File.Read(Pool, sizeof(Pool));
				At = 0;
				if (Have <= 0) {
					Have = 0;
					return(false);
				}
			}
			value = (unsigned char)Pool[At++];
			return(true);
		}

	private:
		FileClass & File;
		char Pool[2048];
		int Have = 0;
		int At = 0;
};


/// <summary>
/// Decodes run-length data into a line of the picture. A byte with its top two bits set is a
/// run: the low six bits count the repeats of the byte that follows. Any other byte stands for
/// itself. A run that would pass the end of the line is cut there.
/// </summary>
/// <param name="out">Receives the first `store` bytes of the line.</param>
/// <param name="length">The decoded size of the line in bytes.</param>
/// <param name="store">How many of the line's bytes there is room for in `out`.</param>
/// <returns>bool; Did the file hold the whole line?</returns>
bool Decode_PCX_Line(PCXByteReader & reader, unsigned char * out, int length, int store)
{
	int produced = 0;
	while (produced < length) {
		unsigned char value;
		if (!reader.Get(value)) {
			return(false);
		}

		int count = 1;
		if ((value & 0xC0) == 0xC0) {
			count = value & 0x3F;
			if (!reader.Get(value)) {
				return(false);
			}
		}

		if (count > length - produced) {
			count = length - produced;
		}
		for (int step = 0; step < count && produced + step < store; step++) {
			out[produced + step] = value;
		}
		produced += count;
	}
	return(true);
}

}


/// <summary>
/// Reads a PCX picture into a surface.
/// A picture with one plane of 8 bit pixels becomes a paletted surface, and one with three planes
/// becomes a 16 bit surface in the display's pixel format. The surface is built from the size the
/// header states, so a header that states more than 4096 pixels on a side is refused.
/// </summary>
/// <param name="file_handle">The file to read.</param>
/// <param name="palette">Receives the picture's 256 colors when the picture has a single plane and
/// the file ends with them. May be NULL.</param>
/// <param name="Buff">The memory to build the surface in, or NULL to allocate it. A picture taller
/// than the memory holds loses its lower rows.</param>
/// <param name="Size">The size of `Buff` in bytes.</param>
/// <returns>Returns with the surface, which the caller owns, or NULL if the file is not a picture
/// the reader accepts or ends before the picture does.</returns>
Surface * Read_PCX_File(FileClass & file_handle, PaletteClass * palette, void * Buff, int Size)
{
	if (!file_handle.Is_Available()) return(NULL);

	if (!file_handle.Open(FileClass::READ)) {
		DebugString("PCX %s rejected: it cannot be opened\n", file_handle.File_Name());
		return(NULL);
	}

	PCX_HEADER header;
	if (file_handle.Read(&header, sizeof(PCX_HEADER)) != (int)sizeof(PCX_HEADER)) {
		DebugString("PCX %s rejected: its header is cut short\n", file_handle.File_Name());
		file_handle.Close();
		return(NULL);
	}

	int const width = (int)header.width - (int)header.x + 1;
	int height = (int)header.height - (int)header.y + 1;
	int const planes = header.color_planes;
	int const line_bytes = header.byte_per_line;

	if (header.id != 10 || header.pixelsize != 8 || (planes != 1 && planes != 3)
		|| width < 1 || width > PCX_MAX_SIDE || height < 1 || height > PCX_MAX_SIDE || line_bytes < width) {
		DebugString("PCX %s rejected: id %d, %d bit pixels, %d planes, %dx%d pixels and %d bytes a line\n", file_handle.File_Name(), (int)header.id, (int)header.pixelsize, planes, width, height, line_bytes);
		file_handle.Close();
		return(NULL);
	}

	int const bytes_per_pixel = (planes == 1) ? 1 : 2;

	if (Buff != NULL) {
		int const rows = Size / (width * bytes_per_pixel);
		height = std::min(rows, height);
		if (height < 1) {
			DebugString("PCX %s rejected: its %d byte buffer holds no whole row\n", file_handle.File_Name(), Size);
			file_handle.Close();
			return(NULL);
		}
	}

	BSurface * pic = new BSurface(width, height, bytes_per_pixel, Buff);
	unsigned char * buffer = (unsigned char *)pic->Lock();
	if (buffer == NULL) {
		DebugString("PCX %s rejected: its picture could not be locked\n", file_handle.File_Name());
		delete pic;
		file_handle.Close();
		return(NULL);
	}
	std::memset(buffer, 0, (std::size_t)width * height * bytes_per_pixel);

	PCXByteReader reader(file_handle);
	bool whole = true;

	if (planes == 3) {

		/*
		**	Each line holds its red, green and blue planes one after the other. They are
		**	interleaved into 16 bit pixels.
		*/
		std::vector<unsigned char> line((std::size_t)line_bytes * 3);
		unsigned short * hibuffer = (unsigned short *)buffer;

		for (int row = 0; row < height && whole; row++) {
			whole = Decode_PCX_Line(reader, line.data(), (int)line.size(), (int)line.size());
			if (whole) {
				unsigned char const * rptr = line.data();
				unsigned char const * gptr = rptr + line_bytes;
				unsigned char const * bptr = gptr + line_bytes;
				for (int column = 0; column < width; column++) {
					*hibuffer++ = (unsigned short)DSurface::Build_Hicolor_Pixel(*rptr++, *gptr++, *bptr++);
				}
			}
		}

	} else if (line_bytes != width) {

		/*
		**	The lines carry padding after the width, which is read and dropped.
		*/
		for (int row = 0; row < height && whole; row++) {
			whole = Decode_PCX_Line(reader, buffer + (std::size_t)row * width, line_bytes, width);
		}

	} else {

		/*
		**	With no padding the runs may carry on from one line into the next, so the picture is
		**	decoded as a single run of pixels.
		*/
		whole = Decode_PCX_Line(reader, buffer, width * height, width * height);
	}

	pic->Unlock();

	if (!whole) {
		DebugString("PCX %s rejected: its picture data ends before the picture does (%dx%d pixels, %d planes)\n", file_handle.File_Name(), width, height, planes);
		delete pic;
		file_handle.Close();
		return(NULL);
	}

	if (palette && planes == 1 && file_handle.Size() >= (int)(sizeof(PCX_HEADER) + 256 * sizeof(RGB))) {
		file_handle.Seek(- (256 * (int)sizeof(RGB)), SEEK_END );
		file_handle.Read (palette, 256L * sizeof ( RGB ));
	}

	file_handle.Close();
	return(pic);
}
