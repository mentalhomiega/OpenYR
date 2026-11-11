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
  - Sonic
  - Culling
  - Naval
  - Organic
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

A weapon with [`LimboLaunch=yes`](/keys/limbolaunch/) takes its firer off the map as it fires, to ride the shot. When the shot lands, the parasite gets into its target if the target is a vehicle, soldier or aircraft on the map and alive, of a [`Parasiteable=yes`](/keys/parasiteable/) type, not in a bunker, with no parasite already inside it. A naval parasite also needs the target in water. A parasite that cannot get in comes back onto the map where it leapt from and takes up its idle mission, or is lost if there is no room.

A parasite weapon does not fire at a target it could not get into. It never fires at a structure, at a vehicle in a bunker, at a victim that already holds a parasite, or at an Iron Curtained target. It does not leap between a bridge deck and the ground below when the two are more than two levels apart. After a parasite leaps at a target, no other parasite weapon fires at that target for 20 frames, so two drones sent at one vehicle do not both leap. A parasite that keeps a target it may no longer fire at drops it within 16 frames and takes up its idle mission.

Units that were firing at the parasite drop it as their target when it leaps, because it leaves the map. Its victim never fires back at the parasite inside it.

## Eating the victim

A vehicle or aircraft victim takes the parasite's primary weapon `Damage` through its warhead at once and then every `ROF` frames, with the parasite as the attacker. A soldier instead loses all of its remaining strength at its first bite. That bite ignores the soldier's armor and defenses, so an attack dog kills a soldier at once. A victim other than a soldier also throws sparks from [`DefaultSparkSystem`](/keys/defaultsparksystem/) and plays the weapon's [`Anim`](/keys/anim/) each time, and rolls it sideways to a side picked at random before the damage lands. A naval organic parasite, such as the giant squid, does not bite this way. It [grapples](#grappling-the-victim) its victim instead.

## Grappling the victim

A parasite whose type is both [`Naval=yes`](/keys/naval/) and [`Organic=yes`](/keys/organic/) runs a grapple on its victim in place of the bites above. The grapple moves through four stages, one step each frame:

- **Grab.** The stock `SQDG` animation plays around the victim, ten frames at three frames each. At its end the squid picks a push direction at random, left or right, and three wakes appear around the victim.
- **Push.** The victim rocks toward that side and back: ten frames at four frames each, with its sideways angle at sin(frame x 18 degrees) x 45 degrees.
- **Damage.** On the frame after the push ends, three splashes appear around the victim. A [`Culling=yes`](/keys/culling/) warhead may sink the victim here. Otherwise the victim takes the weapon's `Damage` through its warhead, the squid picks a new push direction at random, and three more wakes appear.

The first bite lands about 70 frames after the grab starts, and a bite lands every 40 frames after that. The victim is held for the warhead's `Paralyzes` frames on every frame of the grapple. If the victim is not on water, the squid comes out at once and is not paralyzed. The grab animation is always `SQDG`; no rules key sets it.

## Holding the victim

A parasite whose warhead has [`Paralyzes`](/keys/paralyzes/) set paralyzes its victim for that many frames with each bite, so a bite at least every `Paralyzes` frames holds the victim for as long as the parasite stays inside. A bite whose warhead has `Paralyzes=0` ends the victim's paralysis instead. A paralyzed vehicle or ship that drives or sails does not start a move and ignores move orders, and a paralyzed object does not launch spawned aircraft or missiles. The paralysis ends as soon as the parasite comes out.

## Coming out

When the victim dies or leaves the map, the parasite comes back out where the victim stood, facing the way the victim faced; a vehicle parasite stands on the center of that cell. It then takes up its idle mission, which is Area Guard for a type with [`DefaultToGuardArea=yes`](/keys/defaulttoguardarea/) such as the terror drone, and a [`ReselectIfLimboed=yes`](/keys/reselectiflimboed/) parasite that the player had selected when it leapt at a soldier is selected again. This exit does not paralyze the parasite, so it takes a move order at once. A parasite with [`RejoinTeamIfLimboed=yes`](/keys/rejointeamiflimboed/) is added back to the team it left, unless the team refuses it.

A parasite cannot come out on the victim's cell when the victim is in the air or the cell holds a structure, a parasite that is not naval cannot come out on water, beach or rock unless a bridge spans the cell, and a vehicle parasite cannot come out on a cell with a tree. It then comes out on the next cell in the direction the victim faced, and is lost if that cell is water, beach or rock with no bridge over the victim's cell, or if the map refuses it there.

A parasite that a sonic hit, a teleport jump or a giant squid's victim reaching land drives out comes out on the victim's cell, or on the nearest free cell around the victim, turned a quarter turn from the victim's facing. It is lost if it cannot stand where the victim is, by the rules above. It is paralyzed for three times the `ROF` of its primary weapon, and it ignores move orders during that time. If its type is [`Organic=yes`](/keys/organic/), such as the giant squid, it does not fire during that time. A naval parasite comes out on the cell beside the victim in the turned direction instead.

The Iron Curtain on a vehicle or aircraft it holds kills a parasite that is not naval at once and drives a naval one out, and ends the victim's paralysis. A soldier under the curtain dies, and its parasite comes out as it does for any dead victim. A chronosphere warp drives out only a naval parasite; a terror drone stays inside its victim through the warp.

Heavy damage to the victim can kill the parasite instead. A hit from anyone else above the parasite type's [`SuppressionThreshold`](/keys/suppressionthreshold/) dooms the parasite for twice the damage less the threshold, in frames, counting the damage as dealt, before the victim's armor reduces it. If the victim dies in that time, the parasite dies with it unless the parasite is under the Iron Curtain; if a parasite that is not naval is driven out in that time, it dies. Healing the victim drives the parasite out doomed, so a parasite that is not naval dies.

A hit from a warhead with [`Sonic=yes`](/keys/sonic/) drives the parasite out of its victim. The parasite comes out unless it is doomed, in which case it dies.

## What the victim can do

A vehicle with a parasite inside it can still move and fire, unless the parasite's warhead paralyzes it. A transport refuses it as a passenger, so clicking it on a transport gives no enter order, and a vehicle that deploys into a structure, such as a construction vehicle, cannot deploy. Each repair step a service depot gives it drives the parasite out doomed, so a parasite that is not naval dies. A vehicle fed to a grinder with a parasite inside refunds the parasite to the grinder's owner as well, and the parasite dies.
