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

The bomb goes off [`IvanTimedDelay`](/keys/ivantimeddelay/) frames after it is planted. [`BombAttachSound`](/keys/bombattachsound/) plays as it is planted, and [`BombTickingSound`](/keys/bombtickingsound/) ticks at the object until it goes off, when the planter is the player's. Over an object carrying one of the player's bombs, the `BOMBCURS.SHP` icon counts down in six steps, flickering every [`IvanIconFlickerRate`](/keys/ivaniconflickerrate/) frames.

## Going off

The bomb does [`IvanDamage`](/keys/ivandamage/) through [`IvanWarhead`](/keys/ivanwarhead/) where the object stands, with the warhead's explosion, credited to the soldier that planted it. A bomb whose object is off the map when the time comes, such as inside a transport, waits and goes off as soon as the object is back on the map. A bomb whose object is destroyed or removed first goes with it.

Yuri's Revenge also lets the player set a bomb off early, plants death bombs, lets other houses' bomb detectors see bombs, and blows up a bridge a bomb is fixed to. None of these is done yet.
