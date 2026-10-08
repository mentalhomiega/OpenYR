---
title: Parasites
summary: "How a parasite such as the terror drone leaps into a vehicle or soldier, eats it from inside, and comes back out."
category: combat-targeting
keys:
  - Parasite
  - LimboLaunch
  - Parasiteable
  - SuppressionThreshold
  - ReselectIfLimboed
  - Paralyzes
related:
  - type: system
    id: temporal-weapons
---

An object whose primary weapon has a [`Parasite=yes`](/keys/parasite/) warhead gets inside what that weapon hits and hurts it from there. The object gets this ability when it is placed on the map, from the primary weapon it has at that moment; structures never do.

```ini title="rulesmd.ini"
[MYDRONE] ; example VehicleType
Primary=MyDroneJump
SuppressionThreshold=5

[MyDroneJump]
Damage=50          ; damage each bite
ROF=60             ; frames between bites
Warhead=MyParasite
LimboLaunch=yes    ; the drone rides the shot

[MyParasite]
Parasite=yes
```

## Getting in

A weapon with [`LimboLaunch=yes`](/keys/limbolaunch/) takes its firer off the map as it fires, to ride the shot. When the shot lands, the parasite gets into its target if the target is a vehicle, soldier or aircraft on the map and alive, of a [`Parasiteable=yes`](/keys/parasiteable/) type, with no parasite already inside it. A naval parasite also needs the target in water. A parasite that cannot get in comes back onto the map where it leapt from, or is lost if there is no room.

## Eating the victim

The victim takes the parasite's primary weapon `Damage` through its warhead at once and then every `ROF` frames, with the parasite as the attacker. A victim other than a soldier also throws sparks from [`DefaultSparkSystem`](/keys/defaultsparksystem/) and plays the weapon's [`Anim`](/keys/anim/) each time.

## Holding the victim

A parasite whose warhead has [`Paralyzes`](/keys/paralyzes/) set paralyzes its victim for that many frames with each bite, so a bite at least every `Paralyzes` frames holds the victim for as long as the parasite stays inside. A paralyzed vehicle or ship that drives or sails does not start a move and ignores move orders, and a paralyzed object does not launch spawned aircraft or missiles. The paralysis ends as soon as the parasite comes out.

## Coming out

When the victim dies or leaves the map, the parasite comes back out on the victim's cell, or the nearest cell it can stand on. It guards there, and a [`ReselectIfLimboed=yes`](/keys/reselectiflimboed/) parasite the player had selected when it leapt is selected again. A parasite with no room to come out is lost. The Iron Curtain on a vehicle or aircraft it holds kills the parasite at once instead, and ends the victim's paralysis. A soldier under the curtain dies, and its parasite comes out as it does for any dead victim.

A parasite that comes out is paralyzed for one `ROF` of its primary weapon. If its type is [`Organic=yes`](/keys/organic/), such as the giant squid, it does not fire during that time.

Heavy damage to the victim can kill the parasite instead. A hit from anyone else above the parasite type's [`SuppressionThreshold`](/keys/suppressionthreshold/) dooms the parasite for twice the damage less the threshold, in frames; if the victim dies or the parasite is driven out in that time, the parasite dies with it. Healing the victim kills the parasite.

Yuri's Revenge also draws the giant squid wrapped around the ship it holds, and rocks a vehicle each time the parasite bites. Neither is done yet.
