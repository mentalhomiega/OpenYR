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

#include "always.h"

#include "abstype.h"
#include "csf.h"

#include "ccini.h"
#include "crc.h"
#include "globals.h"
#include "savestream.h"
#include "vector.h"

#include <cinttypes>
#include <cstdio>


/***********************************************************************************************
 * AbstractTypeClass::AbstractTypeClass -- Constructor for abstract type objects.              *
 *                                                                                             *
 *    This is the constructor for AbstractTypeClass objects. It initializes the INI name and   *
 *    the text name for this object type.                                                      *
 *                                                                                             *
 * INPUT:   name  -- Text number for the full name of the object.                              *
 *                                                                                             *
 *          ini   -- The ini name for this object type.                                        *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/22/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
AbstractTypeClass::AbstractTypeClass(char const * ininame) :
	BASECLASS(),
	IniName(),
	GivenName()
{
	if (ininame == NULL) {
		char pstr[2 * sizeof(void *) + 1];
		sprintf(pstr, "%0*" PRIXPTR, (int)(2 * sizeof(void *)), (uintptr_t)this);
		IniName = TStringID<24>(pstr);
	} else {
		IniName = TStringID<24>(ininame);
	}

	GivenName = TStringID<48>(IniName);
	AbstractTypes.Add(this);
}


/// <summary>
/// Removes this object type from the master type list.
/// Every abstract type adds itself to the AbstractTypes list when it is created, so it
/// must take itself back off again here or the list would be left holding a dead pointer.
/// </summary>
AbstractTypeClass::~AbstractTypeClass(void)
{
	AbstractTypes.Delete(this);
}


/***********************************************************************************************
 * AbstractTypeClass::Read_INI -- Reads the techno type data from the INI database.            *
 *                                                                                             *
 *    Use this routine to fill in the data for this techno type class object from the          *
 *    database specified. Typical use of this is for the rules parsing.                        *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database that the information will be lifted from.   *
 *                                                                                             *
 * OUTPUT:  bool; Was the database used to extract information? A failure (false) response     *
 *                would mean that the database didn't contain a section that applies to this   *
 *                techno class object.                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
// Yuri's Revenge names an object by its UIName string (AbstractTypeClass::Read_INI); a label the table lacks falls back to Name.
const char * AbstractTypeClass::Full_Name(void) const
{
	if (!UINameLabel.empty()) {
		if (UINameText.empty()) {
			wchar_t const * text = StringTable.Find(UINameLabel.c_str());
			if (text != NULL && *text != L'\0') {
				int const size = WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);
				if (size > 1) {
					UINameText.resize(size - 1);
					WideCharToMultiByte(CP_UTF8, 0, text, -1, UINameText.data(), size, NULL, NULL);
				}
			}
		}
		if (!UINameText.empty()) {
			return(UINameText.c_str());
		}
	}
	return(GivenName);
}


bool AbstractTypeClass::Read_INI(CCINIClass const & ini)
{
	if (ini.Section_Present(IniName)) {

		ini.Get_String(IniName, "Name", GivenName);
		char label[64];
		if (ini.Get_String(IniName, "UIName", UINameLabel.c_str(), label, sizeof(label)) > 0 && UINameLabel != label) {
			UINameLabel = label;
			UINameText.clear();
		}
		return(true);
	}
	return(false);
}


/// <summary>
/// Writes this object type's data out to the INI database.
/// This routine is the counterpart to Read_INI. The type's own section is cleared before
/// anything is written, so no stale entries survive from an earlier save.
/// </summary>
bool AbstractTypeClass::Write_INI(CCINIClass &ini) const
{
	ini.Clear(IniName);
	ini.Put_String(IniName, "Name", GivenName);
	return(true);
}


/// <summary>
/// Adds this object type's identity to a CRC calculation.
/// This routine is called while the game checksum is being built so that both the INI
/// name and the displayed name of the type contribute to the result.
/// </summary>
void AbstractTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((const char *)IniName);
	crc((const char *)GivenName);
}


/// <summary>
/// Lists the members every object type carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void AbstractTypeClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(IniName);
	stream.Serialize(GivenName);
	stream.Serialize(UINameLabel);
}
