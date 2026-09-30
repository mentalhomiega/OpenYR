/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ship.h"

#include "classids.h"


ClassID ShipLocomotionClass::Class_ID(void) const
{
	return(ClassID_ShipLocomotion);
}
