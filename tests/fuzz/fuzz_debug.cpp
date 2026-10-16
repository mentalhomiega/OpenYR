/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Stands in for the debug log in every fuzz harness. The parsers report damaged input there,
// and a run of tens of thousands of damaged inputs would fill a log file and slow every
// iteration. The real log needs the Windows API as well. The messages are discarded after
// being formatted, so a format string that disagrees with its arguments still shows up under
// the sanitizers.

#include "dbgprint.h"

#include <cstdarg>
#include <cstdio>


void Debug_Init(void) {}
void Debug_Init_Console(void) {}
void Debug_Console_Hold(void) {}
char const * Debug_Log_File_Name(void) {return("");}
char const * Debug_Directory(void) {return("");}
bool Delete_Files_Older_Than(char const *, char const *, unsigned) {return(false);}
char const * Last_Error_Text(unsigned long) {return("");}


static void Format_And_Drop(char const * string, std::va_list arguments)
{
	char buffer[2048];
	std::vsnprintf(buffer, sizeof(buffer), string, arguments);
}


void __cdecl DebugString(char const * string, ...)
{
	std::va_list arguments;
	va_start(arguments, string);
	Format_And_Drop(string, arguments);
	va_end(arguments);
}


void __cdecl DebugStringNoPrefix(char const * string, ...)
{
	std::va_list arguments;
	va_start(arguments, string);
	Format_And_Drop(string, arguments);
	va_end(arguments);
}
