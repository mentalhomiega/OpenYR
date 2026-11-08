/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

class EventClass;

/// <summary>
/// The game checksum after each part of the sum, in order: infantry, units, buildings, map layers,
/// logic list and random number. The last is the checksum itself. Count holds the infantry, unit,
/// building and logic-list totals.
/// </summary>
struct SyncFrameDetail
{
	unsigned Prefix[6];
	unsigned Count[4];
};

void Sync_Record_Frame_Detail(int frame, SyncFrameDetail const & detail);

void Print_CRCs(EventClass const * events, int count, unsigned const * crc_ring, unsigned ring_size);
bool Report_Out_Of_Sync(EventClass const * events, int count, unsigned const * crc_ring, unsigned ring_size);
void Sync_Report_Reset(void);

bool Sync_Is_Out_Of_Sync(int house_id);
void Sync_Mark_Out_Of_Sync(int house_id);
