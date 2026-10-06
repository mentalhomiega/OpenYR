/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <vector>


/*
 * The slot rules of a radio set with several contacts, such as a building with several docks.
 * Each slot holds one contact or is empty (NULL). RadioClass keeps the slots and sends the
 * messages; these functions only read and change the slot list, so a test can run them on
 * their own.
 */
namespace RadioSlots
{
	// Grows the list to count slots with the new ones empty; a smaller count changes nothing.
	template<typename T>
	void Grow(std::vector<T *> & links, int count)
	{
		if (count > (int)links.size()) {
			links.resize(count, nullptr);
		}
	}

	// The first slot the object holds, or -1 when it holds none or is NULL.
	template<typename T>
	int Find(std::vector<T *> const & links, T const * object)
	{
		if (object != nullptr) {
			for (std::size_t index = 0; index < links.size(); index++) {
				if (links[index] == object) {
					return((int)index);
				}
			}
		}
		return(-1);
	}

	// The first empty slot, or -1 when every slot is held.
	template<typename T>
	int Find_Free(std::vector<T *> const & links)
	{
		for (std::size_t index = 0; index < links.size(); index++) {
			if (links[index] == nullptr) {
				return((int)index);
			}
		}
		return(-1);
	}

	// Is a slot empty, or held by the object given?
	template<typename T>
	bool Has_Free(std::vector<T *> const & links, T const * object = nullptr)
	{
		for (T const * link : links) {
			if (link == nullptr || (object != nullptr && link == object)) {
				return(true);
			}
		}
		return(false);
	}

	// The slot a received HELLO from the object takes: the one it already holds, else the first
	// empty one, else -1.
	template<typename T>
	int Hello_Slot(std::vector<T *> const & links, T const * object)
	{
		int const slot = Find(links, object);
		return(slot != -1 ? slot : Find_Free(links));
	}

	// Empties every slot the object holds.
	template<typename T>
	void Release(std::vector<T *> & links, T const * object)
	{
		for (T * & link : links) {
			if (link == object) {
				link = nullptr;
			}
		}
	}
}
