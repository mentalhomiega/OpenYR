/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the radio slot rules to the behaviour RadioClass relies on for buildings with several
// docks: growing the slot list, finding a contact, the free-slot test, the slot a HELLO takes,
// and the slots an OVER_OUT empties.
//
// Needs no game data.

#include "radioslots.h"

#include <cstdio>
#include <vector>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


// Stands in for a radio; the slot rules compare its address and nothing else.
struct Contact {
	int Number;
};

Contact A{1};
Contact B{2};
Contact C{3};


void Test_Grow(void)
{
	std::vector<Contact *> links(1, nullptr);
	links[0] = &A;

	RadioSlots::Grow(links, 3);
	Check(links.size() == 3, "growing adds slots up to the count");
	Check(links[0] == &A && links[1] == nullptr && links[2] == nullptr, "growing keeps the held slot and leaves the new ones empty");

	RadioSlots::Grow(links, 1);
	Check(links.size() == 3 && links[0] == &A, "a smaller count changes nothing");

	RadioSlots::Grow(links, 0);
	RadioSlots::Grow(links, -4);
	Check(links.size() == 3, "zero and negative counts change nothing");
}


void Test_Find(void)
{
	std::vector<Contact *> links = {nullptr, &B, &A, &B};

	Check(RadioSlots::Find(links, &A) == 2, "a contact is found in its slot");
	Check(RadioSlots::Find(links, &B) == 1, "a contact in two slots is found in the first");
	Check(RadioSlots::Find(links, &C) == -1, "a contact that holds no slot is not found");
	Check(RadioSlots::Find(links, (Contact const *)nullptr) == -1, "NULL is never found, even beside an empty slot");

	std::vector<Contact *> empty;
	Check(RadioSlots::Find(empty, &A) == -1 && RadioSlots::Find_Free(empty) == -1, "a list with no slots finds nothing");
}


void Test_Free(void)
{
	std::vector<Contact *> links = {&A, nullptr, &B, nullptr};
	Check(RadioSlots::Find_Free(links) == 1, "the first empty slot is the free one");
	Check(RadioSlots::Has_Free(links), "a list with an empty slot has room");

	std::vector<Contact *> full = {&A, &B};
	Check(RadioSlots::Find_Free(full) == -1, "a full list has no free slot");
	Check(!RadioSlots::Has_Free(full), "a full list has no room for anyone");
	Check(!RadioSlots::Has_Free(full, (Contact const *)nullptr), "naming NULL does not make room");
	Check(RadioSlots::Has_Free(full, &B), "a full list has room for a contact already in it");
	Check(!RadioSlots::Has_Free(full, &C), "a full list has no room for a new contact");
}


void Test_Hello(void)
{
	std::vector<Contact *> links = {nullptr, &A, nullptr};
	Check(RadioSlots::Hello_Slot(links, &A) == 1, "a HELLO from a contact keeps the slot it holds");
	Check(RadioSlots::Hello_Slot(links, &B) == 0, "a HELLO from a new contact takes the first empty slot");

	links[0] = &B;
	links[2] = &C;
	Check(RadioSlots::Hello_Slot(links, &C) == 2, "a HELLO from a contact keeps its slot when every slot is held");
	Contact D{4};
	Check(RadioSlots::Hello_Slot(links, &D) == -1, "a HELLO from a new contact finds no slot when every slot is held");

	std::vector<Contact *> one(1, nullptr);
	Check(RadioSlots::Hello_Slot(one, &A) == 0, "a single-slot radio gives its only slot to the first caller");
	one[0] = &A;
	Check(RadioSlots::Hello_Slot(one, &B) == -1, "a held single-slot radio refuses a second caller");
}


void Test_Release(void)
{
	std::vector<Contact *> links = {&A, &B, &A, nullptr};
	RadioSlots::Release(links, &A);
	Check(links[0] == nullptr && links[2] == nullptr, "an OVER_OUT empties every slot the contact holds");
	Check(links[1] == &B && links.size() == 4, "an OVER_OUT keeps the other contacts and the slot count");

	RadioSlots::Release(links, &C);
	Check(links[1] == &B, "releasing a contact that holds no slot changes nothing");

	RadioSlots::Release(links, &B);
	Check(RadioSlots::Find_Free(links) == 0 && RadioSlots::Has_Free(links), "a released radio has room again");
}

}


int main(void)
{
	Test_Grow();
	Test_Find();
	Test_Free();
	Test_Hello();
	Test_Release();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
