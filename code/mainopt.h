/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

bool Change_Display_Mode(int width, int height);
void Main_Options_Dialog(void);
void Display_Options_Dialog(int page);
struct UIDisplayMode;
// Applies a picked mode as a trial and keeps it if the player confirms; false if they did not, so the display page opens again.
bool Display_Mode_Kept(UIDisplayMode const & picked, float scale);
