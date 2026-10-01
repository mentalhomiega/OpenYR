---
title: Mind control
summary: "How a mind control weapon takes enemy objects over, holds them, lets them go, and overloads its firer."
category: combat-targeting
keys:
  - MindControl
  - InfiniteMindControl
  - ControlledAnimationType
  - MindControlRingOffset
  - LeptonMindControlOffset
  - YuriMindControlSound
  - MindClearedSound
  - MindControlAttackLineFrames
  - OverloadCount
  - OverloadDamage
  - OverloadFrames
  - MasterMindOverloadDeathSound
  - ImmuneToPsionics
related:
  - type: system
    id: capture
  - type: system
    id: superweapons
---

An object whose primary weapon has a [`MindControl=yes`](/keys/mindcontrol/) warhead takes over what that weapon hits instead of damaging it. It holds at most the weapon's [`Damage`](/keys/damage/#scope-weapontype) in objects, unless the weapon is [`InfiniteMindControl=yes`](/keys/infinitemindcontrol/). The object gets this ability when it is placed on the map, from the primary weapon it has at that moment.

```ini title="rulesmd.ini"
[YURI]
Primary=MindControl

[MindControl]
Damage=1             ; holds one object at a time
Warhead=Controller

[Controller]
MindControl=yes
```

## Taking an object over

The weapon fires only at an object it could take. It refuses:

- an object of the firer's own house;
- an [`ImmuneToPsionics=yes`](/keys/immunetopsionics/) type;
- an object already under mind control, or one the [psychic dominator](/systems/superweapons/#psychic-dominator) has taken;
- an object under the Iron Curtain or a force shield;
- a structure being built or sold;
- any object while the firer already holds its weapon's `Damage` in objects. A weapon with `Damage=1` is the exception: its firer lets go of the object it holds and takes the new one.

When the warhead hits, the target [changes hands](/systems/capture/#what-changes-hands) as a captured object does and takes no damage. It leaves its team and drops its orders to guard, unless it is a harvester unloading. A unit that now belongs to a computer house hunts.

[`ControlledAnimationType`](/keys/controlledanimationtype/) plays over the object while it is held, [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above its center. Over a structure it plays the art `Height` in cell levels above the center instead. [`YuriMindControlSound`](/keys/yurimindcontrolsound/) plays at the object when the firer's house or the object's former house is the player's.

A line in the firer's house color joins the firer to each object it holds. The line shows while either of them is selected, and for [`MindControlAttackLineFrames`](/keys/mindcontrolattacklineframes/) frames after each capture. It ends [`LeptonMindControlOffset`](/keys/leptonmindcontroloffset/) leptons above the object.

## Letting go

When the firer is destroyed or removed from the game, every object it holds goes back to the house it was taken from. Each one loses its ring, plays its [`MindClearedSound`](/keys/mindclearedsound/) and leaves its team, and a unit returning to a computer house hunts. A firer inside a transport keeps what it holds.

When the [psychic dominator](/systems/superweapons/#psychic-dominator) takes a unit under mind control, the unit is let go in the same way before it joins the dominator's house. Nothing can take it over after that.

## Overload

An `InfiniteMindControl=yes` weapon takes objects without limit and hurts its firer for each look at the overload table that finds damage to deal. The first look comes 30 frames after the firer is placed. At each look, the firer uses the first [`OverloadCount`](/keys/overloadcount/) entry at or above the number of objects it holds, or the last entry when none is that high. It takes the [`OverloadDamage`](/keys/overloaddamage/) entry at the same position and looks again after the [`OverloadFrames`](/keys/overloadframes/) entry at that position.

```ini title="rulesmd.ini"
[CombatDamage]
OverloadCount=3,6,10,50
OverloadDamage=0,50,100,500  ; holding up to 3: no damage; 4 to 6: 50; 7 to 10: 100; more: 500
OverloadFrames=30,60,60,60
```

The overload damage ignores armor and the Iron Curtain. Each hit throws five bursts of [`DefaultSparkSystem`](/keys/defaultsparksystem/) around the firer. The first hit plays [`MasterMindOverloadDeathSound`](/keys/mastermindoverloaddeathsound/), and it plays again only after a look that finds no damage to deal. A firer destroyed by its overload lets go of what it holds like any other.

Yuri's Revenge also shows a special cursor over a target the weapon could take, which is not shown yet. A computer house sends what it takes to hunt; Yuri's Revenge can instead send it to the house's grinder or bio reactor, which is not done yet.
