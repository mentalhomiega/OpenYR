/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the layout revision a save carries: a save stamped with this build's revision reads back
// as current, and one stamped with any other revision, or written before saves recorded one,
// does not. Every file it touches it creates itself, in a scratch directory named by the first
// argument or the working directory, so it reads no game data and leaves nothing behind.

#include "savefile.h"
#include "savever.h"

#include <lzo/lzo1x.h>

#include <cstdio>
#include <string>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-72s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


// The fields every save carries, as one written before saves recorded a revision has them.
void Fill_Listing(SaveFileClass & file)
{
	file.Set_String(PIDSI_SCEN_DESCRIP, "Quick Save: Lone Guardian");
	file.Set_Int(PIDSI_INTERNAL_VER, 0x00020000);
	file.Set_Int(PIDSI_GAME_TYPE, 0);
}

}


int main(int argc, char ** argv)
{
	if (lzo_init() != LZO_E_OK) {
		std::printf("lzo_init failed\n");
		return(2);
	}

	/*
	 * The value a build writes is never 0, which is what a save without the field reads as.
	 */
	Check(SaveVersionInfo::REVISION > 0, "the revision a build writes is above the one an unstamped save reads as");

	/*
	 * A save stamped with this build's revision reads back as current.
	 */
	{
		SaveVersionInfo info;
		info.Set_Internal_Version(0x00020000);
		info.Set_Revision(SaveVersionInfo::REVISION);
		SaveFileClass file;
		info.Save(file);

		SaveVersionInfo back;
		Check(back.Load(file), "a stamped save is listed");
		Check(back.Get_Revision() == SaveVersionInfo::REVISION, "the revision written is the revision read");
		Check(back.Is_Current_Revision(), "a save of this build's revision is current");
	}

	/*
	 * The revision survives the file on disk and the fields-only read the load list makes.
	 */
	{
		std::string const scratch = (argc > 1) ? argv[1] : ".";
		CreateDirectoryA(scratch.c_str(), nullptr);
		std::string const path = scratch + "\\REVISION.SAV";

		SaveVersionInfo info;
		info.Set_Internal_Version(0x00020000);
		info.Set_Revision(SaveVersionInfo::REVISION);
		SaveFileClass file;
		info.Save(file);
		file.Content.assign(64, 0x5A);
		Check(file.Write(path.c_str()) == SaveFileClass::RESULT_OK, "a stamped save is written");

		SaveFileClass listed;
		Check(listed.Read_Fields(path.c_str()) == SaveFileClass::RESULT_OK, "its fields are read back for the list");
		SaveVersionInfo back;
		Check(back.Load(listed) && back.Is_Current_Revision(), "the listed save is current");

		DeleteFileA(path.c_str());
	}

	/*
	 * A save written before saves recorded a revision is still listed, as revision 0, and is
	 * not current.
	 */
	{
		SaveFileClass file;
		Fill_Listing(file);

		SaveVersionInfo back;
		Check(back.Load(file), "a save without a revision is still listed");
		Check(back.Get_Revision() == 0, "a save without a revision reads as revision 0");
		Check(!back.Is_Current_Revision(), "a save without a revision is not current");
	}

	/*
	 * Any other revision, earlier or later, is not current.
	 */
	for (int offset : {-1, 1, 1000}) {
		SaveFileClass file;
		Fill_Listing(file);
		file.Set_Int(PIDSI_SAVE_REVISION, SaveVersionInfo::REVISION + offset);

		SaveVersionInfo back;
		back.Load(file);
		char what[96];
		std::snprintf(what, sizeof(what), "a save of revision %d is not current", SaveVersionInfo::REVISION + offset);
		Check(back.Get_Revision() == SaveVersionInfo::REVISION + offset && !back.Is_Current_Revision(), what);
	}

	/*
	 * A revision field of the wrong kind counts as no revision.
	 */
	{
		SaveFileClass file;
		Fill_Listing(file);
		file.Set_String(PIDSI_SAVE_REVISION, "1");

		SaveVersionInfo back;
		back.Load(file);
		Check(back.Get_Revision() == 0 && !back.Is_Current_Revision(), "a revision stored as text is no revision");
	}

	/*
	 * A block that read a current save and then one without a revision reports the second.
	 */
	{
		SaveVersionInfo stamped;
		stamped.Set_Revision(SaveVersionInfo::REVISION);
		SaveFileClass current;
		stamped.Save(current);

		SaveFileClass older;
		Fill_Listing(older);

		SaveVersionInfo back;
		back.Load(current);
		back.Load(older);
		Check(!back.Is_Current_Revision(), "reading an unstamped save over a stamped one leaves no revision");
	}

	/*
	 * A block whose revision was never set writes none.
	 */
	{
		SaveVersionInfo unset;
		SaveFileClass file;
		unset.Save(file);

		SaveVersionInfo back;
		back.Load(file);
		Check(!back.Is_Current_Revision(), "a block whose revision was never set is not current");
	}

	std::printf("%d failures\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
