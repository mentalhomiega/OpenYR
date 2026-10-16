/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// In-memory sources for the harnesses that read through the engine's straw and file classes.

#pragma once

#include "fuzz.h"

#include "straw.h"
#include "wwfile.h"
#include "xstraw.h"

#include <algorithm>
#include <cstring>


namespace Fuzz
{
	// Calls the function with a straw over the bytes. A BufferStraw of length zero reads without
	// a limit, so an empty input gets a straw that has nothing to give.
	template <typename Function>
	void With_Straw(Bytes const & bytes, Function && function)
	{
		if (bytes.empty()) {
			Straw nothing;
			function(nothing);
		} else {
			BufferStraw straw(bytes.data(), (int)bytes.size());
			function(straw);
		}
	}


	// A read-only file over a copy of the bytes. It follows FileClass's rules for Read and Seek:
	// a read stops at the end of the data, and a seek is not clamped, so a position past either
	// end is reported by the next read returning less than it was asked for.
	class MemoryFile : public FileClass
	{
		public:
			explicit MemoryFile(Bytes const & bytes) : Data(bytes) {}

			virtual char const * File_Name(void) const override {return("memory");}
			virtual char const * Set_Name(char const *) override {return(File_Name());}
			virtual int Create(void) override {return(0);}
			virtual int Delete(void) override {return(0);}
			virtual bool Is_Available(int = false) override {return(true);}
			virtual bool Is_Open(void) const override {return(IsOpen);}
			virtual int Open(char const *, int rights = READ) override {return(Open(rights));}
			virtual int Open(int = READ) override {IsOpen = true; Position = 0; return(1);}
			virtual int Size(void) override {return((int)Data.size());}
			virtual int Write(void const *, int) override {return(0);}
			virtual void Close(void) override {IsOpen = false;}
			virtual void Error(int, int = false, char const * = nullptr) override {}

			virtual int Read(void * buffer, int size) override
			{
				if (size <= 0 || Position < 0 || Position >= (long long)Data.size()) {
					return(0);
				}
				long long const count = std::min<long long>(size, (long long)Data.size() - Position);
				std::memcpy(buffer, Data.data() + Position, (std::size_t)count);
				Position += count;
				return((int)count);
			}

			virtual int Seek(int offset, int origin = SEEK_CUR) override
			{
				long long base = Position;
				if (origin == SEEK_SET) {
					base = 0;
				} else if (origin == SEEK_END) {
					base = (long long)Data.size();
				}
				Position = base + offset;
				return((int)Position);
			}

		private:
			Bytes Data;
			long long Position = 0;
			bool IsOpen = false;
	};
}
