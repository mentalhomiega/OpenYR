/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/RADIO.H 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : RADIO.H                                                      *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : April 23, 1994                                               *
 *                                                                                             *
 *                  Last Update : April 23, 1994   [JLB]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#include "globals.h"
#include "mission.h"

#include <cstdint>
#include <vector>

class ObjectClass;
class TechnoClass;


/****************************************************************************
**	Radio contact is controlled by this class. It handles the mundane chore
**	of keeping the radio contact alive as well as broadcasting messages
**	to the receiving radio. Radio contact is primarily used when one object
**	is in "command" of another.
*/
class RadioClass : public MissionClass
{
		typedef MissionClass BASECLASS;

	private:

		/*
		**	This is a record of the last message received by this receiver.
		*/
		RadioMessageType Old[3];

		/*
		**	These are the objects that radio communication has been established
		**	with, one per slot; an empty slot is NULL. There is one slot unless
		**	Set_Link_Count adds more, as a building with several docks does. Each
		**	receiving radio must also be tuned to the object that holds this set.
		*/
		std::vector<RadioClass *> Links;

		int Find_Free_Slot(void) const;

#ifdef _DEBUG
		/*
		**	This is a text representation of all the possible radio messages. This
		**	text is used for monochrome debug printing.
		*/
		static char const * Messages[RADIO_COUNT];
#endif

	public:

		/*---------------------------------------------------------------------
		**	Constructors, Destructors, and overloaded operators.
		*/
		RadioClass(void);
		virtual ~RadioClass(void) override {/*Radio=0;*/};


		virtual void Serialize(SaveStreamClass & stream) override;

		/*---------------------------------------------------------------------
		**	Member function prototypes.
		*/
		// Is any slot in contact?
		bool In_Radio_Contact(void) const;
		// The object in the first slot, which receives messages sent to no one in particular.
		TechnoClass * Contact_With_Whom(void) const {return((TechnoClass *)Links[0]);};

		int Link_Count(void) const {return((int)Links.size());}
		TechnoClass * Link(int index) const {return((TechnoClass *)Links[index]);}
		void Set_Link_Count(int count);
		int Find_Link_Index(RadioClass const * object) const;
		bool Contains_Link(RadioClass const * object) const {return(Find_Link_Index(object) != -1);}
		// Is a slot empty, or held by the object given?
		bool Has_Free_Link(RadioClass const * object = NULL) const;

		// Inherited from base class(es).
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, intptr_t & param) override;
		virtual RadioMessageType Transmit_Message(RadioMessageType message, intptr_t & param=LParam, RadioClass * to=NULL);
		virtual RadioMessageType Transmit_Message(RadioMessageType message, RadioClass * to);
#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif
		virtual bool Limbo(void) override;
};
