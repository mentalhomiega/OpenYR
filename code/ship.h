/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "drive.h"


// Moves naval units. It drives like DriveLocomotionClass; Yuri's Revenge's water-specific
// differences are not reproduced yet.
class ShipLocomotionClass : public DriveLocomotionClass
{
	public:
		virtual ClassID Class_ID(void) const override;
};
