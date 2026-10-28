/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "always.h"

#include "statehash.h"

#include "aircraft.h"
#include "building.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "scenario.h"
#include "unit.h"


namespace {

constexpr unsigned int FNV_BASIS = 2166136261u;
constexpr unsigned int FNV_PRIME = 16777619u;


void Hash_Value(unsigned int & hash, unsigned int value)
{
	for (int byte = 0; byte < 4; byte++) {
		hash = (hash ^ ((value >> (byte * 8)) & 0xFF)) * FNV_PRIME;
	}
}


template<class T>
void Hash_Technos(unsigned int & hash, DynamicVectorClass<T *> & list)
{
	Hash_Value(hash, list.Count());
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		Hash_Value(hash, object->PositionCoord.X);
		Hash_Value(hash, object->PositionCoord.Y);
		Hash_Value(hash, object->PositionCoord.Z);
		Hash_Value(hash, object->PrimaryFacing.Current().As_Int());
		Hash_Value(hash, object->Strength);
		Hash_Value(hash, object->Get_Mission());
		Hash_Value(hash, object->IsInLimbo);
		Hash_Value(hash, Houses.ID(object->House));
	}
}


template<class T>
void Hash_Targets(unsigned int & hash, DynamicVectorClass<T *> & list)
{
	for (int index = 0; index < list.Count(); index++) {
		AbstractClass const * target = list[index]->TarCom;
		Hash_Value(hash, target != NULL ? target->What_Am_I() : -1);
		Hash_Value(hash, target != NULL ? target->Fetch_ID() : -1);
	}
}

}


/// <summary>
/// Hashes the game state that decides play. Reading the state does not change it.
/// </summary>
SimStateHash Compute_Sim_State_Hash(void)
{
	SimStateHash result;

	unsigned int hash = FNV_BASIS;
	Hash_Technos(hash, Buildings);
	Hash_Technos(hash, Units);
	Hash_Technos(hash, Aircraft);
	Hash_Technos(hash, Infantry);
	for (int index = 0; index < Houses.Count(); index++) {
		Hash_Value(hash, Houses[index]->Credits);
	}
	unsigned char const * random = reinterpret_cast<unsigned char const *>(&Scen->RandomNumber);
	for (size_t index = 0; index < sizeof(Scen->RandomNumber); index++) {
		hash = (hash ^ random[index]) * FNV_PRIME;
	}
	result.State = hash;

	unsigned int targets = FNV_BASIS;
	Hash_Targets(targets, Buildings);
	Hash_Targets(targets, Units);
	Hash_Targets(targets, Aircraft);
	Hash_Targets(targets, Infantry);
	result.Targets = targets;

	return(result);
}
