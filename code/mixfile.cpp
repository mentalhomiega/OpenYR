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

/* $Header: /CounterStrike/MIXFILE.CPP 2     3/13/97 2:06p Steve_tall $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : MIXFILE.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : August 8, 1994                                               *
 *                                                                                             *
 *                  Last Update : July 12, 1996 [JLB]                                          *
 *                                                                                             *
 *                                                                                             *
 *                  Modified by Vic Grippi for WwXlat Tool 10/14/96                            *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   MixFileClass::Cache -- Caches the named mixfile into RAM.                                 *
 *   MixFileClass::Cache -- Loads this particular mixfile's data into RAM.                     *
 *   MixFileClass::Finder -- Finds the mixfile object that matches the name specified.         *
 *   MixFileClass::Free -- Uncaches a cached mixfile.                                          *
 *   MixFileClass::MixFileClass -- Constructor for mixfile object.                             *
 *   MixFileClass::Offset -- Searches in mixfile for matching file and returns offset if found.*
 *   MixFileClass::Retrieve -- Retrieves a pointer to the specified data file.                 *
 *   MixFileClass::~MixFileClass -- Destructor for the mixfile object.                         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "mixfile.h"

#include "bsearch.h"
#include "buff.h"
#include "ccfile.h"
#include "conquer.h"
#include "crc.h"
#include "dbgprint.h"
#include "globals.h"
#include "pk.h"
#include "pkstraw.h"
#include "truecolour.h"
#include "shastraw.h"
#include "xstraw.h"

#include <climits>


//template<class T> int Compare(T const *obj1, T const *obj2) {
//	if (*obj1 < *obj2) return(-1);
//	if (*obj1 > *obj2) return(1);
//	return(0);
//};


/*
**	This is the pointer to the first mixfile in the list of mixfiles registered
**	with the mixfile system.
*/
//template<class T>
List<MixFileClass *> MixFileClass::List;

/// template class MixFileClass<CCFileClass>;

/// <summary>
/// Compares two mixfile index entries by their filename CRC.
/// This is the support routine that the standard search and sort routines use when they
/// need to order or scan the index block of a mixfile.
/// </summary>
/// <param name="ptr1">Pointer to the first index entry to compare.</param>
/// <param name="ptr2">Pointer to the second index entry to compare.</param>
/// <returns>Returns with -1, 0, or 1 according to whether the first entry sorts before,
/// with, or after the second.</returns>
int __cdecl compfunc(void const * ptr1, void const * ptr2)
{
	if (*(int const *)ptr1 < *(int const *)ptr2) return(-1);
	if (*(int const *)ptr1 > *(int const *)ptr2) return(1);
	return(0);
}


/***********************************************************************************************
 * MixFileClass::MixFileClass -- Constructor for mixfile object.                               *
 *                                                                                             *
 *    This is the constructor for the mixfile object. It takes a filename and a memory         *
 *    handler object and registers the mixfile object with the system. The index block is      *
 *    allocated and loaded from disk by this routine.                                          *
 *                                                                                             *
 * INPUT:   filename -- Pointer to the filename of the mixfile object.                         *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *   07/12/1996 JLB : Handles compressed file header.                                          *
 *=============================================================================================*/
MixFileClass::MixFileClass(char const * filename, PKey const * key) :
	IsDigest(false),
	IsEncrypted(false),
	IsAllocated(false),
	Filename(NULL),
	Count(0),
	DataSize(0),
	DataStart(0),
	HeaderBuffer(NULL),
	Data(NULL)
{
	CCFileClass file(filename);		// Working file object.
	Filename = strdup(file.File_Name());
	// The key straw is declared first so that it outlives the file straw it is chained to. The
	// file straw's destructor reaches back to the straw chained from it.
	PKStraw pstraw(PKStraw::DECRYPT, CryptRandom);
	FileStraw fstraw(file);
	Straw * straw = &fstraw;

	if (!file.Is_Available()) return;

	/*
	**	Stuctures used to hold the various file headers.
	*/
	FileHeader fileheader = {};
	struct {
		short First;		// Always zero for extended mixfile format.
		short Second;		// Bitfield of extensions to this mixfile.
	} alternate = {};

	/*
	**	Fetch the first bit of the file. From this bit, it is possible to detect
	**	whether this is an extended mixfile format or the plain format. An
	**	extended format may have extra options or data layout. A file too short to hold
	**	even that is not a mixfile.
	*/
	int got = straw->Get(&alternate, sizeof(alternate));
	if (got != sizeof(alternate)) {
		DebugString("MIX %s rejected: it is too short for a header\n", Filename);
		return;
	}

	/*
	**	Detect if this is an extended mixfile. If so, then see if it is encrypted
	**	and/or has a message digest attached. Otherwise, just retrieve the
	**	plain mixfile header.
	*/
	if (alternate.First == 0) {
		IsDigest = ((alternate.Second & 0x01) != 0);
		IsEncrypted = ((alternate.Second & 0x02) != 0);

		if (IsEncrypted) {
			pstraw.Key(key);
			pstraw.Get_From(&fstraw);
			straw = &pstraw;
		}
		if (straw->Get(&fileheader, sizeof(fileheader)) != sizeof(fileheader)) {
			DebugString("MIX %s rejected: its header is cut short\n", Filename);
			return;
		}

	} else {
		memmove(&fileheader, &alternate, sizeof(alternate));
		int const rest = sizeof(fileheader)-sizeof(alternate);
		if (straw->Get(((char*)&fileheader)+sizeof(alternate), rest) != rest) {
			DebugString("MIX %s rejected: its header is cut short\n", Filename);
			return;
		}
	}

	/*
	**	The counts are the file's claims. A negative one describes no mixfile.
	*/
	if (fileheader.count < 0 || fileheader.size < 0) {
		DebugString("MIX %s rejected: its header claims %d entries and %d bytes\n", Filename, (int)fileheader.count, (int)fileheader.size);
		return;
	}

	Count = fileheader.count;
	DataSize = fileheader.size;
//BGMono_Printf("Mixfileclass %s DataSize: %08x   \n",filename,DataSize);Get_Key();
	/*
	**	Load up the offset control array. A directory that the file does not hold in full
	**	is not a mixfile.
	*/
	HeaderBuffer = new SubBlock [Count];
	if (HeaderBuffer == NULL) return;
	if (straw->Get(HeaderBuffer, Count * sizeof(SubBlock)) != (int)(Count * sizeof(SubBlock))) {
		DebugString("MIX %s rejected: its directory of %d entries is cut short\n", Filename, Count);
		delete [] HeaderBuffer;
		HeaderBuffer = NULL;
		Count = 0;
		DataSize = 0;
		return;
	}

	/*
	**	The start of the embedded mixfile data will be at the current file offset.
	**	This should be true even if the file header has been encrypted because the file
	**	header was cleverly written with just the sufficient number of padding bytes so
	**	that this condition would be true.
	*/
	int const header_end = file.Seek(0, SEEK_CUR);
	DataStart = header_end + file.BiasStart;
//	DataStart = file.Seek(0, SEEK_CUR);

	/*
	**	The data cannot be larger than what the file holds after its directory, and a member
	**	can only be found if it lies inside the data. A member that reaches outside is left out
	**	of the directory, so that no lookup hands out a position beyond the file or its cache.
	*/
	long long available = (long long)file.Size() - header_end;
	if (available < 0) available = 0;
	if (available > INT_MAX) available = INT_MAX;
	long long data_size = (DataSize < available) ? DataSize : available;
	int const listed = Count;
	int kept = 0;
	for (int index = 0; index < Count; index++) {
		SubBlock const & block = HeaderBuffer[index];
		long long const end = (long long)block.Offset + block.Size;
		if (block.Offset >= 0 && block.Size >= 0 && end <= available) {
			HeaderBuffer[kept++] = block;

			// A header that undercounts its data still finds the members that lie inside the file.
			if (end > data_size) data_size = end;
		}
	}
	if (kept < listed) {
		DebugString("MIX %s: %d of its %d entries are left out, as they lie outside the file\n", Filename, listed - kept, listed);
	}
	Count = kept;
	DataSize = (int)data_size;

	/*
	**	Attach to list of mixfiles.
	*/
	List.Add_Tail(this);
}


/***********************************************************************************************
 * MixFileClass::Free -- Uncaches a cached mixfile.                                            *
 *                                                                                             *
 *    Use this routine to uncache a mixfile that has been cached.                              *
 *                                                                                             *
 * INPUT:   filename -- Pointer to the filename of the mixfile that is to be uncached.         *
 *                                                                                             *
 * OUTPUT:  bool; Was the mixfile found and freed?                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/23/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool MixFileClass::Free(char const * filename)
{
	MixFileClass * ptr = Finder(filename);

	if (ptr) {
		ptr->Free();
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * MixFileClass::~MixFileClass -- Destructor for the mixfile object.                           *
 *                                                                                             *
 *    This destructor will free all memory allocated by this mixfile and will remove it from   *
 *    the system. A mixfile removed in this fashion must be created anew in order to be        *
 *    subsequent used.                                                                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *   01/06/1995 JLB : Puts mixfile header table into EMS.                                      *
 *=============================================================================================*/
MixFileClass::~MixFileClass(void)
{
	/*
	**	Deallocate any allocated memory.
	*/
	if (Filename) {
		free((char *)Filename);
	}
	if (Data != NULL) {
		TrueColour_Forget_Range(Data, (std::size_t)DataSize);
	}
	if (Data != NULL && IsAllocated) {
		delete [] Data;
		IsAllocated = false;
	}
	Data = NULL;

	if (HeaderBuffer != NULL) {
		delete [] HeaderBuffer;
		HeaderBuffer = NULL;
	}

	/*
	**	Unlink this mixfile object from the chain.
	*/
	Unlink();
}


/***********************************************************************************************
 * MixFileClass::Retrieve -- Retrieves a pointer to the specified data file.                   *
 *                                                                                             *
 *    This routine will return with a pointer to the specified data file if the file resides   *
 *    in memory. Otherwise, this routine returns NULL. Use this routine to access a resident   *
 *    file directly rather than going through the process of pseudo disk access. This will     *
 *    save both time and RAM.                                                                  *
 *                                                                                             *
 * INPUT:   filename -- Pointer to the filename of the data file to retrieve a pointer to.     *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the data file's data. If the file is not in RAM, then    *
 *          NULL is returned. A shape file in no mixfile returns the shape made for its PNG    *
 *          sheet instead, when there is one (see TRUECOLOUR.md).                              *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/23/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void const * MixFileClass::Retrieve(char const * filename)
{
	void * ptr = NULL;
	MixFileClass * mixfile = NULL;
	int offset = 0;
	int size = 0;
	if (!Offset(filename, &ptr, &mixfile, &offset, &size)) {
		return(TrueColour_Png_Only_Shape(filename));
	}
	if (ptr == NULL && mixfile != NULL && mixfile->ResidentMembers) {
		ptr = (void *)mixfile->Resident_Member(offset, size);
	}
	TrueColour_Note_Shape(ptr, filename);
	return(ptr);
};


/// <summary>
/// Reads one member of an archive that is not cached into memory kept until the archive is
/// destroyed.
/// </summary>
/// <param name="position">The member's position in the archive file, from Offset.</param>
/// <param name="size">The member's size in bytes.</param>
/// <returns>The member's bytes, or NULL when it cannot be read.</returns>
void const * MixFileClass::Resident_Member(int position, int size)
{
	auto found = Members.find(position);
	if (found != Members.end()) {
		return(found->second.get());
	}

	if (size <= 0) {
		return(NULL);
	}

	CCFileClass file(Filename);
	if (!file.Open(FileClass::READ)) {
		return(NULL);
	}
	file.Bias(0);
	file.Bias(DataStart);

	std::unique_ptr<char[]> data(new char[size]);
	file.Seek(position - DataStart, SEEK_SET);
	if (file.Read(data.get(), size) != size) {
		return(NULL);
	}

	char const * result = data.get();
	Members[position] = std::move(data);
	return(result);
}


/***********************************************************************************************
 * MixFileClass::Finder -- Finds the mixfile object that matches the name specified.           *
 *                                                                                             *
 *    This routine will scan through all registered mixfiles and return with a pointer to      *
 *    the matching mixfile. If no mixfile could be found that matches the name specified,      *
 *    then NULL is returned.                                                                   *
 *                                                                                             *
 * INPUT:   filename -- Pointer to the filename to search for.                                 *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the matching mixfile -- if found.                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *   06/08/1996 JLB : Only compares filename and extension.                                    *
 *=============================================================================================*/
MixFileClass * MixFileClass::Finder(char const * filename)
{
	MixFileClass * ptr = List.First();
	while (ptr->Is_Valid()) {
		char path[_MAX_PATH];
		char name[_MAX_FNAME];
		char ext[_MAX_EXT];

		/*
		**	Strip the drive and path (if present) off of the filename
		**	in the mixfile list. This enables a simple comparison to the
		**	filename specified. The filename specified won't have a path attached and
		**	the full pathname in the mixfile list WILL have a path attached. Hence, this
		**	stripping of the path is necessary.
		*/
		_splitpath(ptr->Filename, NULL, NULL, name, ext);
		_makepath(path, NULL, NULL, name, ext);

		if (stricmp(path, filename) == 0) {
			return(ptr);
		}
		ptr = ptr->Next();
	}
	return(NULL);
}


/***********************************************************************************************
 * MixFileClass::Cache -- Caches the named mixfile into RAM.                                   *
 *                                                                                             *
 *    This routine will cache the mixfile, specified by name, into RAM.                        *
 *                                                                                             *
 * INPUT:   filename -- The name of the mixfile that should be cached.                         *
 *                                                                                             *
 * OUTPUT:  bool; Was the cache successful?                                                    *
 *                                                                                             *
 * WARNINGS:   This routine could go to disk for a very long time.                             *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool MixFileClass::Cache(char const * filename, Buffer const * buffer)
{
	MixFileClass * mixer = Finder(filename);

	if (mixer != NULL) {
		return(mixer->Cache(buffer));
	}
	return(false);
}


/***********************************************************************************************
 * MixFileClass::Cache -- Loads this particular mixfile's data into RAM.                       *
 *                                                                                             *
 *    This load the mixfile data into ram for this mixfile object. This is the counterpart     *
 *    to the Free() function.                                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Was the file load successful?  It could fail if there wasn't enough room     *
 *                to allocate the raw data block.                                              *
 *                                                                                             *
 * WARNINGS:   This routine goes to disk for a potentially very long time.                     *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *   07/12/1996 JLB : Handles attached message digest.                                         *
 *=============================================================================================*/
bool MixFileClass::Cache(Buffer const * buffer)
{
	/*
	**	If the mixfile is already cached, then no action needs to be performed.
	*/
	if (Data != NULL) return(true);

	/*
	**	If a buffer was supplied (and it is big enough), then use it as the data block
	**	pointer. Otherwise, the data block must be allocated.
	*/
	if (buffer != NULL && (buffer->Get_Size() == 0 || buffer->Get_Size() >= DataSize)) {
		Data = buffer->Get_Buffer();

	} else {
		Data = new char [DataSize];
		IsAllocated = true;

		/// Purpose unknown; possibly a debugging aid.
		for (char *i = (char *)Data; i < &((char *)Data)[DataSize]; i += 4096) {

			/// Required to stop the compiler from optimizing this loop away.
			volatile char tmp = *i;
		}

		Call_Back();
	}

	/*
	**	If there is a data buffer to fill, then fill it now.
	*/
	if (Data != NULL) {
		//T file(Filename);
		CCFileClass file(Filename);

		FileStraw fstraw(file);
		Straw * straw = &fstraw;

		/*
		**	If a message digest is attached, then link a SHA straw segment to the data
		**	stream so that the actual SHA can be compared with the attached one.
		*/
		SHAStraw sha;
		if (IsDigest) {
			sha.Get_From(fstraw);
			straw = &sha;
		}

		/*
		**	Bias the file to the actual start of the data. This is necessary because the
		**	real data starts some distance (not so easily determined) from the beginning of
		**	the real file.
		*/
		file.Open(FileClass::READ);
		file.Bias(0);
		file.Bias(DataStart);

		/*
		**	Fetch the whole mixfile data in one step. If the number of bytes retrieved
		**	does not equal that requested, then this indicates a serious error.
		*/
		int actual = straw->Get(Data, DataSize);
		if (actual != DataSize) {
			if (IsAllocated) delete [] Data;
			IsAllocated = false;
			Data = NULL;
			file.Error(EIO);
			return(false);
		}

		/*
		**	If there is a digest attached to this mixfile, then read it in and
		**	compare it to the generated digest. If they don't match, then
		**	return with the "failure to cache" error code.
		*/
		if (IsDigest) {
			char digest1[20];
			char digest2[20];
			sha.Result(digest2);
			fstraw.Get(digest1, sizeof(digest1));
			if (memcmp(digest1, digest2, sizeof(digest1)) != 0) {
				if (IsAllocated) delete [] Data;
				IsAllocated = false;
				Data = NULL;
				return(false);
			}
		}

		return(true);
	}
	IsAllocated = false;
	return(false);
}


/***********************************************************************************************
 * MixFileClass::Free -- Frees the allocated raw data block (not the index block).             *
 *                                                                                             *
 *    This routine will free the (presumably large) raw data block, but leave the index        *
 *    block intact. By using this in conjunction with the Cache() function, one can maintain   *
 *    tight control of memory usage. If the index block is desired to be freed, then the       *
 *    mixfile object must be deleted.                                                          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/08/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void MixFileClass::Free(void)
{
	if (Data != NULL) {
		TrueColour_Forget_Range(Data, (std::size_t)DataSize);
	}
	if (Data != NULL && IsAllocated) {
		delete [] Data;
	}
	Data = NULL;
	IsAllocated = false;
}


/***********************************************************************************************
 * MixFileClass::Offset -- Determines the offset of the requested file from the mixfile system.*
 *                                                                                             *
 *    This routine will take the filename specified and search through the mixfile system      *
 *    looking for it. If the file was found, then the mixfile it was found in, the offset      *
 *    from the start of the mixfile, and the size of the embedded file will be returned.       *
 *    Using this method it is possible for the CCFileClass system to process it as a normal    *
 *    file.                                                                                    *
 *                                                                                             *
 * INPUT:   filename    -- The filename to search for.                                         *
 *                                                                                             *
 *          realptr     -- Stores a pointer to the start of the file in memory here. If the    *
 *                         file is not in memory, then NULL is stored here.                    *
 *                                                                                             *
 *          mixfile     -- The pointer to the corresponding mixfile is placed here. If no      *
 *                         mixfile was found that contains the file, then NULL is stored here. *
 *                                                                                             *
 *          offset      -- The starting offset from the beginning of the parent mixfile is     *
 *                         stored here.                                                        *
 *                                                                                             *
 *          size        -- The size of the embedded file is stored here.                       *
 *                                                                                             *
 * OUTPUT:  bool; Was the file found? The file may or may not be resident, but it does exist   *
 *                 and can be opened.                                                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool MixFileClass::Offset(char const * filename, void ** realptr, MixFileClass ** mixfile, int * offset, int * size)
{
	MixFileClass * ptr;

	if (filename == NULL) {
		assert(filename != NULL);//BG
		return(false);
	}

	/*
	**	Create the key block that will be used to binary search for the file.
	*/

	/// Can't call strupr on a const string.
	char filename_upper[_MAX_PATH];
	if (strlen(filename) >= sizeof(filename_upper)) {
		DebugString("MIX lookup rejected: a name of %d characters is too long\n", (int)strlen(filename));
		return(false);
	}
	strcpy(filename_upper, filename);
	strupr(filename_upper);
	int crc = (CRCEngine()(filename_upper, strlen(filename_upper))); //Calculate_CRC(strupr((char *)filename), strlen(filename));

	SubBlock key;
	key.CRC = crc;

	/*
	**	Sweep through all registered mixfiles, trying to find the file in question.
	*/
	ptr = List.First();
	while (ptr->Is_Valid()) {
		SubBlock * block;

		/*
		**	Binary search for the file in this mixfile. If it is found, then extract the
		**	appropriate information and store it in the locations provided and then return.
		*/
		//block = (SubBlock *)bsearch(&key, ptr->HeaderBuffer, ptr->Count, sizeof(SubBlock), compfunc);
		block = Binary_Search<SubBlock>(ptr->HeaderBuffer, ptr->Count, key);
		if (block != NULL) {
			if (mixfile != NULL) *mixfile = ptr;
			if (size != NULL) *size = block->Size;
			if (realptr != NULL) *realptr = NULL;
			if (offset != NULL) *offset = block->Offset;
			if (realptr != NULL && ptr->Data != NULL) {
				*realptr = (char *)ptr->Data + block->Offset;
			}
			if (ptr->Data == NULL && offset != NULL) {
				*offset += ptr->DataStart;
			}
			return(true);
		}

		/*
		**	Advance to next mixfile.
		*/
		ptr = ptr->Next();
	}

	/*
	**	All the mixfiles have been examined but no match was found. Return with the non success flag.
	*/
	assert(1);//BG
	return(false);
}
