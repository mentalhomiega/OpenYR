---
title: Veterancy and promotion
summary: "How experience, promotion, abilities, and elite weapons work."
category: combat-targeting
keys:
  - Armory
  - CrateRadius
  - EliteAbilities
  - ElitePrimary
  - EliteSecondary
  - EnemyInsignia
  - IRepairRate
  - InitialVeteran
  - Insignia
  - Insignia.ShowEnemy
  - InsigniaFrame
  - InsigniaFrames
  - Promote.EliteSound
  - Promote.VeteranSound
  - Trainable
  - VeteranAbilities
  - VeteranArmor
  - VeteranCap
  - VeteranCombat
  - VeteranLevel
  - VeteranROF
  - VeteranRatio
  - VeteranSight
  - VeteranSpeed
related:
  - type: system
    id: drop-pods
  - type: format
    id: teamtypes
  - type: enum
    id: CrateType
---

## Ranks

Vehicles, infantry, aircraft, and buildings each have their own experience total. That total determines their rank:

| Rank | Experience |
| --- | --- |
| Below rookie | less than `0` |
| Rookie | `0` up to but not including `1` |
| Veteran | `1` up to but not including `2` |
| Elite | `2` and above |

Objects start at rookie with `0` experience unless another setting gives them a different rank. Veteran always starts at `1` and elite at `2`. Rules can change how quickly experience is earned, but these thresholds are fixed.

[`VeteranLevel=0`](/keys/veteranlevel/) gives an object `-0.25` experience, placing it below rookie. A map's [placed-object records](/formats/scenario-objects/) can also set experience directly. Below-rookie objects have no rank abilities, just like rookies. They differ in their insignia and in how crates and armories promote them, as described below.

## Earning experience

The kill goes to the object that deals the fatal damage, or to another object standing in for it. The first of these that applies receives the experience, in this order:

- A unit inside an open-topped transport gives the kill to the transport, if the transport has [`Trainable=yes`](/keys/trainable/).
- Otherwise the killing object itself earns it, if its type has `Trainable=yes`.
- Otherwise an object with `MissileSpawn=yes`, such as a missile, gives the kill to the object that launched it, if that object has `Trainable=yes`.
- Otherwise a structure with [`CanOccupyFire=yes`](/keys/canoccupyfire/) and infantry inside gives the kill to the occupant that fired.

If none applies, no object earns experience from the kill. Score still updates. Each kill adds `victim value / (killer cost * VeteranRatio)` to its experience. The victim value is its cost, doubled when the victim was a veteran and tripled when it was elite at the moment it died. The costs come from [`Cost`](/keys/cost/#scope-aircrafttype); [`Points`](/keys/points/) does not affect experience.

Starting from rookie, an object must destroy enemy value worth [`VeteranRatio`](/keys/veteranratio/) times its own cost to reach veteran, and twice that to reach elite. A lower `VeteranRatio` means faster promotion. Cheaper types need fewer kills against the same enemies, and kills of veteran or elite enemies promote faster.

```ini title="rules.ini"
[General]
VeteranRatio=5
VeteranCap=2
```

This example allows promotion through combat up to elite rank. [`VeteranCap`](/keys/veterancap/) sets the experience limit. These are example values; the linked key pages give the defaults.

:::note[Cost multipliers apply to both sides]
Both costs use the defeated owner's [cost multipliers](/keys/cost/#what-a-house-pays). Applying the same multiplier to both costs leaves their ratio unchanged, except for whole-credit rounding. The attacker's own price multipliers are ignored.
:::

Buildings must explicitly set `Trainable=yes` to earn experience from kills. `Trainable` does not control rank benefits: an untrainable object still benefits from a rank given by an armory, a trigger, or another source that ignores this setting.

A building with a [`FreeUnit`](/keys/freeunit/) usually awards experience based on the building's full price, even after the free unit has left. If that unit costs more than the building, its price is used instead. See [structure costs](/keys/cost/#what-a-structure-gives-away) for the calculation.

Keep `VeteranRatio` and the killer's cost above `0`: the formula divides by both. When either is `0`, every kill the object is credited with sets its experience to `VeteranCap`, whatever the victim costs.

A kill gives no experience and no score points if the defeated owner considers the killer an ally. Loss counts, the last-attacker record, and destruction triggers still update.

Destroying a transport also awards experience for each passenger killed inside it. Crushing an object counts as a kill for the vehicle that crushed it.

:::note[Capturing awards no experience]
Capturing an object gives score points equal to its cost, but no experience. Selling a building or losing an object by sinking gives neither score points nor experience to anyone.
:::

Experience does not decrease over time, and changing owners does not reset it.

### The experience ceiling

After each kill the object is credited with, the total is limited to [`VeteranCap`](/keys/veterancap/), even when the kill is worth nothing. Promotions from other sources ignore this limit.

:::caution[Allowing elite rank through combat]
Set `VeteranCap` to at least `2` to allow elite rank through combat. The default limit is `1`, so kills alone can only produce veterans.
:::

:::caution[A low limit can demote an elite]
An object above the limit drops back to it on its next kill that it is credited with, even an allied one. For example, with `VeteranCap=1`, a trainable elite drops to veteran and loses its elite weapon and [`EliteAbilities`](/keys/eliteabilities/).
:::

## Promotion without kills

The following sources can give an object a rank without kills. They ignore `VeteranCap`; only the veterancy crate requires `Trainable=yes`. A map's placed-object records can also set experience directly.

| Source | Result |
| --- | --- |
| [Veterancy crate](/reference/enums/crate/) | Every `Trainable=yes` object on the ground within [`CrateRadius`](/keys/crateradius/) of the crate rises one rank, repeated as many times as the crate's `Data` figure in `[Powerups]` says, which [Crate powerups](/formats/powerups/) covers. |
| [`Armory=yes`](/keys/armory/) building | The infantry inside is promoted once the building's servicing counter runs out. |
| [Drop Pods superweapon](/systems/drop-pods/#drop-pods-superweapon) | Each delivered passenger is created elite. |
| TeamType [`VeteranLevel`](/keys/veteranlevel/) | Every member created for the team takes the rank the value selects. |
| [`InitialVeteran=yes`](/keys/initialveteran/) | The units and infantry drawn from the random starting selection of a skirmish or multiplayer match are created elite. |
| [Make Elite trigger action](/mapping/actions/taction-make-elite/) | Every object the trigger is attached to is promoted to elite. |

A crate promotes each affected object one step at a time: below rookie to rookie, rookie to veteran, then veteran to elite. Elite objects stay elite. A unit that collects a veterancy crate receives money instead when it is not `Trainable=yes` or is already elite, and then promotes nothing. The crate's `Data` value in `[Powerups]` sets the number of steps. Fractions round up, so `2.5` gives three steps and `2` gives two. A value of `0` or less gives no promotion.

The other sources set the rank directly. The armory's result depends on the infantry's current rank.

:::caution[A veterancy crate promotes every trainable object nearby]
Crates also promote enemy and neutral objects within `CrateRadius`, including buildings, if their types have `Trainable=yes`.
:::

:::caution[An armory skips the veteran rank]
Rookie and veteran infantry leave an armory as elite. Below-rookie infantry leave as veteran. Elite infantry do not get an enter cursor. Each admission uses one point of the building's [`Ammo`](/keys/ammo/), which an armory never replenishes.
:::

The training time is set by [`IRepairRate`](/keys/irepairrate/), which also controls hospital treatment time.

## What a rank changes

Promotion bonuses depend on the type's ability lists. A veteran with no abilities gains only a rank insignia. Elite objects can also use elite weapons and cause other objects in their cell to scatter, as described below.

### Abilities

[`VeteranAbilities`](/keys/veteranabilities/) and [`EliteAbilities`](/keys/eliteabilities/) are comma-separated lists on each object type. Veterans use `VeteranAbilities`. Elites use both lists, keeping their veteran abilities after promotion. Rookie and below-rookie objects use neither list.

```ini title="rules.ini"
[MYINF] ; example InfantryType
VeteranAbilities=FIREPOWER,ROF
EliteAbilities=SELF_HEAL,FEARLESS
```

A veteran `MYINF` deals more damage and reloads faster. At elite rank it also heals itself and becomes immune to fear.

Ability names are case-insensitive. The table lists all eighteen abilities and their effects.

| Token | Effect on a qualifying object |
| --- | --- |
| `FASTER` | Movement speed is multiplied by [`VeteranSpeed`](/keys/veteranspeed/). Buildings have no speed to raise. |
| `STRONGER` | Incoming damage is divided by [`VeteranArmor`](/keys/veteranarmor/). |
| `FIREPOWER` | Weapon damage is multiplied by [`VeteranCombat`](/keys/veterancombat/). |
| `SCATTER` | The object scatters from incoming fire even when the owner's units would otherwise hold position. |
| `ROF` | Reload delay is multiplied by [`VeteranROF`](/keys/veteranrof/). |
| `SIGHT` | Sight range is multiplied by [`VeteranSight`](/keys/veteransight/). |
| `CLOAK` | The object cloaks and recloaks without [`Cloakable=yes`](/keys/cloakable/), and stays cloaked while immobilized. |
| `TIBERIUM_PROOF` | Infantry take no damage from standing in Tiberium, as with [`TiberiumProof=yes`](/keys/tiberiumproof/). |
| `VEIN_PROOF` | Veins do not damage the object, as with [`ImmuneToVeins=yes`](/keys/immunetoveins/). |
| `SELF_HEAL` | The object repairs itself while below its [self-healing ceiling](/systems/repair/#self-healing), as with [`SelfHealing=yes`](/keys/selfhealing/). |
| `EXPLODES` | Death produces the violent explosion of [`Explodes=yes`](/keys/explodes/). |
| `RADAR_INVISIBLE` | The object is kept off the local player's radar unless a sensor contact picks it up. |
| `SENSORS` | An adjacent enemy cloaked object shimmers, as with [`Sensors=yes`](/keys/sensors/). |
| `FEARLESS` | Infantry accumulate no fear, as with [`Fearless=yes`](/keys/fearless/). |
| `C4` | Infantry attack buildings by sabotage, as with [`C4=yes`](/keys/c4/). |
| `TIBERIUM_HEAL` | Standing in Tiberium repairs the object, as with [`TiberiumHeal=yes`](/keys/tiberiumheal/). |
| `GUARD_AREA` | An idle armed vehicle, and an idle human-owned infantry, takes the Guard Area mission instead of Guard. A member of a team is unaffected, and so is a computer-owned infantry, whose idle handling never reads the ability. |
| `CRUSHER` | A vehicle crushes crushable objects and overlays, as with [`Crusher=yes`](/keys/crusher/). |
| `EMPIMMUNE` | EMP does not disable the object, as with [`ImmuneToEMP=yes`](/keys/immunetoemp/) (Ares). |
| `RADIMMUNE` | Radiation warheads and lingering radiation do not harm the object, as with [`ImmuneToRadiation=yes`](/keys/immunetoradiation/) (Ares). |
| `UNWARPABLE` | Temporal weapons cannot warp the object, as with [`Warpable=no`](/keys/warpable/) (Ares). |
| `POISONIMMUNE` | `Poison=yes` warheads do not harm the object, as with [`ImmuneToPoison=yes`](/keys/immunetopoison/) (Ares). |
| `PSIONICWEAPONIMMUNE` | `PsychicDamage=yes` warheads do not harm the object, as with [`ImmuneToPsionicWeapons=yes`](/keys/immunetopsionicweapons/) (Ares). |
| `PSIONICSIMMUNE` | Mind control, `Psychedelic=yes` warheads and the Psychic Dominator treat the object as they treat [`ImmuneToPsionics=yes`](/keys/immunetopsionics/) (Ares). |

Unknown or misspelled ability names are ignored without a warning.

:::caution[Write ability lists without spaces]
Spaces around individual names are not removed. For example, `VeteranAbilities=FIREPOWER, ROF` enables only `FIREPOWER`, because the space before `ROF` makes it an unknown name. Spaces at the start or end of the whole value are removed.
:::

Setting an ability list in a later rules file replaces the earlier list. Leaving the key out keeps the earlier list. An empty assignment is ignored, so it cannot clear an existing list.

:::caution[An elite object makes its whole cell scatter]
Incoming fire at a cell containing an elite object makes all occupants scatter, even those without `SCATTER`. This effect requires no ability.
:::

### Rank multipliers

Set the five bonuses below in `rules.ini` under `[General]`. Each applies only when the object has the matching ability:

| Setting | Applied as |
| --- | --- |
| [`VeteranCombat`](/keys/veterancombat/) | Damage is multiplied by the value, after the house and object firepower biases. |
| [`VeteranSpeed`](/keys/veteranspeed/) | Speed is multiplied by the value, after the house ground-speed bias. |
| [`VeteranSight`](/keys/veteransight/) | Sight range is multiplied by the value. A value of `0` switches the bonus off. |
| [`VeteranArmor`](/keys/veteranarmor/) | Incoming damage is divided by the value. |
| [`VeteranROF`](/keys/veteranrof/) | Reload delay is multiplied by the value. |

All five default to `1`, which changes nothing. Keep `VeteranArmor` above `0`, because incoming damage is divided by it.

Sonic weapons and weapons using fire particles receive no firepower bonus. Sonic weapons also receive no reload bonus, as do weapons with an attached spark, fire, or railgun particle system. The reload bonus shortens the delay after a burst, not the gaps between its shots.

### The elite weapons

At elite rank, the weapon named by [`ElitePrimary`](/keys/eliteprimary/) replaces the primary weapon, and the weapon named by [`EliteSecondary`](/keys/elitesecondary/) replaces the secondary. Target selection, range, reload delay, and firing all use the replacement. A slot whose elite key is unset keeps its normal weapon.

Each elite weapon has its own art settings, which default to the normal slot's: [`ElitePrimaryFireFLH`](/keys/eliteprimaryfireflh/), [`ElitePBarrelLength`](/keys/elitepbarrellength/) and [`ElitePBarrelThickness`](/keys/elitepbarrelthickness/) for the primary, and [`EliteSecondaryFireFLH`](/keys/elitesecondaryfireflh/), [`EliteSBarrelLength`](/keys/elitesbarrellength/) and [`EliteSBarrelThickness`](/keys/elitesbarrelthickness/) for the secondary.

:::caution[Building upgrades take priority]
A building upgrade that supplies a weapon takes priority over the building's own weapon in that slot, including its elite weapon. Upgrades are structure types with [`PowersUpBuilding=`](/keys/powersupbuilding/) installed in the host's [upgrade slots](/keys/upgrades/).
:::

### When a promotion takes effect

Promotion bonuses apply the next time the object fires, takes damage, or moves. The game checks its current rank and abilities whenever it calculates those effects.

Extra sight reveals more terrain the next time the object reveals its surroundings. Promotion itself does not reveal the larger area.

When one of the player's objects becomes a veteran or elite, the player hears the type's [`Promote.VeteranSound`](/keys/promote.veteransound/) or [`Promote.EliteSound`](/keys/promote.elitesound/) at the object, or [`UpgradeVeteranSound`](/keys/upgradeveteransound/) or [`UpgradeEliteSound`](/keys/upgradeelitesound/) when the type sets none, and with [EVAMD.INI](/formats/eva-ini/), `EVA_UnitPromoted`. Any object that becomes elite, whoever owns it, flashes for [`EliteFlashTimer`](/keys/eliteflashtimer/) frames. An object created at a rank is not announced.

## Carrying rank between objects

A vehicle keeps its experience when it deploys into a building, and the building passes that experience back when it undeploys. Capturing an object also preserves its rank and any abilities or elite weapon that rank grants.

Passengers who survive a destroyed transport keep their ranks. Crew created when a vehicle or building is destroyed start at rookie.

## Rank display

Veteran and elite objects display different rank insignia beside them, even when unselected. Buildings display them too. The insignia sits farther from vehicles, aircraft, and buildings than from infantry.

Players see an object's rank insignia whoever owns it, unless [`EnemyInsignia=no`](/keys/enemyinsignia/) or the type's [`Insignia.ShowEnemy=no`](/keys/insignia.showenemy/) limits it to the owner and the owner's allies. An observer sees every insignia. Shroud, fog, or an undetected cloak hides the insignia along with the object.

The insignia comes from `PIPS.SHP`: frame 14 for a veteran, frame 15 for an elite object and frame 19 for a below-rookie object. A rookie has none.

### Custom insignia

A type can draw its own insignia for the rookie, veteran and elite ranks. [`Insignia`](/keys/insignia/) names the shape file, and [`InsigniaFrame`](/keys/insigniaframe/) or [`InsigniaFrames`](/keys/insigniaframes/) names the frame. A frame the type sets for a rank replaces the stock frame, so setting one for a rookie shows an insignia where there was none. The file and the frame are chosen separately: a rank with a file and no frame draws the stock frame number from the new file, and a rank with a frame and no file draws that frame from `PIPS.SHP`.

Each key has per-rank forms, such as `Insignia.Elite` and `InsigniaFrame.Veteran`. For each rank the shorthand sets the value first, and the rank's own key then overrides it. The below-rookie insignia is not affected.
