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

/* $Header: /CounterStrike/RADIO.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : RADIO.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : September 10, 1993                                           *
 *                                                                                             *
 *                  Last Update : June 5, 1996 [JLB]                                           *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   RadioClass::Debug_Dump -- Displays the current status of the radio to the mono monitor.   *
 *   RadioClass::Limbo -- When limboing a unit will always break radio contact.                *
 *   RadioClass::Receive_Message -- Handles receipt of a radio message.                        *
 *   RadioClass::Transmit_Message -- Transmit message from one object to another.              *
 *   RadioClass::Transmit_Message -- Transmits a message to the object specified.              *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "radio.h"

#include "_rtti.h"
#include "house.h"
#include "mono.h"
#include "savestream.h"
#include "swizzle.h"
#include "techno.h"

#ifdef _DEBUG
/*
**	These are the text representations of the radio messages that can be transmitted.
*/
char const * RadioClass::Messages[RADIO_COUNT] = {
	"static (no message)",
	"Roger.",
	"Come in.",
	"Over and out.",
	"Requesting transport.",
	"Attach to transport.",
	"I've got a delivery for you.",
	"I'm performing load/unload maneuver. Be careful.",
	"I'm clear.",
	"You are clear to unload. Driving away now.",
	"Am unable to comply.",
	"I'm starting construction now... act busy.",
	"I've finished construction. You are free.",
	"We bumped, redraw yourself please.",
	"I'm trying to load up now.",
	"May I become a passenger?",
	"Are you ready to receive shipment?",
	"Are you trying to become a passenger?",
	"Move to location X.",
	"Do you need to move?",
	"All right already. Now what?",
	"I'm a passenger now.",
	"Backup into refinery now.",
	"Run away!",
	"Tether established.",
	"Tether broken.",
	"Repair one step.",
	"Are you prepared to fight?",
	"Attack this target please.",
	"Reload one step.",
	"Circumstances prevent success.",
	"All done with the request.",
	"Do you need service depot work?",
	"Are you sitting on service depot?"
};
#endif


/// <summary>
/// Creates a radio set that is not in contact with anyone.
/// The message history is cleared as well, so the first message this object receives will
/// always be treated as a new one rather than a repeat.
/// </summary>
RadioClass::RadioClass(void) :
	BASECLASS(),
	Links(1, NULL)
{
	for (int i = 0; i < 3; i++) {
		Old[i] = RADIO_STATIC;
	}
};


#ifdef _DEBUG
/***********************************************************************************************
 * RadioClass::Debug_Dump -- Displays the current status of the radio to the mono monitor.     *
 *                                                                                             *
 *    This displays the radio connection value to the monochrome monitor.                      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/02/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void RadioClass::Debug_Dump(MonoClass * mono) const
{
	mono->Set_Cursor(29, 7);mono->Printf("0-%-47s", Messages[Old[0]]);
	mono->Set_Cursor(29, 8);mono->Printf("1-%-47s", Messages[Old[1]]);
	mono->Set_Cursor(29, 9);mono->Printf("2-%-47s", Messages[Old[2]]);
	if (Links[0] != NULL) {
		mono->Set_Cursor(20, 7);mono->Printf("%08X", Links[0]);
	}
	BASECLASS::Debug_Dump(mono);
}
#endif


/***********************************************************************************************
 * RadioClass::Receive_Message -- Handles receipt of a radio message.                          *
 *                                                                                             *
 *    This is the base version of what should happen when a radio message is received. It      *
 *    turns the radio off when the "OVER_OUT" message is received. All other messages are      *
 *    merely acknowledged with a "ROGER".                                                      *
 *                                                                                             *
 * INPUT:   from     -- The object that is initiating this radio message (always valid).       *
 *                                                                                             *
 *          message  -- The radio message received.                                            *
 *                                                                                             *
 *          param    -- Reference to optional value that might be used to return more          *
 *                      information than can be conveyed in the simple radio response          *
 *                      messages.                                                              *
 *                                                                                             *
 * OUTPUT:  Returns with the response radio message.                                           *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/28/1994 JLB : Created.                                                                 *
 *   09/24/1994 JLB : Streamlined to be only a communications carrier.                         *
 *   05/22/1995 JLB : Recognized who is sending the message                                    *
 *   06/05/1996 JLB : Radio message history tracking.                                          *
 *=============================================================================================*/
RadioMessageType RadioClass::Receive_Message(RadioClass * from, RadioMessageType message, intptr_t & param)
{
	/*
	**	Keep a record of the last message received by this radio.
	*/
	if (message != Old[0]) {
		Old[2] = Old[1];
		Old[1] = Old[0];
		Old[0] = message;
	}

	/*
	**	When this message is received, it means that the other object
	**	has already turned its radio off. Free the slot it held. This
	**	only applies if the sender holds a slot in this radio.
	*/
	if (message == RADIO_OVER_OUT) {
		int const slot = Find_Link_Index(from);
		if (slot != -1) {
			BASECLASS::Receive_Message(from, message, param);
			Links[slot] = NULL;
			return(RADIO_ROGER);
		}
	}

	/*
	**	The "hello" message is an attempt to establish contact. The sender keeps
	**	the slot it already holds or takes the first empty one; with every slot
	**	held by others, the answer is "negative".
	*/
	if (message == RADIO_HELLO && Strength) {
		if (from != NULL && ((TechnoClass *)from)->House->Is_Ally(this) && Is_Techno() && ((TechnoClass *)this)->House->Is_Ally(from)) {
			int slot = Find_Link_Index(from);
			if (slot == -1) {
				slot = Find_Free_Slot();
			}
			if (slot != -1) {
				Links[slot] = from;
				return(RADIO_ROGER);
			}
		}
		return(RADIO_NEGATIVE);
	}

	return(BASECLASS::Receive_Message(from, message, param));
}


/***********************************************************************************************
 * RadioClass::Transmit_Message -- Transmit message from one object to another.                *
 *                                                                                             *
 *    This routine is used to transmit a radio message from this object to another. Most       *
 *    inter object coordination is handled through this mechanism.                             *
 *                                                                                             *
 * INPUT:   to       -- Pointer to the object that will receive the radio message.             *
 *                                                                                             *
 *          message  -- The message itself (see RadioType).                                    *
 *                                                                                             *
 *          param    -- Optional reference to parameter that might be used to pass or          *
 *                      receive additional information.                                        *
 *                                                                                             *
 * OUTPUT:  Returns with the response radio message from the receiving object.                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/22/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, intptr_t & param, RadioClass * to)
{
	if (to == NULL) {
		to = (RadioClass *)Contact_With_Whom();
	}

	/*
	**	If there is no target for the radio message, then always return static.
	*/
	if (to == NULL) return(RADIO_STATIC);

	/*
	**	Handle some special case processing that occurs when certain messages
	**	are transmitted.
	*/
	if (message == RADIO_OVER_OUT) {
		for (RadioClass * & link : Links) {
			if (link == to) {
				link = NULL;
			}
		}
	}

	/*
	**	Contact with an object that already holds a slot needs no message. Otherwise
	**	the object goes in the first empty slot; with none empty, contact with the
	**	first slot's object is broken off to make room (RadioClass::SendCommandWithData,
	**	0x65A970).
	*/
	if (message == RADIO_HELLO) {
		if (Contains_Link(to)) {
			return(RADIO_ROGER);
		}
		int slot = Find_Free_Slot();
		if (slot == -1) {
			Transmit_Message(RADIO_OVER_OUT, Links[0]);
			slot = 0;
		}
		if (to->Receive_Message(Dynamic_Cast<TechnoClass *>(this), message, param) == RADIO_ROGER) {
			Links[slot] = to;
			return(RADIO_ROGER);
		}
		return(RADIO_NEGATIVE);
	}

	return(to->Receive_Message(Dynamic_Cast<TechnoClass *>(this), message, param));
}


/***********************************************************************************************
 * RadioClass::Limbo -- When limboing a unit will always break radio contact.                  *
 *                                                                                             *
 *    This routine will break radio contact as the object is entering limbo state.             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Was the object successfully limboed?                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/25/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool RadioClass::Limbo(void)
{
	if (!IsInLimbo) {
		for (size_t index = 0; index < Links.size(); index++) {
			if (Links[index] != NULL) {
				Transmit_Message(RADIO_OVER_OUT, Links[index]);
			}
		}
	}
	return(BASECLASS::Limbo());
}


/***********************************************************************************************
 * RadioClass::Transmit_Message -- Transmits a message to the object specified.                *
 *                                                                                             *
 *    This routine will transmit the specified message to the object. This routine differs     *
 *    from the normal Transmit_Message in that the LParam value is "faked" into the            *
 *    parameter list. It is presumed that the message sent with this function does not         *
 *    require the LParam.                                                                      *
 *                                                                                             *
 * INPUT:   message  -- The message to transmit.                                               *
 *                                                                                             *
 *          to       -- The requested receiver of this message.                                *
 *                                                                                             *
 * OUTPUT:  Returns with the radio response from the receiver.                                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/21/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, RadioClass * to)
{
	return(Transmit_Message(message, LParam, to));
}


/// <summary>
/// Removes any reference to the specified object.
/// This routine is called when an object is about to be destroyed or removed from the
/// game. Any radio contact with the doomed object must be broken here, since a stale
/// contact would leave this object talking to freed memory.
/// </summary>
/// <param name="target">Pointer to the object that is going away.</param>
/// <param name="all">Should even a casual reference to the target be severed?</param>
void RadioClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (all) {
		for (RadioClass * & link : Links) {
			if (link == target) {
				link = NULL;
			}
		}
	}
}


bool RadioClass::In_Radio_Contact(void) const
{
	for (RadioClass const * link : Links) {
		if (link != NULL) {
			return(true);
		}
	}
	return(false);
}


/// <summary>
/// Grows the number of radio slots to the count given, leaving the new slots empty. A smaller
/// count changes nothing (RadioClass::SetLinkCount, 0x65AE60).
/// </summary>
void RadioClass::Set_Link_Count(int count)
{
	if (count > (int)Links.size()) {
		Links.resize(count, NULL);
	}
}


/// <summary>
/// Returns the slot the object holds, or -1 when it holds none or is NULL.
/// </summary>
int RadioClass::Find_Link_Index(RadioClass const * object) const
{
	if (object != NULL) {
		for (size_t index = 0; index < Links.size(); index++) {
			if (Links[index] == object) {
				return((int)index);
			}
		}
	}
	return(-1);
}


int RadioClass::Find_Free_Slot(void) const
{
	for (size_t index = 0; index < Links.size(); index++) {
		if (Links[index] == NULL) {
			return((int)index);
		}
	}
	return(-1);
}


bool RadioClass::Has_Free_Link(RadioClass const * object) const
{
	for (RadioClass const * link : Links) {
		if (link == NULL || (object != NULL && link == object)) {
			return(true);
		}
	}
	return(false);
}


/// <summary>
/// Adds this radio set to the game state checksum.
/// This routine is used by the multiplayer synchronization checker. The identity of the
/// object this radio is tuned to contributes to the checksum, so a desynchronized radio
/// contact will be caught.
/// </summary>
void RadioClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((int)Links.size());
	for (RadioClass const * link : Links) {
		if (link != NULL) {
			crc(link->Fetch_ID());
			crc((RTTIType)link->RTTI);
		}
	}
}


/// <summary>
/// Lists the members this radio set carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void RadioClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(Old);
	stream.Serialize(Links);
}
