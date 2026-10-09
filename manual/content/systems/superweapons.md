---
title: Superweapons
summary: "How a house gains, charges and fires each declared superweapon, and what each of the hard-coded behaviors delivers."
category: superweapons-special
keys:
  - ShowTimer
  - DisableableFromShell
  - AIIonCannonAPCValue
  - AIIonCannonBaseDefenseValue
  - AIIonCannonConYardValue
  - AIIonCannonEngineerValue
  - AIIonCannonHarvesterValue
  - AIIonCannonHelipadValue
  - AIIonCannonMCVValue
  - AIIonCannonPlugValue
  - AIIonCannonPowerValue
  - AIIonCannonTempleValue
  - AIIonCannonThiefValue
  - AIIonCannonWarFactoryValue
  - AIMinorSuperReadyPercent
  - Action
  - AllyParaDropInf
  - AllyParaDropNum
  - AmerParaDropInf
  - AmerParaDropNum
  - AnimToInfantry
  - BalloonHover
  - AuxBuilding
  - ChargeToDrainRatio
  - ChargingVoice
  - ChronoBlast
  - ChronoBlastDest
  - ChronoInfantryCrush
  - ChronoInSound
  - ChronoOutSound
  - ChronoPlacement
  - Chronoshift.Allow
  - Chronoshift.Crushable
  - DominatorCaptureRange
  - DominatorDamage
  - DominatorFireAtPercentage
  - DominatorFirstAnim
  - DominatorSecondAnim
  - DominatorWarhead
  - DamageToFirestormDamageCoefficient
  - FirestormWall
  - FirestormWarhead
  - ForceShieldBlackoutDuration
  - ForceShieldDuration
  - ForceShieldInvokeAnim
  - ForceShieldPlayFadeSoundTime
  - ForceShieldRadius
  - GeneticMutatorActivateSound
  - GDIFirestormGenerator
  - GDIHunterSeeker
  - HSBuilding
  - ImmuneToPsionics
  - ImpatientVoice
  - IonCannonDamage
  - IonCannonWarhead
  - IronCurtainDuration
  - IronCurtainInvokeAnim
  - LightningCellSpread
  - LightningDamage
  - LightningDeferment
  - LightningHitDelay
  - LightningPrintText
  - LightningScatterDelay
  - LightningSeparation
  - LightningSounds
  - LightningStormDuration
  - LightningWarhead
  - IsPowered
  - MakeInfantry
  - ManualControl
  - MindControlRingOffset
  - MutateExplosion
  - MutateExplosionWarhead
  - MutateWarhead
  - NodHunterSeeker
  - NukeMaker
  - NukeSilo
  - NukeTakeOff
  - Organic
  - ParadropRadius
  - PermaControlledAnimationType
  - PostClick
  - PreClick
  - PreDependent
  - PsychicDominatorActivateSound
  - PsychicRevealActivateSound
  - PsychicRevealRadius
  - RechargeTime
  - RechargeVoice
  - SidebarImage
  - SidebarPCX
  - SovParaDropInf
  - SovParaDropNum
  - SpecialSound
  - SpyPlaneCamera
  - SpyPlaneCameraFrames
  - StartSound
  - StormSound
  - SuperWeapon
  - SuperWeapon2
  - SuperWeapons
  - SuspendVoice
  - Teleporter
  - Type
  - UseChargeDrain
  - WarpOut
  - WeaponType
  - WeatherConBoltExplosion
  - WeatherConBolts
  - WeatherConClouds
  - WeedCapacity
  - YuriParaDropInf
  - YuriParaDropNum
related:
  - type: system
    id: drop-pods
  - type: system
    id: emp-pulse
  - type: system
    id: power
  - type: action
    id: TACTION_1_SPECIAL
  - type: action
    id: TACTION_FULL_SPECIAL
  - type: action
    id: TACTION_ACTIVATE_FIRESTORM
  - type: action
    id: TACTION_DEACTIVATE_FIRESTORM
---

Every superweapon has one rules section and one copy in each house. The section sets what all copies share: the recharge delay, the cameo, the mouse action and the behavior. Each house's copy keeps its own state: whether the house holds the weapon, whether it is suspended, and how far its countdown has run.

A weapon's **behavior** is the effect it delivers when fired. `Type=` selects one of the behaviors built into the engine, and rules cannot add more. The **declared list** is `[SuperWeaponTypes]`, and a weapon's **position** in that list is the number trigger actions use to name it.

## Declaring a superweapon

`[SuperWeaponTypes]` lists the sections that declare superweapons.

```ini title="rules.ini"
[SuperWeaponTypes]
1=MultiSpecial
2=EMPulseSpecial
3=FirestormSpecial
4=IonCannonSpecial
5=HuntSeekSpecial
6=ChemicalSpecial
7=DropPodSpecial
```

The list is read in order, and each new name takes the next position. Later rules files, such as `FIRESTRM.INI` and a scenario's rules override, can add names. A name that first appears there goes after everything `rules.ini` declared. A name that is already declared keeps its position.

Each house receives one copy of every declared weapon when the house is created, in list order. The two trigger actions that grant a weapon, [Add 1-time special weapon](/mapping/actions/taction-1-special/) and [Add repeating special weapon](/mapping/actions/taction-full-special/), name it by its position. A structure names it by section name, in [`SuperWeapon=`](/keys/superweapon/) or [`SuperWeapon2=`](/keys/superweapon2/).

A listed name with no matching section is still declared, with every default. It charges for five minutes, has no cameo artwork, and does nothing when fired.

[`Type=`](/keys/type/#scope-superweapontype) selects the behavior. Several sections may use the same behavior, and each still has a separate cameo, countdown and grant condition. A section with no recognized `Type=` charges and shows a cameo like any other, but firing it has no effect.

:::danger[Keep `[SuperWeaponTypes]` in the shipped order]
A missile silo does not launch the `WeaponType=` of the weapon that ordered the launch. It uses the section whose position matches that weapon's behavior, in this order: `MultiMissile`, `EMPulse`, `Firestorm`, `IonCannon`, `HunterSeeker`, `ChemMissile`, `DropPod`. The first section in the list therefore supplies every `Type=MultiMissile` launch, and the sixth supplies every `Type=ChemMissile` launch. The shipped list follows this order.

If you insert, remove or reorder entries, a silo launches the [`WeaponType=`](/keys/weapontype/) of whichever section now holds that position. Two sections with the same missile behavior always launch the same missile. A list with fewer than six entries makes a chemical missile launch read past the end of the list.
:::

## Becoming available

A house updates which superweapons it holds after one of its structures is built, placed, sold, destroyed, captured, switched on or off, or fitted with a plug. On the house's next update, two passes run in order:

1. The first pass removes each weapon whose granting structures are gone, and suspends or resumes the rest. [Power output and drain](/systems/power/#superweapons) covers when a held weapon is suspended.
2. The second pass grants every weapon that the house's structures now provide.

A defeated house loses every weapon in the first pass, including one-time and trigger-granted weapons, and skips the second pass. When the house's power crosses the full-power line, only the first pass runs.

While the match's superweapons option is off, the second pass does not grant a weapon whose [`DisableableFromShell`](/keys/disableablefromshell/) is `yes`. The option is set by the [launch file](/formats/spawn-ini/) or the skirmish lobby.

### From a structure or a plug

A house gains a weapon it does not already hold when it owns a structure that grants the weapon and is active and out of [limbo](/glossary/#limbo). A structure grants the weapons its type names in [`SuperWeapon=`](/keys/superweapon/) and [`SuperWeapon2=`](/keys/superweapon2/), and the weapons named the same way by a plug fitted to it. For the local player, the cameo appears on the sidebar at the same time.

```ini title="rules.ini"
[NAMISL]        ; Missile Silo
SuperWeapon=MultiSpecial
SuperWeapon2=ChemicalSpecial
NukeSilo=yes

[GAPLUG3]       ; Ion Cannon Uplink, a plug for the GDI Upgrade Center
PowersUpBuilding=GAPLUG
SuperWeapon=IonCannonSpecial
```

[`AuxBuilding=`](/keys/auxbuilding/) adds a condition to the grant that comes from a structure's type. A weapon that names an `AuxBuilding=` is granted by a structure's `SuperWeapon=` or `SuperWeapon2=` only while its house owns at least one standing structure of that BuildingType.

:::caution[A plug ignores `AuxBuilding=`]
A plug grants its weapon without the `AuxBuilding=` check, and the house keeps the weapon on the same terms. In the shipped rules the ion cannon and the drop pods come only from plugs, so an `AuxBuilding=` on either has no effect. The hunter seeker comes both from a plug and from the Nod temple, and the check applies to the temple's grant.
:::

A weapon granted while its house is short of power arrives suspended, even with [`IsPowered=no`](/keys/ispowered/), and its `ChargingVoice=` does not play. [Power output and drain](/systems/power/#superweapons) covers suspension by power and what it costs a charge-draining weapon.

### One shot at a time

The [Add 1-time special weapon](/mapping/actions/taction-1-special/) trigger action grants a single-use copy of the weapon at the position it names. A [missile crate](/systems/crates/#what-each-result-does) also grants one; the warning below explains which weapon.

A one-time weapon is fully charged the moment it is granted and is never suspended. It needs no structure, and it is removed from the house as soon as it fires. No voice announces the grant.

A house that already holds the weapon gains nothing from either path. Its copy keeps its current charge and stays a repeating weapon.

:::danger[Keep `Type=MultiMissile` and `Action=Nuke` on one section]
The missile crate checks that the collecting house has a `Type=MultiMissile` weapon. It then grants the first section in the declared list that sets [`Action=Nuke`](/keys/action/#scope-superweapontype), which need not be the same section. If a `Type=MultiMissile` section exists and no section sets `Action=Nuke`, collecting the crate crashes the game. If no `Type=MultiMissile` section exists, the crate grants nothing. The shipped rules put both on `MultiSpecial`.
:::

### Granted outright

The [Add repeating special weapon](/mapping/actions/taction-full-special/) trigger action grants the weapon and frees it from structures. An ordinary weapon recharges after every shot for as long as the house lives, and a `ManualControl=yes` weapon still waits for its [next start](#manual-control). Losing or selling structures cannot remove the weapon, and low power cannot suspend it. Only defeat removes it.

If the house already holds the weapon, the action only frees it from structures. A one-time copy the house already holds stays one-time and is still removed when it fires.

:::caution[Granting a weapon that is on hold]
If the weapon is suspended when the action runs, it stays suspended for the rest of the match. Only the structure check can resume a suspended weapon, and the action removes the weapon from that check.
:::

## Charging

[`RechargeTime=`](/keys/rechargetime/) sets the charge delay in minutes, which the game converts at 900 frames a minute. The weapon's countdown starts at that delay, and the weapon is ready when the countdown reaches zero.

The table lists the events that change an ordinary weapon's countdown. The weed pool and charge-draining weapons are covered below. A stopped countdown does not run, so the weapon stays uncharged until another event starts it again.

| Event | Effect on the countdown |
| --- | --- |
| Granted, ordinary weapon | started at the full delay |
| Granted, `ManualControl=yes` | set to the full delay and stopped |
| Granted as a one-time weapon | set to zero, so the weapon is ready at once |
| Suspended | stopped where it stands |
| Resumed, ordinary weapon | continues from where it stopped |
| Resumed, `ManualControl=yes` | stays stopped |
| Resumed, `UseChargeDrain=yes` | restarted at the full delay |
| Fired, repeating weapon | restarted at the full delay |
| Fired, `ManualControl=yes` | set to the full delay and stopped |
| Fired, one-time weapon | the weapon is removed from the house |
| Fired, `PreClick=yes` weapon | unchanged; the weapon stays ready |
| Its `PostClick=yes` weapon fired | restarted at the full delay, or stopped there while suspended or with `ManualControl=yes`; a one-time weapon is removed |

Voices play only for the local player's weapons. [`ChargingVoice=`](/keys/chargingvoice/) plays when a countdown starts: when a structure grants the weapon, after each shot, and when the weed pool restarts a chemical missile. [`RechargeVoice=`](/keys/rechargevoice/) plays when the countdown reaches zero. No voice plays when a trigger action or a crate grants the weapon, or when a suspended weapon resumes. A charge-draining weapon plays `ChargingVoice=` only when a structure grants it; its later charge and drain cycles are silent.

:::caution[`RechargeTime=0` counts as unset]
A value of exactly `0` is ignored. The weapon keeps the delay an earlier rules file set, or five minutes if none did. A near-instant recharge needs a small positive value.
:::

### Manual control

[`ManualControl=yes`](/keys/manualcontrol/) stops the countdown when the weapon is granted and again after every shot, so the weapon never charges on its own. The only event that starts it is a full weed pool, and only for a `Type=ChemMissile` weapon. A weapon of any other behavior with `ManualControl=yes` never charges, although a one-time grant still arrives fully charged.

When a house's [weed pool](/systems/veins/#the-weed-pool) holds exactly [`WeedCapacity`](/keys/weedcapacity/) units, the first `Type=ChemMissile` weapon the house holds that is not ready restarts its countdown at the full delay, and the pool is emptied. This happens with or without `ManualControl=yes`, and a countdown that is already running starts over.

Two states change what a full pool does:

- A ready weapon leaves the pool full. The countdown restarts as soon as the weapon fires.
- A suspended weapon still empties the pool, but its countdown does not start.

### Countdown timers

Every player sees the countdown of each [`ShowTimer=yes`](/keys/showtimer/) superweapon that any house holds, listed in the bottom right corner of the battlefield in the owner's colors, as the weapon's name followed by the minutes and seconds left, with hours in front once there are any. A weapon that is ready shows `00:00`. A house whose country is [`MultiplayPassive=yes`](/keys/multiplaypassive/) shows none, and in a campaign a suspended weapon that has not started charging is left out.

## Charge-draining weapons

[`UseChargeDrain=yes`](/keys/usechargedrain/) gives a weapon three states: charging, ready and discharged. Firing a ready weapon delivers its `Type=` effect and moves it to the discharged state, where the countdown measures how long the effect lasts. Firing it again during the drain returns it to ready. When the drain runs out, the weapon returns to charging with a full delay on the countdown, and the effect ends.

[`ChargeToDrainRatio`](/keys/chargetodrainratio/) converts between charge and drain time:

- On firing, the charge built so far (`RechargeTime` minus the time left on the countdown) is multiplied by the ratio and becomes the drain time.
- On firing again, the unspent drain is divided by the ratio and returned as charge.

An effect switched off the moment it starts therefore gives back the full charge. An effect switched off with half its drain left gives back half. A weapon returned to ready can be fired again at once, but its next drain lasts only as long as the charge it holds allows. Meanwhile its countdown keeps running toward a full charge.

A charge-draining weapon always shows a clock on its cameo and never shows the plain ready face. It can be fired while ready or discharged, but not while charging or suspended.

This cycle runs only for a house under human control. For any other house, firing the weapon only switches the effect on or off; the state and the countdown do not change.

### The firestorm defense

`Type=Firestorm` is the only behavior built for this cycle. Firing the weapon raises every [`FirestormWall=yes`](/keys/firestormwall/) structure its house owns, and firing it again lowers them.

Two events outside the cycle also change the wall. Damage aimed at a raised section [shortens the countdown](/systems/laser-fences/#damage-while-the-wall-is-up) through [`DamageToFirestormDamageCoefficient`](/keys/damagetofirestormdamagecoefficient/). Losing the last working [`GDIFirestormGenerator`](/keys/gdifirestormgenerator/) structure [lowers the wall](/systems/laser-fences/#losing-the-generator), as if the weapon had been fired again.

[The firestorm wall](/systems/laser-fences/#the-firestorm-wall) covers what a raised section does to what it touches, through [`FirestormWarhead`](/keys/firestormwarhead/) and the animations beside it.

## Firing it

### The sidebar cameo

A superweapon's cameo is the shape file named by [`SidebarImage=`](/keys/sidebarimage/), or `XXICON.SHP` when that file cannot be found. A readable PCX picture named by [`SidebarPCX=`](/keys/sidebarpcx/) is drawn on the sidebar instead. The cameo shows no numeric countdown. Its charge appears as a clock and a short caption. [The sidebar](/systems/sidebar/) covers where the cameo sits, how it is announced, how it is captioned, and when it leaves.

The caption depends on the weapon's state. A charge-draining weapon has a caption while it charges and a fourth state that ordinary weapons lack.

| Weapon state | Caption |
| --- | --- |
| Suspended | "On Hold" |
| Charging | none, or "Charging..." for a charge-draining weapon |
| Ready | "Ready", or "Release" for `Type=HunterSeeker` |
| Discharged, charge-draining | "Activated" |

The clock is drawn while the weapon is not fully charged, and on every charge-draining weapon. It uses the charge-up art while the weapon cannot be fired and the discharge art once it can.

Clicking a cameo that cannot be fired plays [`SuspendVoice=`](/keys/suspendvoice/) when its countdown is stopped and [`ImpatientVoice=`](/keys/impatientvoice/) otherwise. A `ManualControl=yes` weapon waiting for its next start has a stopped countdown, so it plays `SuspendVoice=`.

### Aiming and the click

[`Action=`](/keys/action/#scope-superweapontype) decides what clicking a cameo that can be fired does:

- `Action=None` fires the weapon at once at cell 0,0, with no targeting step. The shipped firestorm and hunter seeker are fired this way.
- Any other value arms targeting mode, deselects everything, and plays the select-target announcement.

While targeting mode is armed, the cursor over the map shows the weapon's `Action=` in place of the usual cursor. An EM pulse also shows an out-of-range cursor, which [the EM pulse cannon](/systems/emp-pulse/#em-pulse-cannon-superweapon) covers.

Releasing the left button fires the weapon at the cell under the pointer. A right click on the cameo or on the map cancels targeting.

Band selection is off while targeting mode is armed. The minimap does not accept superweapon actions, so a shot cannot be aimed there.

A left click fires the first section in the declared list whose `Action=` matches the click's action. This happens whether or not targeting mode is armed, and whichever cameo armed it. Two sections that share one `Action=` therefore always fire the earlier section. Clicking the later section's cameo arms targeting, but the shot fires the earlier section, or nothing if that section is not ready.

If a section's `Action=` is one that ordinary orders also use, the player's next such order fires the weapon whenever it is charged. For example, a ready weapon with `Action=Attack` fires at the target of the player's next attack order.

:::caution[A misspelled `Action=` becomes `None`]
An `Action=` value the engine does not recognize reads as `None`. Clicking the charged cameo then fires the weapon at once at cell 0,0, with no way to choose where the effect lands.
:::

### Two-click weapons

The chronosphere takes two clicks: the first picks the units, and the second picks where they go. Each click fires a separate section, paired by three keys:

- The first section is [`PreClick=yes`](/keys/preclick/) with `Type=ChronoSphere`.
- The second section is [`PostClick=yes`](/keys/postclick/) with [`PreDependent=ChronoSphere`](/keys/predependent/) and `Type=ChronoWarp`.

```ini title="rulesmd.ini"
[ChronoSphereSpecial]
Type=ChronoSphere
Action=ChronoSphere
PreClick=yes

[ChronoWarpSpecial]
Type=ChronoWarp
Action=ChronoWarp
PostClick=yes
PreDependent=ChronoSphere
```

For the player, the first click arms targeting mode again, now for the first `PostClick=yes` section whose `PreDependent=` names `ChronoSphere`. The first section stays charged. A right click cancels the second click and leaves it ready for a new first click.

The second section needs no structure, charge or cameo of its own. It fires only while its house holds a weapon of the `PreDependent=` behavior, fully charged, and it acts on the cells that weapon last picked. That weapon's charge is then spent, as the [charging table](#charging) shows. When a house holds several weapons of that behavior, the first in `[SuperWeaponTypes]` is used.

### The computer's use

A computer house fires its ready superweapons during its periodic AI pass, which runs every 7 to 7.5 seconds. Outside a campaign, the pass always fires them. In a campaign, it fires them only when the house's [`IQ=`](/keys/iq/) is at least [`SuperWeapons`](/keys/superweapons/) in `[IQ]`. A campaign house takes its `IQ=` from its section in the scenario, and has 0 when the section sets none.

Each ready weapon goes to the handler for its `Type=`. There is no handler for `EMPulse` or `Firestorm`, so the computer never fires either on its own. Its firestorm wall goes up only through the [Activate Firestorm Defense](/mapping/actions/taction-activate-firestorm/) trigger action.

Every handler waits until the house has a [declared enemy](/systems/base-attacked/#picking-a-first-enemy). In a campaign the computer does not pick an enemy on its own, so a campaign house has none until damage or a trigger makes it angry at someone. Until then its superweapons stay charged and unused.

- **Multi missile**, the nuke: we aim it at the cell the ion cannon rating picks, as the lightning storm does, unless the house's [preferred target](#the-preferred-target) is another quarry. It fires only while the house has an enemy.
- **Chem missile** targets the enemy structure whose cell rates highest on the firing house's [threat map](/systems/base-attacked/#the-threat-map). A structure at full translucency, the last step of a cloak's fade, is rated at random from 0 to 100 instead. Every structure the enemy owns is considered, including one in [limbo](/glossary/#limbo).
- **Hunter seeker** is released with no target; the drone chooses one itself.
- **Drop pods** land around the computer's *own* base, not the enemy's. The handler picks a random point in one of four compass quadrants, one to two base radii from the base's center, with the radius held between 3 and 8 cells. It then aims at the nearest cell to that point that infantry can enter.
- **Ion cannon** rates every enemy object and strikes one of the highest rated.
- **Lightning storm**: we aim it as the ion cannon does, or by the [preferred target](#the-preferred-target) when that is another quarry, and only while no storm is raging or waiting to break and the house has an enemy.
- **Paradrops, the spy plane and the psychic reveal** aim near the center of the enemy's base, or of the computer's own base when it has no enemy: at the nearest cell with clear ground for a five by five group of infantry, moved two cells along each map axis. When the [preferred target](#the-preferred-target) is another quarry, they aim by it instead.
- **Genetic mutator** aims at the infantryman, of any house, with the most infantry of other, unallied houses on its cell and the cells around it. We do not fire it while a trigger has aimed the house's weapons.
- **Psychic dominator** aims at the object, of any house, with the most enemy units it could take over within about three cells. It waits while a dominator blast is still running. We hold it while a trigger has aimed the house's weapons.

The computer never fires the force shield or the chronosphere on its own. Yuri's Revenge computer teams use the chronosphere through the [Chrono prep for ABwP](/scripting/missions/56/) and [Chrono prep for AQ](/scripting/missions/57/) script lines.

The ion cannon's rating is the only one with settings. We use it for the nuke as well, and for the lightning storm, while the preferred target is Anything.

Only enemy objects that are on the ground layer, active and out of [limbo](/glossary/#limbo) are candidates. In difficulty slot 0, an object still being built also counts, if its factory is producing and not on hold.

We start each enemy object at a rating of 1. The table below replaces that figure for the kinds it covers, whatever the object's strength, so a damaged object and a healthy one of the same kind are rated alike. We rate an object outside the playable area 0. A cloaked object is rated at random even there.

Only the highest rating matters. The computer collects every candidate that ties for the highest rating and strikes one of them at random. Apart from cloaked objects, described below, a table value of `4` and one of `40` therefore select the same target when nothing else rates 4 or higher. A table value replaces the starting rating; it is not added to it. A value below another candidate's rating ranks the object below that candidate. For example, an [`AIIonCannonConYardValue`](/keys/aiioncannonconyardvalue/) of `2` ranks every construction yard below an ordinary structure, which takes the fixed `4`.

The rows are tested from the top for each kind of object, and the first match wins. A base defense that also produces vehicles is therefore rated as a war factory. Rows marked as fixed cannot be changed by any rules file. Each per-difficulty list is read at the position of the *firing* house's [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), not the target's. None of these lists has a built-in value, so each needs one entry for every difficulty.

| Candidate | Rating | Where the figure comes from |
| --- | --- | --- |
| Engineer | [`AIIonCannonEngineerValue`](/keys/aiioncannonengineervalue/) | Per-difficulty list |
| Vehicle thief | [`AIIonCannonThiefValue`](/keys/aiioncannonthiefvalue/) | Per-difficulty list |
| Any other infantry | `2` | Fixed in the engine |
| [`Factory=BuildingType`](/keys/factory/) structure | [`AIIonCannonConYardValue`](/keys/aiioncannonconyardvalue/) | Per-difficulty list |
| `Factory=UnitType` structure | [`AIIonCannonWarFactoryValue`](/keys/aiioncannonwarfactoryvalue/) | Per-difficulty list |
| Structure whose rated output beats its rated drain | [`AIIonCannonPowerValue`](/keys/aiioncannonpowervalue/) | Per-difficulty list |
| [`IsBaseDefense=yes`](/keys/isbasedefense/) structure | [`AIIonCannonBaseDefenseValue`](/keys/aiioncannonbasedefensevalue/) | Per-difficulty list |
| [`IsPlug=yes`](/keys/isplug/) structure | [`AIIonCannonPlugValue`](/keys/aiioncannonplugvalue/) | Per-difficulty list |
| [`IsTemple=yes`](/keys/istemple/) structure | [`AIIonCannonTempleValue`](/keys/aiioncannontemplevalue/) | Per-difficulty list |
| [`HoverPad=yes`](/keys/hoverpad/) structure | [`AIIonCannonHelipadValue`](/keys/aiioncannonhelipadvalue/) | Per-difficulty list |
| Structure listed in [`BuildTech`](/keys/buildtech/) | [`AIIonCannonTechCenterValue`](/keys/aiioncannontechcentervalue/) | Per-difficulty list |
| Any other structure | `4` | Fixed in the engine |
| [`Harvester=yes`](/keys/harvester/) vehicle | [`AIIonCannonHarvesterValue`](/keys/aiioncannonharvestervalue/) | Per-difficulty list |
| Vehicle whose [`DeploysInto`](/keys/deploysinto/) is a [`BuildConst`](/keys/buildconst/) type | [`AIIonCannonMCVValue`](/keys/aiioncannonmcvvalue/) | Per-difficulty list |
| Vehicle with [`Passengers`](/keys/passengers/) above zero | [`AIIonCannonAPCValue`](/keys/aiioncannonapcvalue/) | Per-difficulty list |
| Any other vehicle | `2` | Fixed in the engine |

No row covers aircraft, so an aircraft on the ground is a candidate rated 1 however badly damaged it is.

A cloaked object, or a structure at full translucency, takes a random rating instead, from 0 up to ten above the best rating found so far in the scan. It can therefore outrate everything scanned before it, and its chance depends on its place in the scan. The higher the best rating so far, the less likely the draw is to beat it, so large table values make cloaked objects rarely chosen. This rule is separate from the 0 to 100 draw the chem missile handler uses.

### The preferred target

Each house stores one quarry, its preferred target, which starts as Anything. The [Preferred target](/mapping/actions/taction-preferred-target/) action sets it, and a save game keeps it. A computer house's nuke, lightning storm and paradrops aim by it, unless a [preferred target cell](/mapping/actions/taction-set-preferred-target-cell/) already aims the weapon at a cell.

- **Anything** leaves the aim to the rules above: the ion cannon rating for the nuke and the lightning storm, and the enemy's base for the paradrops.
- **Any other quarry** makes the house look through its team list for the first attack team it owns. The team's leader, as an attack team picks it (see [`LeadershipRating`](/keys/leadershiprating/)), is the point the search starts from. The search takes the greatest threat of the quarry's kind that is an enemy, anywhere on the map, and the cell of that threat is the target. The kind each quarry scans for is the one a team's Attack line uses, as [the quarry enum](/reference/enums/quarry/) lists it. Anything and Base Threats scan for any enemy.

A house with no attack team, or whose search finds no enemy, strikes nothing. The nuke and the lightning storm then stay charged and the next AI pass tries again. A house with no enemy does not fire the nuke or the storm, whatever its quarry.

## Announcements

With [EVAMD.INI](/formats/eva-ini/), the announcer speaks the Yuri's Revenge line for each weapon's behavior, and [`RechargeVoice=`](/keys/rechargevoice/) is not used. The ready line plays to the weapon's owner. The fired line plays to every player.

| Behavior | Ready | Fired |
| --- | --- | --- |
| `MultiMissile` | `EVA_NuclearMissileReady` | `EVA_NuclearMissileLaunched`, unless no silo takes a repeating weapon's order |
| `IronCurtain` | `EVA_IronCurtainReady` | `EVA_IronCurtainActivated` |
| `LightningStorm` | `EVA_LightningStormReady` | `EVA_LightningStormCreated` |
| `ChronoSphere` | `EVA_ChronosphereReady` | `EVA_ChronosphereActivated`, when the warp fires |
| `ParaDrop`, `AmerParaDrop` | `EVA_ReinforcementsReady` | none |
| `SpyPlane` | `EVA_SpyPlaneReady` | none |
| `PsychicDominator` | `EVA_PsychicDominatorReady` | `EVA_PsychicDominatorActivated` |
| `GeneticConverter` | `EVA_GeneticMutatorReady` | `EVA_GeneticMutatorActivated` |
| `ForceShield` | `EVA_ForceShieldReady` | none |
| `PsychicReveal` | `EVA_PsychicRevealReady` | none |

Outside a campaign, the player also hears when a house that is not an ally places or captures a structure whose [`SuperWeapon=`](/keys/superweapon/) it could fire: `EVA_NuclearSiloDetected`, `EVA_IronCurtainDetected`, `EVA_WeatherDeviceReady` for a lightning storm, `EVA_ChronosphereDetected`, `EVA_PsychicDominatorDetected` or `EVA_GeneticMutatorDetected`. Other behaviors, `SuperWeapon2=`, and a weapon whose `AuxBuilding=` the house lacks are not announced.

## What each behavior delivers

`Type=DropPod` calls the [drop-pod delivery](/systems/drop-pods/#drop-pods-superweapon) on the chosen cell, and `Type=Firestorm` toggles [the firestorm defense](#the-firestorm-defense). This section covers the other behaviors.

### Ion cannon

The blast lands on the target cell, at the height of that cell's terrain. It plays [`IonBlast`](/keys/ionblast/), or the last entry of [`SplashList`](/keys/splashlist/) over water, and always plays [`IonBeam`](/keys/ionbeam/).

The blast deals `IonCannonDamage` through [`IonCannonWarhead`](/keys/ioncannonwarhead/) with no attacker. The warhead's [`Verses`](/keys/verses/) table, its spread falloff and [`Immune=yes`](/keys/immune/) all apply, and no kill is credited. A bright warhead also lights the scene. A cell under a bridge is hit twice: once at bridge height and once at ground level.

A shockwave then rolls outward and stops the infantry and vehicles it passes near the target. Vehicles driving out of a war factory, or standing on its exit cells, keep moving.

### Multi missile and chem missile

A repeating missile launches from a silo. The engine finds the first BuildingType with [`NukeSilo=yes`](/keys/nukesilo/) that names this weapon in `SuperWeapon=` or `SuperWeapon2=`, and uses one of the house's structures of that type. If the house owns none, nothing launches, but the charge is spent. Only that first type is searched, so a silo of a later type that grants the same weapon never launches it.

The house stores a single missile target. A second launch before the first silo has launched its missile therefore redirects that silo. A missile already in flight keeps its target.

The silo opens its door and launches the missile on the same frame, without waiting for the door to finish opening. It then closes the door and returns to guard. The projectile, warhead, maximum speed and range come from the `WeaponType=` of the section the [declaration warning](#declaring-a-superweapon) describes. The silo ignores that weapon's `Damage=` and gives the projectile a fixed strength of 200. The missile leaves five eighths of a cell (160 leptons) north of the silo's center, pointing straight up, and [`NukeTakeOff`](/keys/nuketakeoff/) plays there. When the launching house is not the local player's, the player hears the launch-detected announcement.

In Yuri's Revenge the missile is a [`Vertical=yes`](/keys/vertical/) projectile with a [`NukeMaker=yes`](/keys/nukemaker/) warhead. It climbs off the top of the screen and explodes at its [`DetonationAltitude`](/keys/detonationaltitude/), and the `NukePayload` weapon's projectile then falls onto the target from the same height and deals the payload's damage.

```ini title="rulesmd.ini"
[NukeSpecial]
Type=MultiMissile
WeaponType=NukeCarrier

[NukeCarrier]          ; climbs out of the silo
Projectile=GiantNukeUp ; Vertical=yes, DetonationAltitude=20000
Warhead=NukeMaker      ; NukeMaker=yes

[NukePayload]          ; falls on the target
Projectile=GiantNukeDown
Warhead=NUKE
Damage=600
```

A one-time missile needs no silo. It enters from the map edge nearest the target. It is built from the hard-coded weapon `MultiLauncher` or `ChemLauncher`, according to the behavior, and deals that weapon's `Damage=`. It is fired with a range of `100000` leptons, longer than any map is wide.

### Hunter seeker

The drone comes out of the house's structure whose type is listed in [`HSBuilding`](/keys/hsbuilding/). If the house owns several, the newest is used. If it owns none, the charge is spent and nothing launches.

The drone appears at the nearest cell to that structure that infantry could enter, even though the drone is a vehicle. If that cell lies outside the playable area, the region a scenario declares with `[Map] LocalSize=`, or the drone cannot be placed there, nothing launches and the charge is spent. A placed drone chooses a target and attacks it.

The drone type is the [`HunterSeeker`](/keys/hunterseeker/#scope-side) of the side the firing house [acts as](/keys/actslike/). In each rules file, [`GDIHunterSeeker`](/keys/gdihunterseeker/) and [`NodHunterSeeker`](/keys/nodhunterseeker/) in `[General]` set it for the first two sides in `[Sides]`, and a side's section in the same file overrides them. A house with no side, or whose side names no drone, spends the charge and launches nothing.

### Iron curtain

A `Type=IronCurtain` weapon plays [`IronCurtainInvokeAnim`](/keys/ironcurtaininvokeanim/) over the target cell, then covers every object on that cell and the eight cells around it. On a cell with a bridge, only the objects on the bridge are covered. What happens to each covered object depends on its kind:

- Infantry die, and the firing house is credited with the kill.
- A vehicle or aircraft with [`Organic=yes`](/keys/organic/) takes damage equal to its full strength, which its armor can reduce.
- Anything else is protected for [`IronCurtainDuration`](/keys/ironcurtainduration/) frames. A structure's demolition charge is also defused. A vehicle or aircraft also loses its paralysis, and a parasite inside it dies.

A protected object takes no damage, except from damage that ignores armor, such as a demolition charge going off. Healing still reaches it. Infantry cannot plant a demolition charge on a protected structure or walk into one, so an engineer cannot capture it.

A protected object pulses dark while the protection lasts: it flashes bright as the protection starts, throbs dark, and flashes bright again in its last second. Yuri's Revenge also tints it with `IronCurtainColor`, which is not drawn yet.

```ini title="rulesmd.ini"
[MyCurtainSpecial]
Type=IronCurtain
Action=IronCurtain
RechargeTime=5

[CombatDamage]
IronCurtainDuration=750 ; 50 seconds at normal game speed
```

Computer houses do not fire the Iron Curtain on their own. A computer team asks for it with the [Iron Curtain me](/scripting/missions/55/) script line.

### Lightning storm

A `Type=LightningStorm` weapon calls a storm over the target cell. Only one storm exists at a time: a shot while a storm rages or waits to break does nothing, and the weapon stays charged. A shot the player fires then also prints that a storm is already active; a shot from a computer house prints nothing.

The storm breaks [`LightningDeferment`](/keys/lightningdeferment/) frames after the shot. While it waits, and while [`LightningPrintText`](/keys/lightningprinttext/) is `yes`, the EVA warning plays and a message appears each time the frames left are a multiple of 225. It then rages for [`LightningStormDuration`](/keys/lightningstormduration/) frames:

- The map darkens to the scenario's ion storm lighting. When the storm breaks, [`StormSound`](/keys/stormsound/) plays and a message appears, if `LightningPrintText` is `yes`.
- Every house that is not an ally of the firing house loses its radar for the storm's duration.
- Every [`LightningHitDelay`](/keys/lightninghitdelay/) frames, a cloud gathers over the center.
- Every [`LightningScatterDelay`](/keys/lightningscatterdelay/) frames, a cloud gathers over a random cell up to half of [`LightningCellSpread`](/keys/lightningcellspread/) cells from the center along each axis. A cell closer than [`LightningSeparation`](/keys/lightningseparation/) cells to an existing cloud is passed over; after three such cells the chance is lost.

A cloud is one of [`WeatherConClouds`](/keys/weatherconclouds/), hung high enough for a bolt to reach the ground. Once the cloud's animation is past halfway, a bolt from [`WeatherConBolts`](/keys/weatherconbolts/) strikes the cell below, and the cloud stays until its last frame. The strike plays one of [`LightningSounds`](/keys/lightningsounds/) and [`WeatherConBoltExplosion`](/keys/weatherconboltexplosion/), and deals [`LightningDamage`](/keys/lightningdamage/) through [`LightningWarhead`](/keys/lightningwarhead/) with no attacker. The house that called the storm is credited with what the strike destroys, as [kills credited to a house alone](/systems/veterancy/#kills-credited-to-a-house-alone) describes. A strike that hits empty road, rock, wall or weeds, or changes what stands in the cell, throws up two to four [`MetallicDebris`](/keys/metallicdebris/) animations, unless an infantryman stood there.

The lighting returns to normal once the duration is over and the last cloud has gone.

```ini title="rulesmd.ini"
[General]
LightningDeferment=250     ; frames of warning
LightningStormDuration=180 ; frames the storm rages
LightningHitDelay=10       ; a cloud over the center this often
LightningScatterDelay=5    ; a cloud nearby this often
LightningCellSpread=10
LightningSeparation=3
LightningDamage=250
LightningWarhead=IonWH
```

### Psychic reveal

A `Type=PsychicReveal` weapon uncovers the map for the firing house, shroud and fog, out to [`PsychicRevealRadius`](/keys/psychicrevealradius/) cells from the target, and plays [`PsychicRevealActivateSound`](/keys/psychicrevealactivatesound/) there.

### Genetic mutator

A `Type=GeneticConverter` weapon plays [`IonBlast`](/keys/ionblast/) over the target and [`GeneticMutatorActivateSound`](/keys/geneticmutatoractivatesound/). What it hits depends on [`MutateExplosion`](/keys/mutateexplosion/):

- With `MutateExplosion=yes`, a blast of 10000 damage goes off through [`MutateExplosionWarhead`](/keys/mutateexplosionwarhead/), so that warhead's [`CellSpread`](/keys/cellspread/) and `Verses` decide who is caught.
- Otherwise, every infantryman on the target cell and the eight cells around it takes its full strength as damage through [`MutateWarhead`](/keys/mutatewarhead/), ignoring armor.

A warhead with [`InfDeath=9`](/keys/infdeath/) mutates the infantry it kills: each leaves [`InfantryMutate`](/keys/infantrymutate/), which becomes a new infantryman of the firing house when it ends, as [`MakeInfantry`](/keys/makeinfantry/) describes. The firing house is credited with every infantryman killed.

### Force shield

A `Type=ForceShield` weapon plays [`ForceShieldInvokeAnim`](/keys/forceshieldinvokeanim/) over the target and the weapon's [`StartSound`](/keys/startsound/#scope-superweapontype). Every structure of the firing house or its allies whose center is less than [`ForceShieldRadius`](/keys/forceshieldradius/) cells from the target is protected for [`ForceShieldDuration`](/keys/forceshieldduration/) frames, exactly as the [Iron Curtain](#iron-curtain) protects it. The firing house then makes no power for [`ForceShieldBlackoutDuration`](/keys/forceshieldblackoutduration/) frames. The weapon's [`SpecialSound`](/keys/specialsound/) plays [`ForceShieldPlayFadeSoundTime`](/keys/forceshieldplayfadesoundtime/) frames before the protection ends.

Yuri's Revenge tints shielded structures with `ForceShieldColor`, which is not drawn yet.

### Psychic dominator

A `Type=PsychicDominator` weapon plays [`DominatorFirstAnim`](/keys/dominatorfirstanim/) over the target and [`PsychicDominatorActivateSound`](/keys/psychicdominatoractivatesound/). Nothing happens unless both [`DominatorFirstAnim`](/keys/dominatorfirstanim/) and [`DominatorSecondAnim`](/keys/dominatorsecondanim/) are set.

Once the first animation has played [`DominatorFireAtPercentage`](/keys/dominatorfireatpercentage/) percent of its frames, the blast fires:

- a shockwave ripples out from the target, and `DominatorSecondAnim` plays there;
- [`DominatorDamage`](/keys/dominatordamage/) goes off through [`DominatorWarhead`](/keys/dominatorwarhead/), credited to the firing house;
- every vehicle, infantryman and aircraft within [`DominatorCaptureRange`](/keys/dominatorcapturerange/) cells joins the firing house for good, with [`PermaControlledAnimationType`](/keys/permacontrolledanimationtype/) shown [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above it. A computer house sends its new units hunting. A unit under [mind control](/systems/mind-control/#letting-go) is let go first, and nothing can take a dominated unit over afterwards. Taking a unit over springs no [`Entered by...`](/mapping/events/tevent-player-entered/) trigger. An infantryman or aircraft taken over this way also springs the [destroyed-any](/mapping/events/tevent-destroyed-any/) trigger events. Every unit taken over counts as lost for its old house and as a kill for the dominator's house, unless its type is [`DontScore=yes`](/keys/dontscore/); a `DontScore` type still springs those events.

Structures, objects in the air or under the Iron Curtain, and types with [`ImmuneToPsionics=yes`](/keys/immunetopsionics/) or [`BalloonHover=yes`](/keys/balloonhover/) are not taken over. Only one blast runs at a time: a shot while one runs is refused, and the weapon stays charged. A shot the player fires then prints that the dominator is already active, as Yuri's Revenge does. A computer house's shot prints nothing. Yuri's Revenge dims the map's lighting during the blast, which is not done yet.

### Paradrops

A `Type=ParaDrop` or `Type=AmerParaDrop` weapon calls in cargo planes that drop infantry by parachute around the target cell. A shot aimed at water lands at the nearest cell infantry can enter, if that cell is dry.

The infantry come from a pair of `[General]` lists, one naming the types and one the number of each:

| Weapon and firing house | Lists |
| --- | --- |
| `Type=AmerParaDrop`, any house | [`AmerParaDropInf`](/keys/amerparadropinf/) and [`AmerParaDropNum`](/keys/amerparadropnum/) |
| `Type=ParaDrop`, a house of the first side in `[Sides]` | [`AllyParaDropInf`](/keys/allyparadropinf/) and [`AllyParaDropNum`](/keys/allyparadropnum/) |
| `Type=ParaDrop`, a house of the third side | [`YuriParaDropInf`](/keys/yuriparadropinf/) and [`YuriParaDropNum`](/keys/yuriparadropnum/) |
| `Type=ParaDrop`, a house of any other side | [`SovParaDropInf`](/keys/sovparadropinf/) and [`SovParaDropNum`](/keys/sovparadropnum/) |

Each type in the list sends one plane of the `PDPLANE` aircraft type, carrying the number at the same position in the second list. When the two lists differ in length, or there is no `PDPLANE` type, the shot spends its charge and no plane comes.

```ini title="rulesmd.ini"
[ParaDropSpecial]
Type=ParaDrop
Action=ParaDrop
RechargeTime=4

[General]
AllyParaDropInf=E1,GGI
AllyParaDropNum=6,2   ; two planes: six GIs and two guardian GIs
```

The planes fly in from the firing house's [`Edge`](/keys/edge/), which is north outside a campaign. Within [`ParadropRadius`](/keys/paradropradius/) leptons of the target, measured along the ground, a plane drops a paratrooper every 5 frames while it is over the playable map. Each one lands half a cell to the left or right of the plane's path, alternating sides, and falls under the [`Parachute`](/keys/parachute/) canopy with [`ChuteSound`](/keys/chutesound/). A paratrooper that could not stand where it would land stays aboard.

A plane that leaves the radius with paratroopers still aboard turns back for another pass. It gives up after five passes in a row in which nobody could jump. A plane that is empty or has given up flies back to its own edge and leaves the map.

A paratrooper of a player's house guards where it lands; one of a computer house hunts.

### Spy plane

A `Type=SpyPlane` weapon sends planes of the `SPYP` aircraft type from the firing house's [`Edge`](/keys/edge/) over the target cell. It sends one plane for each entry of [`AllyParaDropInf`](/keys/allyparadropinf/), whatever the firing house's side. It sends none when `AllyParaDropInf` and [`AllyParaDropNum`](/keys/allyparadropnum/) differ in length or there is no `SPYP` type.

While a plane is within its primary weapon's `Range` of the target, it photographs: it maps the ground for its house around the point below it, out to the weapon's `Damage` in cells, at most 10.

- On the way to the target it photographs every [`SpyPlaneCameraFrames`](/keys/spyplanecameraframes/) frames and plays [`SpyPlaneCamera`](/keys/spyplanecamera/) each time.
- Within three cells of the target it turns for the map edge opposite its house's edge, photographing every 3 frames without the sound, and is removed once it leaves the map.

```ini title="rulesmd.ini"
[SPYP]
Primary=SpyCameraWeapon

[SpyCameraWeapon]
Range=20  ; photographs within 20 cells of the target
Damage=6  ; maps 6 cells around the point below the plane
```

### Chronosphere

A `Type=ChronoSphere` weapon picks the units to move, and a `Type=ChronoWarp` weapon moves them. [Two-click weapons](#two-click-weapons) covers how the two shots pair up. The first shot plays [`ChronoPlacement`](/keys/chronoplacement/) over the picked cell, which only the player sees, and only while aiming the second shot.

When the warp fires, [`ChronoBlast`](/keys/chronoblast/) plays over the picked cell and [`ChronoBlastDest`](/keys/chronoblastdest/) over the target. Then each vehicle, infantryman and landed aircraft on the picked cell and the eight cells around it is handled in turn. On a cell with a bridge, only those on the bridge count.

- An [`Organic=yes`](/keys/organic/) unit, which every infantryman is by default, is destroyed unless its type is [`Teleporter=yes`](/keys/teleporter/). The firing house is credited with the kill.
- A unit under the Iron Curtain, and a vehicle standing on a war factory, stays where it is. So does a unit whose type sets [`Chronoshift.Allow=no`](/keys/chronoshift.allow/), even when it is organic.
- Every other unit moves to the cell in the same position relative to the target, keeping its place within the cell. [`WarpOut`](/keys/warpout/) plays where it leaves and where it lands, with [`ChronoOutSound`](/keys/chronooutsound/) and [`ChronoInSound`](/keys/chronoinsound/).

What stands where a unit lands decides what happens to it:

- Another vehicle, infantryman or aircraft is destroyed, unless the warp is moving it too. An arriving infantryman destroys only infantry on its own spot in the cell.
- If that object's type sets [`Chronoshift.Crushable=no`](/keys/chronoshift.crushable/), the arriving unit is destroyed instead and everything on the cell stays. This also happens to an arriving infantryman when [`ChronoInfantryCrush=no`](/keys/chronoinfantrycrush/) and the object is a vehicle or aircraft. Neither key is read when a structure or terrain object is on the cell, because the unit then moves to another cell.
- Anything under the Iron Curtain or the force shield destroys the arriving unit instead.
- A structure or a terrain object, such as a tree, sends the arriving unit to the nearest cell it could stand on. With no such cell, the unit is destroyed.

A unit landing on a cell with a bridge lands on the bridge. A unit whose landing spot is off the map is destroyed. A vehicle set down on water it cannot cross sinks, and any other unit set down where it cannot move, such as on a cliff, is destroyed.

A unit destroyed at its landing spot credits no house.

A moved unit stays in its team and stays selected, but it stops where it lands and forgets its move order.

### EM pulse

The shot goes to the house's [EM pulse cannon](/systems/emp-pulse/#em-pulse-cannon-superweapon) nearest the target. That page covers everything the pulse then does.

:::caution[A one-time EM pulse launches nothing]
An EM pulse granted by the [Add 1-time special weapon](/mapping/actions/taction-1-special/) trigger action or by a missile crate spends its charge and is removed from the house, but no cannon fires. The same section granted by a structure or by [Add repeating special weapon](/mapping/actions/taction-full-special/) launches normally.
:::

## Scripting

Two trigger actions grant a weapon, and [becoming available](#becoming-available) covers what each one does. Two more toggle the firestorm defense. [Activate Firestorm Defense](/mapping/actions/taction-activate-firestorm/) and [Deactivate Firestorm Defense](/mapping/actions/taction-deactivate-firestorm/) each fire the house's first `Type=Firestorm` weapon at cell 0,0. Each does nothing when the wall is already in the state it asks for.

Three strike actions do not use superweapons at all: [Ion-cannon strike](/mapping/actions/taction-ion-cannon/), [Nuke strike](/mapping/actions/taction-multi-missile/) and [Chem-missile strike](/mapping/actions/taction-chem-missile/). They create the effect directly at the waypoint, so they need no weapon, no charge, no silo and no house that owns one.

The [Preferred target](/mapping/actions/taction-preferred-target/) action changes where a computer house's nuke, lightning storm and paradrops aim, as [the preferred target](#the-preferred-target) describes. It does not change how the computer aims any other behavior, and a human house's weapons are never aimed by it.

## Parsed settings without effect

[`NukeProjectile`](/keys/nukeprojectile/) and [`NukeDown`](/keys/nukedown/) in `[SpecialWeapons]` are read but have no effect; a silo takes its projectile from a section's `WeaponType=` instead. [`EMPulseWarhead`](/keys/empulsewarhead/) and [`EMPulseProjectile`](/keys/empulseprojectile/) in the same section have no effect either. [The EM pulse cannon](/systems/emp-pulse/#em-pulse-cannon-superweapon) covers them.
