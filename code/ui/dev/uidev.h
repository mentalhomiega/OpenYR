/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "win.h"

class UIRmlRenderClass;


// The developer window is shown; it takes the mouse and keys it wants.
bool UIDev_Active(void);

// The developer window or the performance overlay is in the frame UIDev_Render draws.
bool UIDev_Drawing(void);

void UIDev_Toggle(UIRmlRenderClass const & render);
void UIDev_Tick(UIRmlRenderClass const & render);
void UIDev_Render(UIRmlRenderClass & render);
void UIDev_Shutdown(UIRmlRenderClass & render);
// Frees the overlay's textures; it makes them again from the pixels it keeps when it next draws.
void UIDev_Release_Textures(UIRmlRenderClass & render);

void UIDev_Mouse_Position(int x, int y);
bool UIDev_Mouse_Button(int button, bool down);
bool UIDev_Mouse_Wheel(float delta);
bool UIDev_Key(WPARAM virtualkey, bool down);
bool UIDev_Character(wchar_t unit);
void UIDev_Focus(bool focused);
bool UIDev_Wants_Mouse(void);
