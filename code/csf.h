/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

class Straw;

/*
 * A Red Alert 2 string table (.CSF): UTF-16 strings looked up by label, case-insensitively.
 * A string may carry an extra ASCII value, which names the speech file played with it.
 */
class CSFClass
{
	public:
		/*
		 * Replaces the table with the one read from the straw. Returns false, leaving the table
		 * empty, when the data has no CSF header or declares no labels or no strings. A malformed
		 * record ends the table and the labels before it are kept, as in Yuri's Revenge.
		 */
		bool Load(Straw & straw);

		/*
		 * Adds a further table to this loaded one, as Ares's extra string tables are added: each
		 * of its labels is added, or takes over a label this table already holds. Of labels
		 * repeated in the added table, the first one in it counts. A table in a language other
		 * than this one's is skipped unless its language is LANGUAGE_NEUTRAL. Returns false,
		 * leaving this table unchanged, when the table is skipped, does not load, or this table
		 * has not loaded.
		 */
		bool Merge(CSFClass const & other);
		bool Merge(Straw & straw);

		// The header language of a table that is added whatever the main table's language.
		static constexpr int LANGUAGE_NEUTRAL = -1;

		void Clear(void);

		bool Is_Loaded(void) const {return(Loaded);}
		int Count(void) const {return((int)Entries.size());}
		int Language(void) const {return(LanguageID);}

		/*
		 * The string for a label, or NULL when the table has no such label. When extra is given
		 * it receives the string's extra value, or NULL when there is none.
		 */
		wchar_t const * Find(char const * label, char const ** extra = nullptr) const;

		// The string for a label as UTF-8, or an empty string when the table has no such label.
		std::string Find_UTF8(char const * label) const;

	private:
		struct EntryType {
			std::string Label;
			std::wstring Value;
			std::string Extra;
			bool HasExtra;
		};

		std::vector<EntryType> Entries;
		int LanguageID = 0;
		bool Loaded = false;
};

extern CSFClass StringTable;

/*
 * The string for a label. A label the table lacks yields "MISSING:'<label>'" and is written
 * to the debug log; before a table has loaded every label yields a fixed error text.
 */
wchar_t const * Fetch_String(char const * label, char const ** extra = nullptr);
