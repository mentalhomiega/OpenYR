/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rgb.h"

#include <string>

/*
 * The colors the skirmish and network lobbies offer. Without a [Colors] section in the UI file
 * they are the eight stock colors. A UI file lists up to sixteen, each with the color scheme the
 * player is drawn in, the swatch the lobby shows and a name.
 */

int Lobby_Color_Count(void);

// The swatch the lobby draws for a color index, which is clamped to the colors listed.
RGBClass Lobby_Color_RGB(int index);
// The same swatch as a Windows COLORREF.
unsigned long Lobby_Color_Ref(int index);

// The name the lobby shows for a color index. Empty when the index is one of the eight stock
// colors that the UI file leaves unnamed, whose names the lobby supplies.
std::string Lobby_Color_Name(int index);

// The scheme index a color index is drawn in, or -1 when the stock table decides it. The scheme is
// the one built for lighting, as the stock table's are.
int Lobby_Color_Scheme(int index);
int Lobby_Observer_Scheme(void);
