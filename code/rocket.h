/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "loco.h"


// Flies a spawned missile, such as a V3 rocket, from its launcher to its target and
// detonates it there (Yuri's Revenge's RocketLocomotionClass, 0x661EC0).
class RocketLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:
		RocketLocomotionClass(void);
		virtual ~RocketLocomotionClass(void) override;

		virtual ClassID Class_ID(void) const override;
		virtual void Serialize(SaveStreamClass & stream) override;

		virtual bool Is_Moving(void) override;
		virtual Coord Destination(void) override;
		virtual void Move_To(Coord to) override;
		virtual void Stop_Moving(void) override;
		virtual bool Process(void) override;
		virtual LayerType In_Which_Layer(void) override;
		virtual Matrix3D Draw_Matrix(int * key) override;
		virtual bool Is_To_Have_Shadow(void) override {return(false);}

	private:
		enum MissionStateType {
			STATE_NONE,
			STATE_PAUSE,
			STATE_TILT,
			STATE_CLIMB,
			STATE_CRUISE,
			STATE_DIVE,
		};

		bool Timer_Expired(void) const;
		double Timer_Progress(void) const;
		void Start_Timer(int frames);
		bool Is_Impact_Due(void);
		void Explode(void);
		void Move_Along(void);

		// The point the missile flies to and explodes at.
		Coord DestinationCoord;

		// The flight phase, and when the current timed phase started and how long it lasts.
		int MissionState;
		int TimerStart;
		int TimerLength;

		// The frame the next trail puff is due.
		int NextTrailFrame;

		// The speed in leptons per frame, and the climb angle in radians, positive upward.
		double CurrentSpeed;
		float CurrentPitch;

		// The horizontal distance to the target when the climb ended, which shapes a lazy curve.
		int ApogeeDistance;

		// Was the launcher elite when the missile left? It decides the damage and warhead.
		bool IsSpawnerElite;
};
