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
  - AICaptureNormal
  - AICaptureWounded
  - AICaptureLowPower
  - AICaptureLowMoney
  - AICaptureLowMoneyMark
  - AICaptureWoundedMark
  - MindControlDecision
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

When the warhead hits, the target [changes hands](/systems/capture/#what-changes-hands) as a captured object does and takes no damage. It leaves its team and drops its orders to guard, unless it is a harvester unloading. A unit that now belongs to a computer house then does what [the computer decides](#what-a-computer-does-with-a-unit).

[`ControlledAnimationType`](/keys/controlledanimationtype/) plays over the object while it is held, [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above its center. Over a structure it plays the art `Height` in cell levels above the center instead. [`YuriMindControlSound`](/keys/yurimindcontrolsound/) plays at the object when the firer's house or the object's former house is the player's.

A line in the firer's house color joins the firer to each object it holds. The line shows while either of them is selected, and for [`MindControlAttackLineFrames`](/keys/mindcontrolattacklineframes/) frames after each capture. It ends [`LeptonMindControlOffset`](/keys/leptonmindcontroloffset/) leptons above the object.

## Letting go

When the firer is destroyed or removed from the game, every object it holds goes back to the house it was taken from. Each one loses its ring, plays its [`MindClearedSound`](/keys/mindclearedsound/) and leaves its team, and a unit returning to a computer house does what [the computer decides](#what-a-computer-does-with-a-unit). A firer inside a transport keeps what it holds.

When the [psychic dominator](/systems/superweapons/#psychic-dominator) takes a unit under mind control, the unit is let go in the same way before it joins the dominator's house. Nothing can take it over after that.

## Going into transports and structures

An infantryman or vehicle under mind control may not:

- board a [transport](/systems/transports/);
- [garrison](/systems/garrisons/) a structure;
- enter a [tank bunker](/systems/tank-bunkers/);
- enter a [`Hospital=yes`](/keys/hospital/) or [`Armory=yes`](/keys/armory/) structure.

A player pointing such an object at one of these gets the cannot-enter cursor. Over a structure it could otherwise garrison, it gets the cursor a soldier that is not an `Occupier=yes` type would get there.

Other structures take it in under their usual conditions: a [`Grinding=yes`](/keys/grinding/) structure, an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) or [`UnitAbsorb=yes`](/keys/unitabsorb/) structure such as the Bio Reactor, and a structure an [`Engineer=yes`](/keys/engineer/#scope-infantrytype) or [`Infiltrate=yes`](/keys/infiltrate/) soldier goes into to [repair, capture or infiltrate](/systems/capture/) it.

Taking an object over gives it a new order, so one that was on its way in does not go in. A computer team that loads its transport leaves its members under mind control outside.

An object the psychic dominator took is no longer under mind control, so it may go in anywhere its house's other objects may.

## Overload

An `InfiniteMindControl=yes` weapon takes objects without limit and hurts its firer for each look at the overload table that finds damage to deal. The first look comes 30 frames after the firer is placed. At each look, the firer uses the first [`OverloadCount`](/keys/overloadcount/) entry at or above the number of objects it holds, or the last entry when none is that high. It takes the [`OverloadDamage`](/keys/overloaddamage/) entry at the same position and looks again after the [`OverloadFrames`](/keys/overloadframes/) entry at that position.

```ini title="rulesmd.ini"
[CombatDamage]
OverloadCount=3,6,10,50
OverloadDamage=0,50,100,500  ; holding up to 3: no damage; 4 to 6: 50; 7 to 10: 100; more: 500
OverloadFrames=30,60,60,60
```

The overload damage ignores armor and the Iron Curtain. Each hit throws five bursts of [`DefaultSparkSystem`](/keys/defaultsparksystem/) around the firer. The first hit plays [`MasterMindOverloadDeathSound`](/keys/mastermindoverloaddeathsound/), and it plays again only after a look that finds no damage to deal. A firer destroyed by its overload lets go of what it holds like any other.


## What a computer does with a unit

When a vehicle, infantryman or aircraft is taken over or let go and ends up owned by a computer house, the computer picks what it does next. A structure is left alone, and a unit whose firer no longer exists hunts. Otherwise the choice depends on the firer's house and on the unit:

| Condition, checked in this order | Weights used |
| --- | --- |
| The firer's house has less money than [`AICaptureLowMoneyMark`](/keys/aicapturelowmoneymark/) | [`AICaptureLowMoney`](/keys/aicapturelowmoney/) |
| The firer's house drains more power than it produces | [`AICaptureLowPower`](/keys/aicapturelowpower/) |
| The unit's health is below [`AICaptureWoundedMark`](/keys/aicapturewoundedmark/) of its maximum | [`AICaptureWounded`](/keys/aicapturewounded/) |
| Otherwise | [`AICaptureNormal`](/keys/aicapturenormal/) |

The computer rolls a number from 1 to 100 and adds the weights in order until their sum reaches the roll. The weight that reaches it picks the choice:

1. join the firer's team;
2. go to the nearest of the owner's `Grinding=yes` structures;
3. go to the nearest of the owner's `InfantryAbsorb=yes` or `UnitAbsorb=yes` structures that would take it in;
4. hunt;
5. do nothing.

If the weights add up to less than the roll, or the roll is not reached within six weights, the unit is left as it is. Otherwise, if the firer is a member of a team whose [`MindControlDecision`](/keys/mindcontroldecision/) is not `0`, that value replaces the choice. The unit joins the firer's team only when it now belongs to the firer's house and the firer is in a team. When the choice cannot be carried out, or is any value other than 1 to 5, the unit hunts.

```ini title="rulesmd.ini"
[General]
AICaptureNormal=75,5,5,15     ; mostly join the team
AICaptureWounded=15,40,40,5   ; mostly grinder or bio reactor
AICaptureLowPower=15,5,75,5   ; mostly bio reactor
AICaptureLowMoney=15,75,5,5   ; mostly grinder
AICaptureLowMoneyMark=2000
AICaptureWoundedMark=.25
```
