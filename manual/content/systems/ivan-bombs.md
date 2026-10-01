---
title: Ivan bombs
summary: "How a soldier's IvanBomb weapon fixes a time bomb to its target and when the bomb goes off."
category: combat-targeting
keys:
  - IvanBomb
  - IvanTimedDelay
  - IvanDamage
  - IvanWarhead
  - IvanIconFlickerRate
  - BombTickingSound
  - BombAttachSound
  - Ivan
  - Bombable
  - BombDisarm
  - BombSight
  - CanDetonateTimeBomb
related:
  - type: system
    id: capture
---

A soldier whose weapon has an [`IvanBomb=yes`](/keys/ivanbomb/) warhead fixes a time bomb to what it hits instead of damaging it. Only infantry plant bombs, and an object carries one bomb at a time; an Ivan bomb weapon does not fire at an object that already has one.

```ini title="rulesmd.ini"
[MyIvanBomber] ; example Weapon
Warhead=MyIvanBomb

[MyIvanBomb]
IvanBomb=yes

[CombatDamage]
IvanTimedDelay=450   ; the bomb goes off 450 frames after it is planted
IvanDamage=450
IvanWarhead=MyIvanWH
```

## The countdown

The bomb goes off [`IvanTimedDelay`](/keys/ivantimeddelay/) frames after it is planted. [`BombAttachSound`](/keys/bombattachsound/) plays as it is planted, and [`BombTickingSound`](/keys/bombtickingsound/) ticks at the object until it goes off, when the planter is the player's. Over an object carrying a bomb the player [can see](#seeing-and-disarming-bombs), the `BOMBCURS.SHP` icon counts down in six steps, flickering every [`IvanIconFlickerRate`](/keys/ivaniconflickerrate/) frames.

## Going off

The bomb does [`IvanDamage`](/keys/ivandamage/) through [`IvanWarhead`](/keys/ivanwarhead/) where the object stands, with the warhead's explosion, credited to the soldier that planted it. A bomb whose object is off the map when the time comes, such as inside a transport, waits and goes off as soon as the object is back on the map. A bomb whose object is destroyed or removed first goes with it.

## Ordering a bomb

When the player points an [`Ivan=yes`](/keys/ivan/) soldier at something it would attack, the cursor shows the bomb if the target's type is [`Bombable=yes`](/keys/bombable/) and the target carries no bomb yet. Otherwise it shows the no-move cursor, and clicking gives no order. The key only limits the player's orders: a soldier that picks a target on its own still plants a bomb on an object whose type sets `Bombable=no`.

## Seeing and disarming bombs

The player sees a bomb that the player's house planted, and any bomb within [`BombSight`](/keys/bombsight/) cells of one of the player's objects.

When the player points an [`Engineer=yes`](/keys/engineer/#scope-infantrytype) soldier at an object carrying a bomb the player sees, the cursor shows the disarm cursor. The click orders the soldier to attack the object, so it needs a weapon whose warhead is [`BombDisarm=yes`](/keys/bombdisarm/). That warhead removes the bomb from what it hits instead of damaging it, and a weapon with it fires only at an object carrying a bomb. A soldier still attacking the object can bomb it again.

```ini title="rulesmd.ini"
[ENGINEER]
Primary=DefuseKit
BombSight=4

[DefuseKit]
Range=1.5
Warhead=BombDisarm
FireOnce=yes

[BombDisarm]
BombDisarm=yes
```

## Setting a bomb off early

With [`CanDetonateTimeBomb=yes`](/keys/candetonatetimebomb/), the player sets a bomb off at once by selecting only the vehicle or soldier carrying it and clicking it; the cursor shows the demolition cursor. Only the house that planted the bomb can do this, so the player first has to bomb one of their own units. Yuri's Revenge ships with `CanDetonateTimeBomb=no`.

Yuri's Revenge also plants death bombs and blows up a bridge a bomb is fixed to. Neither is done yet.
