---
title: Warheads and damage
summary: "How a blast chooses the objects it damages, how much strength each one loses, and what else the blast does to the ground."
category: weapons-projectiles
keys:
  - AffectsAllies
  - ShakeXlo
  - ShakeXhi
  - ShakeYlo
  - ShakeYhi
  - AmmoCrateDamage
  - AnimList
  - Armor
  - AtomDamage
  - BridgeStrength
  - Bright
  - CellSpread
  - ChainReaction
  - CollapseChance
  - Conventional
  - Damage
  - DamageSelf
  - Deform
  - DeformThreshhold
  - DestroyableBridges
  - EMEffect
  - ExpSpread
  - Explodes
  - Fire
  - HarvesterUnit
  - Immune
  - Inert
  - InfDeath
  - InvisibleInGame
  - IonImmune
  - IonStormWarhead
  - IsVeinholeMonster
  - Jellyfish
  - JumpJet
  - LimpetFactor
  - MaxDamage
  - MinDamage
  - Particle
  - PercentAtMax
  - ProneDamage
  - Rocker
  - Sparky
  - SplashList
  - Spread
  - Tiberium
  - TiberiumExplosive
  - TypeImmune
  - Veinhole
  - Verses
  - VeteranArmor
  - Wall
  - WallAbsoluteDestroyer
  - Warhead
  - Webby
  - Wood
related:
  - type: system
    id: projectile-flight
  - type: system
    id: firing-geometry
  - type: system
    id: destruction-and-debris
  - type: system
    id: target-selection
  - type: system
    id: veterancy
  - type: system
    id: difficulty
  - type: system
    id: emp-pulse
  - type: enum
    id: ArmorType
---

Damage reaches an object in two stages. First, a **blast** goes off at one point on the map and collects the objects near it. A blast carries a raw damage figure, a warhead, and the object credited with anything it kills. A landing shell raises a blast. So do a chain-reacting Tiberium field, an exploding barrel, a lightning bolt from an ion storm, and the [C4](/keys/c4/) charge an infantryman sets on a bridge. Second, each object the blast collected converts the raw figure into the strength it loses. This conversion also runs for damage that comes from no blast, such as [the damage a base takes while it is short of power](/systems/power/#the-structure-damage-tick).

This page covers both stages. [Firing geometry and beam weapons](/systems/firing-geometry/) sets the raw figure a shot carries. [Projectile flight and impact](/systems/projectile-flight/) sets where the blast goes off and how many blasts one shot raises. [Destruction and debris](/systems/destruction-and-debris/) covers what a destroyed object leaves behind.

## Warheads in brief

A **WarheadType** is a rules section that describes what damage does once it arrives. The rules [`[Warheads]` list](/formats/rules-registries/) registers each one, and a weapon's [`Warhead=`](/keys/warhead/#scope-weapontype) names the one its shots use. The `Warhead=` page covers a name that no section declares and a weapon with no warhead.

The weapon sets how much damage a shot deals, and the projectile sets how the shot travels. The warhead sets how far the blast reaches, how the damage thins with distance, and how much of it each armor class takes. It also sets every effect the blast has on the ground.

```ini title="rulesmd.ini"
[MyShellWH]                    ; a WarheadType registered in [Warheads]
Verses=100%,100%,90%,80%,70%,60%,50%,40%,30%,100%,100%
CellSpread=1.5                 ; the blast reaches a cell and a half
PercentAtMax=.5                ; and deals half damage at that distance
ProneDamage=50%                ; what reaches an infantryman lying down
AnimList=MYBANG16,MYBANG24     ; example AnimTypes
```

`Verses` lists one percentage for each [armor class](/reference/enums/armor/) an object type can have, in a fixed order. The values are examples.

Three warhead settings replace blast damage entirely:

- A shot whose warhead is [`EMEffect=yes`](/keys/emeffect/) raises [a pulse](/systems/emp-pulse/) instead of a blast.
- A shot whose warhead is [`Webby=yes`](/keys/webby/) spreads a web over the surrounding cells instead of raising a blast.
- A weapon whose warhead sets [`LimpetFactor`](/keys/limpetfactor/) to `1` or more never fires a shot at a vehicle, infantryman, aircraft or structure. The `LimpetFactor` page covers what it does instead.

## What a blast reaches

A blast first collects its full list of candidates, and only then damages any of them. Two sweeps fill the list. The cell sweep always runs, and the airborne sweep runs only when the blast is above the ground. Every candidate from either sweep then faces the same tests.

A [lepton](/glossary/#lepton) is the engine's distance unit, and a cell is 256 leptons across. The warhead's [`CellSpread`](/keys/cellspread/) sets the blast's **reach**: `CellSpread` × 256 leptons, rounded down. At the default `CellSpread=0`, the reach is zero and only a target at the exact point of impact can be damaged.

### The cells in reach

The cell sweep collects everything standing in the cells around the blast: structures, vehicles, infantry, landed aircraft and trees. It covers every cell whose offset from the blast's cell, counting the longer of its two axis offsets plus half the shorter one rounded down, is no more than `CellSpread` rounded up. At `CellSpread=0` the sweep covers only the blast's cell, and up to `1` it covers that cell and the eight around it. The sweep never covers cells more than 11 away.

Each object is measured as follows:

- A vehicle, infantryman, landed aircraft or tree is measured in a straight line, in three dimensions, from the blast to its center.
- A structure is collected once for every one of its cells the sweep covers, and each of those is a separate hit. Each is measured from the center of that cell, at ground level. In the blast's own cell, a structure counts as a direct hit, at distance zero, unless the blast goes off more than two terrain levels (208 leptons) above that cell's ground. A large structure caught by a wide blast therefore loses strength several times over.

The cell sweep leaves out two kinds of candidate:

- the object credited with the blast, unless its type is [`DamageSelf=yes`](/keys/damageself/);
- a vehicle whose type is listed in [`HarvesterUnit`](/keys/harvesterunit/), while the harvester truce or the scenario's [`HarvesterImmune`](/keys/harvesterimmune/) flag is on.

A [veinhole monster](/systems/veins/#destruction) standing in any cell in reach is also collected, provided its overlay sets [`IsVeinholeMonster=yes`](/keys/isveinholemonster/).

The blast's cell decides whether the sweep collects objects on bridge decks or objects on the ground, and that choice applies to every cell in reach. When the blast's cell lies under a bridge and the blast is more than 208 leptons above the ground there, the sweep collects what stands on the bridge decks. Otherwise it collects what stands on the ground.

### The airborne sweep

When the blast is above the ground level at the center of its cell, a second sweep runs. It checks every aircraft, every infantryman whose type is [`JumpJet=yes`](/keys/jumpjet/), and every vehicle whose type is [`Jellyfish=yes`](/keys/jellyfish/). It collects each one that meets **all of:**

- it is placed on the map and in the air;
- it has strength left;
- it is within the blast's reach, measured in three dimensions.

This sweep is the only way a blast reaches a flying object, because an object in flight is not listed in any cell. A shell that explodes one lepton above the ground runs this sweep just as an airburst overhead does. This sweep does not leave out the object credited with the blast. Each cell of [a wide-area blast](#the-wide-area-blast) goes off exactly at ground level, so a wide-area blast never reaches a flying object.

### Which candidates are damaged

Each candidate is then tested in the order below. It is damaged only if it meets **all of:**

- it is still in play;
- it is not a structure whose type is [`InvisibleInGame=yes`](/keys/invisibleingame/);
- its strength is above zero;
- it is placed on the map and not in [limbo](/glossary/#limbo);
- its distance is within the blast's reach. An aircraft in the air counts at half its distance here and in the damage it takes;
- **any of:**
  - the warhead is not the one [`IonStormWarhead`](/keys/ionstormwarhead/) names;
  - the candidate is not an infantryman, vehicle or aircraft;
  - the candidate belongs to no team;
  - the candidate's team type is not [`IonImmune=yes`](/keys/ionimmune/).

A pinpoint blast, one with `CellSpread` of `0.5` or less, that lands within 85 leptons of an object under the [Iron Curtain](/systems/superweapons/#iron-curtain) damages only objects under the Iron Curtain, which take none. The object must be in the blast's own cell or in the air. Everything else nearby is spared.

## What the target loses

Every candidate receives the same raw figure and then reduces it separately, so two objects in the same explosion can lose very different amounts. The steps run in this order:

1. **Prone infantry.** An infantryman lying down keeps only the warhead's [`ProneDamage`](/keys/pronedamage/) fraction of the figure, rounded down but never below one point.
2. **Web.** A [`Webby=yes`](/keys/webby/) warhead entangles any infantryman whose type is not [`IsWebImmune=yes`](/keys/iswebimmune/). The figure becomes zero, so the hit does nothing else.
3. **Armor multipliers.** The figure is divided by the owning house's armor multiplier and by the object's crate armor multiplier, then rounded down. The crate multiplier stays `1` until an armor crate raises it. In a campaign, the house multiplier is the difficulty's [`Armor=`](/keys/armor/#scope-difficulty-settings). In any other game, it is that figure multiplied by the country's [`Armor=`](/keys/armor/#scope-housetype). [Difficulty settings](/systems/difficulty/#how-the-figures-are-combined) shows how the two combine.
4. **Veteran armor.** An object with the `STRONGER` [ability](/systems/veterancy/#abilities) divides the figure again by one plus [`VeteranArmor`](/keys/veteranarmor/), rounded down.
5. **Minimum of one.** If steps 3 and 4 left less than one point, the figure becomes one point.
6. **Type immunity.** An object whose type is [`TypeImmune=yes`](/keys/typeimmune/) takes no damage from a credited attacker of the same type owned by the same house.
7. **Iron Curtain.** An object under the [Iron Curtain](/systems/superweapons/#iron-curtain) takes no damage.
8. **Allies.** An [`AffectsAllies=no`](/keys/affectsallies/) warhead does nothing to an object whose owner is an ally of the credited attacker's house, the attacker's own house included.
9. **Object immunity.** An object whose type is [`Immune=yes`](/keys/immune/#scope-aircrafttype) takes no damage. Neither does an object already at zero strength.
10. **Distance.** The figure thins with the target's distance from the blast, as [the next section](#how-distance-thins-the-damage) explains.
11. **Armor table.** The result is multiplied by the warhead's [`Verses`](/keys/verses/) entry for the target's [`Armor=`](/keys/armor/#scope-aircrafttype) class and rounded down. It can reach zero.
12. **`MaxDamage` ceiling.** The result is capped at [`MaxDamage`](/keys/maxdamage/). The cap applies to each hit separately, not to the blast as a whole.
13. **Strength lost.** The result is taken off the target's strength, but never more than the strength it had. A killing blow therefore counts only the strength the target had left.

Steps 3 to 8 apply only to vehicles, infantry, aircraft and structures, so a tree or a veinhole monster skips them.

A tree takes no damage at all unless the warhead is [`Wood=yes`](/keys/wood/).

A blast never deals forced damage. The engine deals forced damage directly, for example when a C4 charge destroys a structure. Forced damage skips step 1 and steps 3 to 12, so of the numbered steps only the web of step 2 and step 13 apply. It still does nothing to an object already at zero strength. Refusals that belong to a particular kind of object also still apply, such as a tree's need for a `Wood=yes` warhead and a harvester's protection under the harvester truce.

Unforced damage with no warhead, or any unforced damage in a scenario with [`Inert=yes`](/keys/inert/), does nothing. An ordinary blast in either case damages nothing and has none of the ground effects below. A [wide-area blast](#the-wide-area-blast) in an `Inert=yes` scenario can still crater the ground at its center.

### How distance thins the damage

Damage falls in a straight line from the full figure at the point of impact to the warhead's [`PercentAtMax`](/keys/percentatmax/) share of it at the edge of the reach. The result is rounded down and never drops below zero.

With `R` the reach in leptons and `F` the figure, a target `d` leptons away takes `(F − F × PercentAtMax) × (R − d) ÷ R + F × PercentAtMax`. For a 100-point figure with `CellSpread=2` (a reach of 512 leptons) and `PercentAtMax=.5`:

| Distance | Damage before `Verses` |
| --- | --- |
| 0 (a direct hit) | 100 |
| 256 leptons (one cell) | 75 |
| 512 leptons (the edge) | 50 |

The damage does not thin at all when `PercentAtMax=1`, the default, or when the reach is zero. A `PercentAtMax` above `1` makes damage grow toward the edge.

## Healing

A weapon with a negative [`Damage=`](/keys/damage/#scope-weapontype) heals. A healing figure skips every reduction above: the prone fraction, the armor multipliers, veteran armor, type immunity, the Iron Curtain, distance thinning, the armor table and `MaxDamage`. The target regains the whole figure, up to its type's maximum strength, if it meets **all of:**

- it is less than 8 leptons from the blast, a thirty-second of a cell, so a healing shot repairs only what it hits and nothing beside it;
- it is not `Immune=yes`;
- it still has strength left, so nothing destroyed is revived.

No trigger event springs from a heal, and the healed object does not fire back at the healer.

Healing also clears every [limpet](/keys/limpetfactor/) mark from a vehicle, infantryman, aircraft or structure. This happens to every such object within the blast's reach, before the 8-lepton and immunity tests. A healing blast therefore strips limpet marks from everything around it but repairs only what it hits.

## What one blast does besides damage

A blast acts on the overlays in reach while its cell sweep runs, before any object is damaged. Every threshold below is compared with the blast's raw figure, not with the damage any object took. Armor and distance therefore never decide whether those effects happen.

- A Tiberium overlay in any cell the sweep covers that sets [`ChainReaction=yes`](/keys/chainreaction/) loses a tenth of the raw figure from its Tiberium when the warhead is [`Tiberium=yes`](/keys/tiberium/#scope-warheadtype). A blast never sets the Tiberium off. Several blasts the engine raises itself skip this step.
- A wall overlay in any cell the sweep covers is knocked down outright by a [`WallAbsoluteDestroyer=yes`](/keys/wallabsolutedestroyer/) warhead. Otherwise it may drop one damage level when the warhead is [`Wall=yes`](/keys/wall/#scope-warheadtype), or [`Wood=yes`](/keys/wood/) against a wood-armored wall. The wall always drops when the raw figure is at least the overlay's [`Strength`](/keys/strength/#scope-overlaytype). Below that, the chance is about the raw figure divided by that `Strength`.

After it damages objects, the blast acts on the ground in the order below.

1. [`Rocker=yes`](/keys/rocker/) rocks every object drawn as a voxel in the seven-by-seven block of cells around the blast, when the raw figure is above 30. No setting changes that threshold.
2. A bridge span at the blast's cell may be damaged, when [`DestroyableBridges=yes`](/keys/destroyablebridges/) and the warhead is `Wall=yes`. [`BridgeStrength`](/keys/bridgestrength/) sets the chance.
3. An [`Explodes=yes`](/keys/explodes/#scope-overlaytype) overlay in the blast's cell is removed and explodes for [`AmmoCrateDamage`](/keys/ammocratedamage/), whatever the warhead.
4. The ground may be cratered when the raw figure is above [`DeformThreshhold`](/keys/deformthreshhold/). [`Deform`](/keys/deform/) sets the chance.
5. A destroyable cliff at the blast's cell may collapse, with a chance of [`CollapseChance`](/keys/collapsechance/), whatever the warhead.
6. The warhead's [`Particle`](/keys/particle/) system releases one particle at the blast.
7. A `Wall=yes` or [`Fire=yes`](/keys/fire/) warhead cracks the ice beneath the blast, unless the blast is up on a bridge.

The explosion animation and the lighting flash are not blast effects. [`AnimList`](/keys/animlist/), the [`SplashList`](/keys/splashlist/) animation that [`Conventional=yes`](/keys/conventional/) uses over water, and the [`Bright=yes`](/keys/bright/#scope-warheadtype) flash are chosen by whatever raised the blast. Whether they appear therefore depends on what caused the explosion as well as on the warhead.

A projectile that detonates shakes the screen when its warhead sets a shake range. The view moves sideways by a random number of pixels from [`ShakeXlo`](/keys/shakexlo/) to [`ShakeXhi`](/keys/shakexhi/), and vertically by one from [`ShakeYlo`](/keys/shakeylo/) to [`ShakeYhi`](/keys/shakeyhi/); a range of `0` to `0` leaves that direction alone. Every second frame the view swings to the other side and the shake shrinks by one pixel, so a 5-pixel shake settles after about ten frames. The shake happens wherever the projectile lands, even out of view, and only blasts raised by projectiles shake the screen.

Three more warhead settings act on the damaged object. Each is described on the page that covers its effect:

- [`InfDeath`](/keys/infdeath/) picks the death animation a killed infantryman plays.
- [`Sparky=yes`](/keys/sparky/) chooses the flames a structure shows as it drops a damage level, and sets a struck tree alight.
- [`Veinhole=yes`](/keys/veinhole/) gives a damaged object something to fire back at when no attacker is credited. [Target selection](/systems/target-selection/#retaliation) covers when a damaged object fires back.

## The wide-area blast

A wide-area blast raises an ordinary blast in every cell of a square block. The center cell's blast carries the full raw figure. Every other cell's blast carries the figure scaled by that cell's distance from the center, so the figure rises toward the rim instead of falling. [`ExpSpread`](/keys/expspread/) gives the size of the block and the scale.

No landing shot raises a wide-area blast. Two deaths do:

- the collateral blast of a dying object whose type is [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) or that has the `EXPLODES` [ability](/systems/veterancy/#abilities);
- the extra blast a destroyed harvester adds for the Tiberium it carries, under [`TiberiumExplosive=yes`](/keys/tiberiumexplosive/#scope-global-rules), unless the harvester truce or the scenario's `HarvesterImmune` flag is on.

A nuclear detonation has a fallback route to a wide-area blast for when its explosion animation cannot be created. [`AtomDamage`](/keys/atomdamage/) explains why that route is never taken.
