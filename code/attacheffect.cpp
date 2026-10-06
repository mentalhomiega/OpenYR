/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "attacheffect.h"

#include "anim.h"
#include "animtype.h"
#include "crc.h"
#include "dbgprint.h"
#include "savestream.h"
#include "techno.h"
#include "techtype.h"
#include "warhead.h"

#include <algorithm>
#include <climits>


void AttachEffectTypeClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Animation);
	stream.Serialize(Duration);
	stream.Serialize(IsTemporalHidesAnim);
	stream.Serialize(SpeedMultiplier);
	stream.Serialize(ArmorMultiplier);
	stream.Serialize(FirepowerMultiplier);
	stream.Serialize(ROFMultiplier);
	stream.Serialize(IsCloakable);
	stream.Serialize(IsForceDecloak);
	stream.Serialize(IsDiscardOnEntry);
	stream.Serialize(IsPenetratesIronCurtain);
	stream.Serialize(Delay);
	stream.Serialize(InitialDelay);
	stream.Serialize(IsCumulative);
	stream.Serialize(IsAnimResetOnReapply);
}


void AttachEffectTypeClass::Compute_CRC(CRCEngine & crc) const
{
	crc(Duration);
	crc(SpeedMultiplier);
	crc(ArmorMultiplier);
	crc(FirepowerMultiplier);
	crc(ROFMultiplier);
	crc(IsCloakable);
	crc(IsForceDecloak);
	crc(IsDiscardOnEntry);
	crc(IsPenetratesIronCurtain);
	crc(Delay);
	crc(InitialDelay);
	crc(IsCumulative);
}


AttachEffectTypeClass const & AttachedEffectsClass::EffectStruct::Type(void) const
{
	return(Warhead != nullptr ? Warhead->AttachEffect : Techno->AttachEffect);
}


void AttachedEffectsClass::EffectStruct::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Warhead);
	stream.Serialize(Techno);
	stream.Serialize(Remaining);
	stream.Serialize(Anim);
}


/// <summary>
/// Attaches the warhead's effect to the object, or restarts the duration of the one it already
/// has from that warhead unless the warhead is AttachEffect.Cumulative (Ares).
/// </summary>
void AttachedEffectsClass::Attach(TechnoClass * owner, WarheadTypeClass const * warhead)
{
	AttachEffectTypeClass const & type = warhead->AttachEffect;
	if (!type.Is_Defined()) {
		return;
	}

	EffectStruct * effect = nullptr;
	if (!type.IsCumulative) {
		for (EffectStruct & existing : Effects) {
			if (existing.Warhead == warhead) {
				effect = &existing;
				break;
			}
		}
	}

	if (effect != nullptr) {
		effect->Remaining = type.Duration;
		if (type.IsAnimResetOnReapply) {
			Remove_Anim(*effect);
		}
	} else {
		EffectStruct added;
		added.Warhead = warhead;
		added.Remaining = type.Duration;
		Effects.push_back(added);
		effect = &Effects.back();
	}

	if (type.IsForceDecloak && owner->Cloak != UNCLOAKED) {
		owner->Do_Uncloak();
	}
	Update_Anim(owner, *effect);
}


/// <summary>
/// Counts down every effect, ends those that ran out, and attaches the effect of the object's own
/// type when its delay has passed. Nothing counts down while the object is off the map or being
/// warped out by a temporal weapon.
/// </summary>
void AttachedEffectsClass::AI(TechnoClass * owner)
{
	if (owner->Strength <= 0) {
		Clear();
		return;
	}
	if (owner->IsInLimbo || owner->IsBeingWarpedOut) {
		for (EffectStruct & effect : Effects) {
			Update_Anim(owner, effect);
		}
		return;
	}

	TechnoTypeClass const * own = owner->TClass;
	for (size_t index = 0; index < Effects.size();) {
		EffectStruct & effect = Effects[index];
		if (effect.Remaining > 0 && --effect.Remaining == 0) {
			if (effect.Warhead == nullptr && effect.Techno == own) {
				SelfDelay = own->AttachEffect.Delay < 0 ? -1 : own->AttachEffect.Delay;
			}
			Remove_Anim(effect);
			Effects.erase(Effects.begin() + index);
			continue;
		}
		Update_Anim(owner, effect);
		index++;
	}

	AttachEffectTypeClass const & type = own->AttachEffect;
	if (type.Is_Defined()) {
		if (!IsSelfStarted) {
			IsSelfStarted = true;
			SelfDelay = std::max(type.InitialDelay, 0);
		}
		bool const active = std::any_of(Effects.begin(), Effects.end(), [own](EffectStruct const & effect) {
			return(effect.Warhead == nullptr && effect.Techno == own);
		});
		if (!active && SelfDelay >= 0) {
			if (SelfDelay > 0) {
				SelfDelay--;
			} else if (!owner->Is_Iron_Curtained() || type.IsPenetratesIronCurtain) {
				EffectStruct added;
				added.Techno = own;
				added.Remaining = type.Duration;
				Effects.push_back(added);
				if (type.IsForceDecloak && owner->Cloak != UNCLOAKED) {
					owner->Do_Uncloak();
				}
				Update_Anim(owner, Effects.back());
			}
		}
	}
}


/*
 * Removes the animations of an object leaving the map, and the effects that end on entry. A
 * discarded effect of the object's own type starts its AttachEffect.Delay as if it had run out.
 */
void AttachedEffectsClass::Limbo(TechnoClass * owner)
{
	for (EffectStruct & effect : Effects) {
		Remove_Anim(effect);
		if (effect.Warhead == nullptr && effect.Techno == owner->TClass && effect.Type().IsDiscardOnEntry) {
			SelfDelay = effect.Techno->AttachEffect.Delay < 0 ? -1 : effect.Techno->AttachEffect.Delay;
		}
	}
	std::erase_if(Effects, [](EffectStruct const & effect) { return(effect.Type().IsDiscardOnEntry); });
}


void AttachedEffectsClass::Clear(void)
{
	for (EffectStruct & effect : Effects) {
		Remove_Anim(effect);
	}
	Effects.clear();
}


void AttachedEffectsClass::Detach(AbstractClass const * target)
{
	for (EffectStruct & effect : Effects) {
		if (effect.Anim == target) {
			effect.Anim = nullptr;
		}
	}
}


void AttachedEffectsClass::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Effects);
	stream.Serialize(SelfDelay);
	stream.Serialize(IsSelfStarted);
}


void AttachedEffectsClass::Compute_CRC(CRCEngine & crc) const
{
	crc(int(Effects.size()));
	for (EffectStruct const & effect : Effects) {
		crc(effect.Remaining);
		crc(effect.Warhead != nullptr);
	}
	crc(SelfDelay);
	crc(IsSelfStarted);
}


double AttachedEffectsClass::Speed_Multiplier(void) const
{
	double value = 1.0;
	for (EffectStruct const & effect : Effects) {
		value *= effect.Type().SpeedMultiplier;
	}
	return(value);
}


double AttachedEffectsClass::Armor_Multiplier(void) const
{
	double value = 1.0;
	for (EffectStruct const & effect : Effects) {
		value *= effect.Type().ArmorMultiplier;
	}
	return(value);
}


double AttachedEffectsClass::Firepower_Multiplier(void) const
{
	double value = 1.0;
	for (EffectStruct const & effect : Effects) {
		value *= effect.Type().FirepowerMultiplier;
	}
	return(value);
}


double AttachedEffectsClass::ROF_Multiplier(void) const
{
	double value = 1.0;
	for (EffectStruct const & effect : Effects) {
		value *= effect.Type().ROFMultiplier;
	}
	return(value);
}


bool AttachedEffectsClass::Is_Cloakable(void) const
{
	return(std::any_of(Effects.begin(), Effects.end(), [](EffectStruct const & effect) { return(effect.Type().IsCloakable); }));
}


/*
 * The animation shows while the object is on the map and uncloaked, and, for a
 * TemporalHidesAnim effect, not being warped out. It loops until the effect ends.
 */
void AttachedEffectsClass::Update_Anim(TechnoClass * owner, EffectStruct & effect)
{
	AttachEffectTypeClass const & type = effect.Type();
	bool const show = type.Animation != nullptr && !owner->IsInLimbo && owner->Cloak == UNCLOAKED
		&& !(type.IsTemporalHidesAnim && owner->IsBeingWarpedOut);

	if (show && effect.Anim == nullptr) {
		effect.Anim = new AnimClass(type.Animation, owner->Center_Coord());
		if (effect.Anim != nullptr) {
			effect.Anim->Attach_To(owner);
			effect.Anim->Loops = UCHAR_MAX;
		}
	} else if (!show) {
		Remove_Anim(effect);
	}
}


void AttachedEffectsClass::Remove_Anim(EffectStruct & effect)
{
	if (effect.Anim != nullptr) {
		effect.Anim->Delete_Me();
		effect.Anim = nullptr;
	}
}
