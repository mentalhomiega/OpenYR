/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Offers crash, hang and out-of-sync reports to the developers, as
// [Options] ReportUpload and ReportURL say. DRAFT: with ReportURL empty, the
// default, none of this asks or sends anything.

#pragma once


// Asks about the crash and hang folders the player has not answered for, then
// starts sending whatever is waiting. Called once, before the first menu.
void Report_Startup(void);

// Notes the out-of-sync report this seat just wrote, to ask about once the
// match is over.
void Report_Note_Desync(char const * sync_path);

// Asks about an out-of-sync report noted during the match that just ended,
// and about any hang the watchdog caught during it, then sends.
void Report_After_Match(void);
