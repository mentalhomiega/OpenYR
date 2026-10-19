# Keys OpenYR reads and gamemd does not

OpenYR reads the INI keys listed here, and Yuri's Revenge (`gamemd.exe`) does
not. A mod or map that sets one of them behaves differently in OpenYR, and the
original game ignores it.

Each key links to its page in the manual, which owns the details. The default
and the effect come from that page. In the tables, `rules.ini`, `art.ini` and
`ra2md.ini` stand for `rulesmd.ini`, `artmd.ini` and `RA2MD.INI`. A key that
appears under two headings, such as `AttachEffect.Duration`, is read in both
places.

We build this page with `python manual/tools/openyr_keys.py` from the manual's
key catalog and change records. Do not edit it by hand.

## How we pick the keys

A key is listed when a change record in `manual/changes/` adds it and
`manual/tools/openyr_keys.yaml` says gamemd does not read it. That file holds a
decision for each record. A record whose name starts with `yr-` ports gamemd
behavior, so its keys stay out. A record whose keys we could not place is also
left out until someone who knows the gamemd rules decides it;
`python manual/tools/openyr_keys.py --report` lists those records.

Each decision rests on what the change record says, not on a check of gamemd.
If a listed key turns out to be one gamemd reads, move its record to
`gamemd_records` in `openyr_keys.yaml`.

The From column says where a key comes from:

| From | Meaning |
| --- | --- |
| OpenYR | We added the key. |
| OpenTS | OpenTS added the key before or alongside our fork. |
| Ares, Phobos | We read the key as the Ares or Phobos documentation describes it. |

## Contents

- [Rules](#rules): 332 entries
- [Art](#art): 7 entries
- [Client settings](#client-settings): 38 entries
- [UI.INI](#uiini): 31 entries
- [Other game files](#other-game-files): 10 entries
- [SPAWN.INI](#spawnini): 64 entries
- [OPENTS.INI and the launcher](#opentsini-and-the-launcher): 34 entries

## Rules

Example:

```ini
[AudioVisual]
VoxelSpecularStrength=3      ; 0 or less turns the highlight off
VoxelShadeScale=16
VoxelSpecularFromView=no

[General]
LargeSidebarStrips=yes       ; keep the 225-cameo sidebar strip

[HTNK]                       ; a unit type
WreckReleasesPassengers=yes
Bounty=yes
Bounty.Value=200
```

### BuildingType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`BarrelAnimZAdjust`](../manual/content/keys/barrelanimzadjust--buildingtype.md) | `-21` | Sets the depth bias, in pixels, of a building's voxel barrel. | OpenYR |
| [`Chronoshift.IsVehicle`](../manual/content/keys/chronoshift.isvehicle--buildingtype.md) | `no` | Marks a structure as a deployed vehicle for the chronosphere. | Ares |
| [`EligibileForAllyBuilding`](../manual/content/keys/eligibileforallybuilding.md) | `no` | Whether an ally's placements may anchor on a building of this type. | OpenTS |
| [`LightningRod.Modifier`](../manual/content/keys/lightningrod.modifier--buildingtype.md) | `1.0` | Scales the damage a lightning storm bolt does to a structure with LightningRod=yes. | Ares |
| [`ReverseEngineersVictims`](../manual/content/keys/reverseengineersvictims.md) | `no` | Makes this grinding structure teach its owner to build the types it grinds up. | Ares |
| [`SolidHeight`](../manual/content/keys/solidheight.md) | 0, so the building is not solid | How many cells of a building's height stop a SubjectToBuildings projectile. | Ares |
| [`SortCameoAsBaseDefense`](../manual/content/keys/sortcameoasbasedefense.md) | Follows `IsBaseDefense=` | Sorts a BuildingType's cameo into the base-defense group of the structures strip. | OpenTS |
| [`SuperWeapons`](../manual/content/keys/superweapons--buildingtype.md) | `none` | The superweapons a structure grants, or the house intelligence level at which a computer house fires them. | Ares |
| [`UC.DamageMultiplier`](../manual/content/keys/uc.damagemultiplier.md) | `100%` | Multiplies the damage of a passed-through shot that is not fatal, before the occupant's own damage rules. | Ares |
| [`UC.FatalRate`](../manual/content/keys/uc.fatalrate.md) | `0%` | The share of passed-through shots that kill one occupant of the structure outright. | Ares |
| [`UC.PassThrough`](../manual/content/keys/uc.passthrough.md) | `0%` | The share of shots at an occupied structure that hit one of its occupants instead of the structure. | Ares |

### BulletType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AirburstSpread`](../manual/content/keys/airburstspread.md) | not set, so an Airburst projectile splits as `Splits` describes | Makes an Airburst projectile fire its AirburstWeapon at every cell within this many cells of the burst. | Ares |
| [`AroundTarget`](../manual/content/keys/aroundtarget.md) | Depends; see the page | Whether the bomblets of a splitting or spreading projectile are placed around its target or around the point it burst at. | Ares |
| [`BallisticScatter.Max`](../manual/content/keys/ballisticscatter.max.md) | the `[CombatDamage]` `BallisticScatter` value | The most a projectile type moves the aim point of an inaccurate arcing shot, in cells. | Ares |
| [`BallisticScatter.Min`](../manual/content/keys/ballisticscatter.min.md) | Depends; see the page | The least a projectile type moves the aim point of an inaccurate arcing shot, in cells. | Ares |
| [`SubjectToBuildings`](../manual/content/keys/subjecttobuildings.md) | `no` | Makes a projectile explode on a building that is solid to it, so that it cannot fly through. | Ares |
| [`SubjectToTrenches`](../manual/content/keys/subjecttotrenches.md) | `yes` | With no, every shot of this projectile at a structure with UC.PassThrough passes through to its occupants. | Ares |

### HouseType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`File.Flag`](../manual/content/keys/file.flag.md) | Computed; see the page | The PCX file shown as the country's flag beside a player's name on the loading screen. | Ares |
| [`File.LoadScreen`](../manual/content/keys/file.loadscreen.md) | Computed; see the page | The shape drawn as the loading screen of a skirmish or multiplayer match for a player of the country. | Ares |
| [`File.LoadScreenPAL`](../manual/content/keys/file.loadscreenpal.md) | Computed; see the page | The palette the country's loading screen is drawn with. | Ares |
| [`GivesBounty`](../manual/content/keys/givesbounty.md) | `yes` | Whether destroying this country's objects pays bounty to the destroyer's owner. | Ares |
| [`ListIndex`](../manual/content/keys/listindex.md) | `100` | Where the country stands in the lists of countries that the lobbies offer. | Ares |
| [`LoadScreenText.Brief`](../manual/content/keys/loadscreentext.brief.md) | Computed; see the page | The string table label of the description printed on the country's loading screen. | Ares |
| [`LoadScreenText.Color`](../manual/content/keys/loadscreentext.color.md) | Computed; see the page | The color scheme of the text on the country's loading screen. | Ares |
| [`LoadScreenText.Name`](../manual/content/keys/loadscreentext.name.md) | Computed; see the page | The string table label of the country name printed on its loading screen. | Ares |
| [`LoadScreenText.SpecialName`](../manual/content/keys/loadscreentext.specialname.md) | Computed; see the page | The string table label of the special unit's name printed on the country's loading screen. | Ares |

### Object types

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AttachEffect.Animation`](../manual/content/keys/attacheffect.animation--aircrafttype.md) | `none` | The animation shown on an object while an attached effect lasts. | Ares |
| [`AttachEffect.ArmorMultiplier`](../manual/content/keys/attacheffect.armormultiplier--aircrafttype.md) | `1.0` | Divides the damage an object takes while an attached effect lasts. | Ares |
| [`AttachEffect.Cloakable`](../manual/content/keys/attacheffect.cloakable--aircrafttype.md) | `no` | Whether an object can cloak while an attached effect lasts. | Ares |
| [`AttachEffect.Delay`](../manual/content/keys/attacheffect.delay--aircrafttype.md) | `0` | `AttachEffect.Delay` is how many frames after an object's own effect ends, or is removed on entry, it is attached again. | Ares |
| [`AttachEffect.DiscardOnEntry`](../manual/content/keys/attacheffect.discardonentry--aircrafttype.md) | `no` | Whether an attached effect is removed when the object leaves the map. | Ares |
| [`AttachEffect.Duration`](../manual/content/keys/attacheffect.duration--aircrafttype.md) | `0` | How many frames an attached effect lasts; 0 attaches nothing. | Ares |
| [`AttachEffect.FirepowerMultiplier`](../manual/content/keys/attacheffect.firepowermultiplier--aircrafttype.md) | `1.0` | Multiplies the damage of the shots an object fires while an attached effect lasts. | Ares |
| [`AttachEffect.ForceDecloak`](../manual/content/keys/attacheffect.forcedecloak--aircrafttype.md) | `no` | Whether attaching an effect uncloaks the object. | Ares |
| [`AttachEffect.InitialDelay`](../manual/content/keys/attacheffect.initialdelay--aircrafttype.md) | `0` | `AttachEffect.InitialDelay` is how many frames after an object of this type enters the game its own effect is attached for the first time. | Ares |
| [`AttachEffect.PenetratesIronCurtain`](../manual/content/keys/attacheffect.penetratesironcurtain--aircrafttype.md) | `no` | Whether an effect can be attached to an object under the Iron Curtain or a Force Shield. | Ares |
| [`AttachEffect.ROFMultiplier`](../manual/content/keys/attacheffect.rofmultiplier--aircrafttype.md) | `1.0` | Multiplies the reload time of an object while an attached effect lasts. | Ares |
| [`AttachEffect.SpeedMultiplier`](../manual/content/keys/attacheffect.speedmultiplier--aircrafttype.md) | `1.0` | Multiplies the movement speed of an object while an attached effect lasts. | Ares |
| [`AttachEffect.TemporalHidesAnim`](../manual/content/keys/attacheffect.temporalhidesanim--aircrafttype.md) | `no` | Whether an attached effect's animation is hidden while a temporal weapon warps the object out. | Ares |
| [`Bounty`](../manual/content/keys/bounty.md) | `no` | Makes the house that owns this object collect a bounty whenever the object destroys an enemy object. | Ares |
| [`Bounty.Display`](../manual/content/keys/bounty.display.md) | [AudioVisual] BountyDisplay, which is no when it is also absent | Whether the bounty this object earns its owner is shown over the destroyed object. | Ares |
| [`Bounty.EliteValue`](../manual/content/keys/bounty.elitevalue.md) | Bounty.Value from the same section, or 0 when that is also absent | The bounty this object pays when it is destroyed as an elite, overriding Bounty.Value. | Ares |
| [`Bounty.RookieValue`](../manual/content/keys/bounty.rookievalue.md) | Bounty.Value from the same section, or 0 when that is also absent | The bounty this object pays when it is destroyed as a rookie, overriding Bounty.Value. | Ares |
| [`Bounty.Value`](../manual/content/keys/bounty.value.md) | `0` | The bounty this object pays when it is destroyed, set for the rookie, veteran and elite ranks at once. | Ares |
| [`Bounty.VeteranValue`](../manual/content/keys/bounty.veteranvalue.md) | Bounty.Value from the same section, or 0 when that is also absent | The bounty this object pays when it is destroyed as a veteran, overriding Bounty.Value. | Ares |
| [`BuildTime.MultipleFactory`](../manual/content/keys/buildtime.multiplefactory.md) | [General] MultipleFactory | The build-time multiplier each factory past the first applies to production of this type, in place of MultipleFactory. | Ares, Phobos |
| [`CameoSortOrder`](../manual/content/keys/cameosortorder--aircrafttype.md) | `0` | Orders a type's sidebar cameo among the cameos of the same kind. | OpenTS |
| [`CanBeReversed`](../manual/content/keys/canbereversed.md) | `yes` | Whether grinding up an object of this type in a reverse engineering structure teaches its owner anything. | Ares |
| [`Chronoshift.Allow`](../manual/content/keys/chronoshift.allow.md) | `yes` | Lets the chronosphere skip a vehicle, infantry type or landed aircraft, leaving it where it is. | Ares |
| [`Chronoshift.Crushable`](../manual/content/keys/chronoshift.crushable.md) | `yes` | Stops a unit the chronosphere sets down on this object from destroying it; the arriving unit is destroyed instead. | Ares |
| [`Crashable`](../manual/content/keys/crashable--aircrafttype.md) | `yes` | Whether a destroyed object of this type falls, or what happens to its passengers when it is destroyed. | Ares, Phobos |
| [`EMP.Modifier`](../manual/content/keys/emp.modifier.md) | 1.0, so the stun lasts the pulse's full duration | Scales how long an EM pulse stuns this object. | Ares |
| [`EMP.Threshold`](../manual/content/keys/emp.threshold.md) | no; a stun never destroys the object | The longest EM pulse stun this object survives; a longer stun destroys it. | Ares |
| [`ForceShield.Modifier`](../manual/content/keys/forceshield.modifier.md) | `1` | Multiplies the length of a Force Shield put on an object of this type; 0 or less makes it immune. | Ares |
| [`GroupAs`](../manual/content/keys/groupas.md) | none; the type is grouped under its own ID | Names the group the Select Same Type command puts this type in. | Ares |
| [`HealthBar.Hide`](../manual/content/keys/healthbar.hide.md) | `no` | Stops the health bar from being drawn over objects of this type. | Ares, Phobos |
| [`ImmuneToAbduction`](../manual/content/keys/immunetoabduction.md) | `no` | With yes, an Abductor weapon cannot take this object aboard and deals its ordinary damage instead. | Ares |
| [`ImmuneToEMP`](../manual/content/keys/immunetoemp.md) | A BuildingType or UnitType uses its `IsCoreDefender=` value. Every other type uses `no` | Stops an EM pulse from stunning, powering off, downing or destroying objects of this type. | OpenTS |
| [`Insignia`](../manual/content/keys/insignia.md) | `PIPS.SHP` | The shape file that supplies the rank insignia frames of an object type, for all three ranks at once. | Ares, Phobos |
| [`Insignia.ShowEnemy`](../manual/content/keys/insignia.showenemy.md) | [General] EnemyInsignia, which is yes when it is also absent | Whether a player not allied with the owner sees this object type's rank insignia. | Ares, Phobos |
| [`InsigniaFrame`](../manual/content/keys/insigniaframe.md) | `-1` | The insignia frame an object type shows for all three ranks, or for one rank through the rank keys. | Ares, Phobos |
| [`InsigniaFrames`](../manual/content/keys/insigniaframes.md) | `none` | The insignia frames of an object type's rookie, veteran and elite ranks, as a comma-separated list. | Ares, Phobos |
| [`IronCurtain.Modifier`](../manual/content/keys/ironcurtain.modifier.md) | `1` | Multiplies the length of an Iron Curtain put on an object of this type; 0 makes it immune. | Ares |
| [`KeepAlive`](../manual/content/keys/keepalive.md) | Depends; see the page | Whether owning an object of this type keeps a player in a short game. | Ares |
| [`MaxPips`](../manual/content/keys/maxpips.md) | Depends; see the page | How many pips a selected object's pip row holds. | OpenYR |
| [`Mechanic`](../manual/content/keys/mechanic.md) | `no` | Turns a healing weapon away from infantry and onto vehicles. | OpenYR |
| [`OmniHealer`](../manual/content/keys/omnihealer.md) | `no` | Lets a healing weapon mend infantry and vehicles alike. | OpenYR |
| [`OpenTopped.DamageMultiplier`](../manual/content/keys/opentopped.damagemultiplier--aircrafttype.md) | The [CombatDamage] OpenToppedDamageMultiplier= value | `OpenTopped.DamageMultiplier` multiplies the damage of each shot a passenger fires from this `OpenTopped=yes` transport. | Phobos |
| [`OpenTopped.RangeBonus`](../manual/content/keys/opentopped.rangebonus--aircrafttype.md) | The [CombatDamage] OpenToppedRangeBonus= value | `OpenTopped.RangeBonus` sets how many whole cells beyond its weapons' `Range` a passenger can fire from this `OpenTopped=yes` transport. | Phobos |
| [`OpenTopped.WarpDistance`](../manual/content/keys/opentopped.warpdistance--aircrafttype.md) | The [CombatDamage] OpenToppedWarpDistance= value | `OpenTopped.WarpDistance` sets how far, in cells, a temporal firer riding this `OpenTopped=yes` transport can hold its target before it lets go. | Phobos |
| [`OpenTransport.DamageMultiplier`](../manual/content/keys/opentransport.damagemultiplier--aircrafttype.md) | The [CombatDamage] OpenTransport.DamageMultiplier= value | `OpenTransport.DamageMultiplier` multiplies the damage of each shot this object fires from any `OpenTopped=yes` transport. | Phobos |
| [`OpenTransport.RangeBonus`](../manual/content/keys/opentransport.rangebonus--aircrafttype.md) | The [CombatDamage] OpenTransport.RangeBonus= value | `OpenTransport.RangeBonus` adds whole cells to this object's weapon range while it fires from any `OpenTopped=yes` transport. | Phobos |
| [`Promote.EliteSound`](../manual/content/keys/promote.elitesound.md) | [AudioVisual] UpgradeEliteSound, which is none when it is also absent | The sound played at one of the player's objects of this type as it becomes elite. | Ares, Phobos |
| [`Promote.VeteranSound`](../manual/content/keys/promote.veteransound.md) | [AudioVisual] UpgradeVeteranSound, which is none when it is also absent | The sound played at one of the player's objects of this type as it becomes a veteran. | Ares, Phobos |
| [`ProtectedDriver`](../manual/content/keys/protecteddriver.md) | `no` | With yes, KillDriver warheads leave this vehicle's driver alone. | Ares |
| [`ProtectedDriver.MinHealth`](../manual/content/keys/protecteddriver.minhealth.md) | Depends; see the page | The share of its health at or below which a KillDriver warhead can take this vehicle's driver. | Ares |
| [`ReversedAs`](../manual/content/keys/reversedas.md) | none; grinding teaches the object's own type | Names the type that grinding up an object of this type teaches in place of its own. | Ares |
| [`ScrapExplosion`](../manual/content/keys/scrapexplosion.md) | (none) | The animations a destroyed object leaves behind while scrap wreckage is switched on. | OpenTS |
| [`SelfHealingCap`](../manual/content/keys/selfhealingcap.md) | `-1` | The share of maximum strength at which this type stops healing itself. | OpenTS |
| [`SelfHealingRate`](../manual/content/keys/selfhealingrate.md) | `-1` | The interval between this type's self-healing ticks. | OpenTS |
| [`SelfHealingStep`](../manual/content/keys/selfhealingstep.md) | `-1` | The strength one self-healing tick restores to this type. | OpenTS |
| [`VehicleThief.Allowed`](../manual/content/keys/vehiclethief.allowed.md) | `yes` | Whether a vehicle thief can take vehicles of this type. | Ares, Phobos |

### OverlayType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`BuildableOver`](../manual/content/keys/buildableover.md) | `no` | Lets structures be placed on cells that hold this overlay. | OpenTS |

### Side

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AdvancedPowerPlant`](../manual/content/keys/advancedpowerplant--side.md) | Computed; see the page | The BuildingType a computer house playing for this side inserts to cover a power shortfall, once the structures the house owns meet this type's `Prerequisite` list. | OpenTS |
| [`AIBaseDefenseCoefficient`](../manual/content/keys/aibasedefensecoefficient--side.md) | Computed; see the page | Scales how many base defenses a computer house playing for this side mixes into its base plan as the plan grows more expensive. | OpenTS |
| [`AIBaseDefensePlaceholders`](../manual/content/keys/aibasedefenseplaceholders--side.md) | 3 for the first side and 2 for every other | Sets how many extra base defenses a computer house playing for this side adds at the end of its generated base plan. | OpenTS |
| [`AIBaseDefensesWithWalls`](../manual/content/keys/aibasedefenseswithwalls--side.md) | yes for the second side and no for every other | Whether a computer house playing for this side adds its `AIBaseDefensePlaceholders` block of extra base defenses even when it also plans a perimeter wall. | OpenTS |
| [`AIBuildsWalls`](../manual/content/keys/aibuildswalls--side.md) | Computed; see the page | Whether computer houses plan a perimeter wall around their bases. | OpenTS |
| [`AIWallDefense`](../manual/content/keys/aiwalldefense--side.md) | The first side takes GDIWallDefense as each rules file sets it; any other side uses 0 | Along its perimeter wall, a computer house playing for this side adds pairs of a wall tower and a base defense. | OpenTS |
| [`AIWallDefenseCoefficient`](../manual/content/keys/aiwalldefensecoefficient--side.md) | Computed; see the page | The part of the limit on wall tower and defense pairs that grows with the game's difficulty. | OpenTS |
| [`AIWallTowers`](../manual/content/keys/aiwalltowers--side.md) | The first side takes WallTower as each rules file sets it; every other side lists none | The wall towers a computer house playing for this side builds, each meant to carry a base defense that plugs into it. | OpenTS |
| [`CampaignScore.Animation`](../manual/content/keys/campaignscore.animation.md) | Computed; see the page | The shape looped on the campaign score screen. | Ares |
| [`CampaignScore.Background`](../manual/content/keys/campaignscore.background.md) | Computed; see the page | The shape drawn as the background of the campaign score screen for a side. | Ares |
| [`CampaignScore.Palette`](../manual/content/keys/campaignscore.palette.md) | Computed; see the page | The palette of the campaign score screen for a side. | Ares |
| [`Crew`](../manual/content/keys/crew.md) | Computed; see the page | The InfantryType that leaves a destroyed or sold crewed object owned by a house of the side. | Ares |
| [`DefaultDisguise`](../manual/content/keys/defaultdisguise.md) | Computed; see the page | The InfantryType that an object with PermaDisguise shows for a house of the side when it has no other disguise. | Ares |
| [`Engineer`](../manual/content/keys/engineer--side.md) | The `[General]` `Engineer` type | Whether a soldier restores and captures structures, or which InfantryType a destroyed or sold structure can hand back as an engineer. | Ares |
| [`EVA.Tag`](../manual/content/keys/eva.tag.md) | Computed; see the page | The announcer voice a side hears, named by one of the voices EVAMD.INI offers. | Ares |
| [`HunterSeeker`](../manual/content/keys/hunterseeker--side.md) | Computed; see the page | Marks a type as a hunter-seeker drone, or names the drone a side's hunter-seeker superweapon launches. | OpenTS |
| [`MessageTextColor`](../manual/content/keys/messagetextcolor.md) | `none` | The color scheme of the text a map's text action prints for a player of the side. | Ares |
| [`MultiplayerScore.Background`](../manual/content/keys/multiplayerscore.background.md) | Computed; see the page | The shape drawn as the side's picture on the multiplayer score screen. | Ares |
| [`MultiplayerScore.Bars`](../manual/content/keys/multiplayerscore.bars.md) | Computed; see the page | The PCX files drawn behind the title, the headings and each player row of the multiplayer score screen. | Ares |
| [`MultiplayerScore.LoseTheme`](../manual/content/keys/multiplayerscore.losetheme.md) | `SCORE` | The theme played behind the multiplayer score screen when the local player lost. | Ares |
| [`MultiplayerScore.Palette`](../manual/content/keys/multiplayerscore.palette.md) | Computed; see the page | The palette the multiplayer score screen's picture is drawn with. | Ares |
| [`MultiplayerScore.WinTheme`](../manual/content/keys/multiplayerscore.wintheme.md) | `SCORE` | The theme played behind the multiplayer score screen when the local player won. | Ares |
| [`PowerTurbine`](../manual/content/keys/powerturbine--side.md) | Computed; see the page | The power plant upgrade a computer house playing for this side prefers when it inserts a plant to cover a power shortfall. | OpenTS |
| [`RegularPowerPlant`](../manual/content/keys/regularpowerplant--side.md) | Computed; see the page | The BuildingType a computer house playing for this side inserts to cover a power shortfall when neither its `PowerTurbine` nor its `AdvancedPowerPlant` qualifies. | OpenTS |
| [`Sidebar.MixFileIndex`](../manual/content/keys/sidebar.mixfileindex.md) | Computed; see the page | The number that names the sidebar archives a side plays with. | Ares |
| [`Sidebar.YuriFileNames`](../manual/content/keys/sidebar.yurifilenames.md) | Yes for the third side in `[Sides]` and no for every other side | Whether the side's radar logo is drawn from RADARY.SHP in the colors of RADARYURI.PAL. | Ares |
| [`SurvivorDivisor`](../manual/content/keys/survivordivisor.md) | Computed; see the page | The credits of a crewed structure's refund that make one survivor, for a house of the side. | Ares |
| [`Technician`](../manual/content/keys/technician--side.md) | The `[General]` `Technician` type | The InfantryType that a crewed object leaves as a technician survivor. | Ares |
| [`ToolTipColor`](../manual/content/keys/tooltipcolor.md) | `none` | The color of the border and text of the classic sidebar's tooltips for a side. | Ares |

### SuperWeaponType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`Battery.KeepOnline`](../manual/content/keys/battery.keeponline--superweapontype.md) | `none` | Lists the structures a running battery keeps powered on. | Ares |
| [`Battery.Overpower`](../manual/content/keys/battery.overpower--superweapontype.md) | `none` | Lists the structures a running battery overpowers. | Ares |
| [`Battery.Power`](../manual/content/keys/battery.power--superweapontype.md) | `0` | Sets the power a running battery adds. | Ares |
| [`CameoSortOrder`](../manual/content/keys/cameosortorder--superweapontype.md) | `0` | Orders a type's sidebar cameo among the cameos of the same kind. | OpenTS |
| [`Chronosphere.AffectsIronCurtain`](../manual/content/keys/chronosphere.affectsironcurtain--superweapontype.md) | `no` | Chooses whether a chronoshift moves units under the Iron Curtain. | Ares |
| [`Chronosphere.AffectsUndeployable`](../manual/content/keys/chronosphere.affectsundeployable--superweapontype.md) | `no` | Makes the chronosphere move every structure that can undeploy, as if it were a vehicle. | Ares |
| [`Chronosphere.AffectsUnwarpable`](../manual/content/keys/chronosphere.affectsunwarpable--superweapontype.md) | `yes` | Chooses whether a chronoshift moves units that cannot be warped. | Ares |
| [`Chronosphere.BlastDest`](../manual/content/keys/chronosphere.blastdest--superweapontype.md) | [General] ChronoBlastDest | Names the animation that plays over the target cell when units are chronoshifted. | Ares |
| [`Chronosphere.BlastSrc`](../manual/content/keys/chronosphere.blastsrc--superweapontype.md) | [General] ChronoBlast | Names the animation that plays over the picked cell when units are chronoshifted. | Ares |
| [`Chronosphere.BlowUnplaceable`](../manual/content/keys/chronosphere.blowunplaceable--superweapontype.md) | `yes` | Chooses whether a structure that cannot land at the target is destroyed or stays where it was. | Ares |
| [`Chronosphere.KillCargo`](../manual/content/keys/chronosphere.killcargo--superweapontype.md) | `no` | Chooses whether a chronoshift destroys the passengers of the units it moves. | Ares |
| [`Chronosphere.KillOrganic`](../manual/content/keys/chronosphere.killorganic--superweapontype.md) | `yes` | Chooses whether a chronoshift destroys organic units. | Ares |
| [`Chronosphere.KillTeleporters`](../manual/content/keys/chronosphere.killteleporters--superweapontype.md) | `no` | Chooses whether a chronoshift destroys units that can teleport. | Ares |
| [`Chronosphere.ReconsiderBuildings`](../manual/content/keys/chronosphere.reconsiderbuildings--superweapontype.md) | `yes` | Chooses whether a structure flagged as a deployed vehicle counts as a vehicle for the chronosphere. | Ares |
| [`Cursor`](../manual/content/keys/cursor--superweapontype.md) | the cursor for the Action | Names the cursor shown over cells where the weapon may be aimed. | Ares |
| [`Deliver.BaseNormal`](../manual/content/keys/deliver.basenormal--superweapontype.md) | `yes` | Reads whether delivered structures count as part of the base; it has no effect yet. | Ares |
| [`Deliver.Buildups`](../manual/content/keys/deliver.buildups--superweapontype.md) | `no` | Whether delivered structures play their construction animation. | Ares |
| [`Deliver.Owner`](../manual/content/keys/deliver.owner--superweapontype.md) | Invoker | Chooses who owns the objects a unit delivery brings in. | Ares |
| [`Deliver.Types`](../manual/content/keys/deliver.types--superweapontype.md) | `none` | Lists the objects a unit delivery brings in. | Ares |
| [`Dominator.Capture`](../manual/content/keys/dominator.capture--superweapontype.md) | `yes` | Chooses whether a psychic dominator takes units over. | Ares |
| [`Dominator.CaptureImmuneToPsionics`](../manual/content/keys/dominator.captureimmunetopsionics--superweapontype.md) | `no` | Chooses whether a psychic dominator takes over units that are immune to psionics. | Ares |
| [`Dominator.CaptureMindControlled`](../manual/content/keys/dominator.capturemindcontrolled--superweapontype.md) | `yes` | Chooses whether a psychic dominator takes over units that are mind controlled. | Ares |
| [`Dominator.CapturePermaMindControlled`](../manual/content/keys/dominator.capturepermamindcontrolled--superweapontype.md) | `yes` | Chooses whether a psychic dominator takes over units that are permanently mind controlled. | Ares |
| [`Dominator.ControlAnim`](../manual/content/keys/dominator.controlanim--superweapontype.md) | [CombatDamage] PermaControlledAnimationType | Names the animation shown over a unit a psychic dominator takes over. | Ares |
| [`Dominator.FireAtPercentage`](../manual/content/keys/dominator.fireatpercentage--superweapontype.md) | [General] DominatorFireAtPercentage | Sets how far through its first animation a psychic dominator fires. | Ares |
| [`Dominator.FirstAnim`](../manual/content/keys/dominator.firstanim--superweapontype.md) | [General] DominatorFirstAnim | Names the animation that plays over a psychic dominator's target before it fires. | Ares |
| [`Dominator.FirstAnimHeight`](../manual/content/keys/dominator.firstanimheight--superweapontype.md) | `0` | Sets how high above the ground the first dominator animation plays. | Ares |
| [`Dominator.PermanentCapture`](../manual/content/keys/dominator.permanentcapture--superweapontype.md) | `yes` | Chooses whether a unit a psychic dominator takes over is held for good. | Ares |
| [`Dominator.Ripple`](../manual/content/keys/dominator.ripple--superweapontype.md) | `yes` | Chooses whether a psychic dominator sends out its shockwave. | Ares |
| [`Dominator.SecondAnim`](../manual/content/keys/dominator.secondanim--superweapontype.md) | [General] DominatorSecondAnim | Names the animation that plays when a psychic dominator fires. | Ares |
| [`Dominator.SecondAnimHeight`](../manual/content/keys/dominator.secondanimheight--superweapontype.md) | `0` | Sets how high above the ground the second dominator animation plays. | Ares |
| [`DropPod.Maximum`](../manual/content/keys/droppod.maximum--superweapontype.md) | [General] DropPodInfantryMaximum | Sets the most pods a drop pod weapon lands. | Ares |
| [`DropPod.Minimum`](../manual/content/keys/droppod.minimum--superweapontype.md) | [General] DropPodInfantryMinimum | Sets the fewest pods a drop pod weapon lands. | Ares |
| [`DropPod.Types`](../manual/content/keys/droppod.types--superweapontype.md) | E1 and E2, equally likely | Lists the infantry a drop pod weapon lands. | Ares |
| [`DropPod.Veterancy`](../manual/content/keys/droppod.veterancy--superweapontype.md) | `2.0` | Sets the rank the pod infantry land with. | Ares |
| [`EMPulse.Cannons`](../manual/content/keys/empulse.cannons--superweapontype.md) | Every EMPulseCannon=yes structure | Lists the structures that fire this EM pulse. | Ares |
| [`EMPulse.Linked`](../manual/content/keys/empulse.linked--superweapontype.md) | `no` | Reads a flag for EM pulse cannons; it has no effect yet. | Ares |
| [`EMPulse.PulseBall`](../manual/content/keys/empulse.pulseball--superweapontype.md) | `PULSBALL` | Reads the animation a cannon plays before it fires; it has no effect yet. | Ares |
| [`EMPulse.PulseDelay`](../manual/content/keys/empulse.pulsedelay--superweapontype.md) | `32` | Reads how long a cannon waits before it fires; it has no effect yet. | Ares |
| [`EMPulse.TargetSelf`](../manual/content/keys/empulse.targetself--superweapontype.md) | `no` | Makes each firing cannon detonate its weapon on itself. | Ares |
| [`EVA.Activated`](../manual/content/keys/eva.activated--superweapontype.md) | The behavior's own line | Names the announcer line played when the weapon fires. | Ares |
| [`EVA.Detected`](../manual/content/keys/eva.detected--superweapontype.md) | The behavior's own line | Names the announcer line played when an enemy builds the structure that grants the weapon. | Ares |
| [`EVA.Impatient`](../manual/content/keys/eva.impatient--superweapontype.md) | `ImpatientVoice` | Names the announcer line played when the cameo is clicked before the weapon is ready. | Ares |
| [`EVA.InsufficientFunds`](../manual/content/keys/eva.insufficientfunds--superweapontype.md) | `EVA_InsufficientFunds` | Names the announcer line played when a shot is refused for lack of money. | Ares |
| [`EVA.Ready`](../manual/content/keys/eva.ready--superweapontype.md) | The behavior's own line | Names the announcer line played when the weapon is charged. | Ares |
| [`EVA.SelectTarget`](../manual/content/keys/eva.selecttarget--superweapontype.md) | The select-target line | Names the announcer line played when the cameo starts aiming. | Ares |
| [`HunterSeeker.Buildings`](../manual/content/keys/hunterseeker.buildings--superweapontype.md) | [SpecialWeapons] HSBuilding | Lists the structures that launch a hunter seeker. | Ares |
| [`HunterSeeker.RandomOnly`](../manual/content/keys/hunterseeker.randomonly--superweapontype.md) | `no` | Reads a targeting flag for hunter seekers; it has no effect yet. | Ares |
| [`HunterSeeker.Type`](../manual/content/keys/hunterseeker.type--superweapontype.md) | The side's HunterSeeker | Names the vehicle a hunter seeker launch sends out. | Ares |
| [`Light.Ambient`](../manual/content/keys/light.ambient--superweapontype.md) | The scenario's IonAmbient (storm), DominatorAmbient (dominator); 200 (missile) | Sets the ambient light of the lighting a nuclear missile, lightning storm or psychic dominator gives the map. | Ares |
| [`Light.Blue`](../manual/content/keys/light.blue--superweapontype.md) | The scenario's IonBlue or DominatorBlue (storm, dominator); 125 (missile) | Sets the blue tint of the lighting a nuclear missile, lightning storm or psychic dominator gives the map. | Ares |
| [`Light.Enabled`](../manual/content/keys/light.enabled--superweapontype.md) | `yes` | Chooses whether a nuclear missile, lightning storm or psychic dominator lights the map. | Ares |
| [`Light.Green`](../manual/content/keys/light.green--superweapontype.md) | The scenario's IonGreen or DominatorGreen (storm, dominator); 150 (missile) | Sets the green tint of the lighting a nuclear missile, lightning storm or psychic dominator gives the map. | Ares |
| [`Light.Red`](../manual/content/keys/light.red--superweapontype.md) | The scenario's IonRed or DominatorRed (storm, dominator); 175 (missile) | Sets the red tint of the lighting a nuclear missile, lightning storm or psychic dominator gives the map. | Ares |
| [`Lightning.BoltExplosion`](../manual/content/keys/lightning.boltexplosion--superweapontype.md) | The explosion that the bolt's damage and warhead pick | Names the animation each lightning bolt leaves where it strikes. | Ares |
| [`Lightning.Bolts`](../manual/content/keys/lightning.bolts--superweapontype.md) | [General] WeatherConBolts | Lists the bolt animations a lightning storm can use. | Ares |
| [`Lightning.CloudHeight`](../manual/content/keys/lightning.cloudheight--superweapontype.md) | `-1` | Sets the height of a lightning storm's clouds above the ground. | Ares |
| [`Lightning.Clouds`](../manual/content/keys/lightning.clouds--superweapontype.md) | [General] WeatherConClouds | Lists the cloud animations a lightning storm can use. | Ares |
| [`Lightning.Debris`](../manual/content/keys/lightning.debris--superweapontype.md) | [AudioVisual] MetallicDebris | Lists the debris animations a lightning bolt can throw up. | Ares |
| [`Lightning.DebrisMax`](../manual/content/keys/lightning.debrismax--superweapontype.md) | `4` | Sets the most debris a lightning bolt throws up. | Ares |
| [`Lightning.DebrisMin`](../manual/content/keys/lightning.debrismin--superweapontype.md) | `2` | Sets the least debris a lightning bolt throws up. | Ares |
| [`Lightning.Duration`](../manual/content/keys/lightning.duration--superweapontype.md) | [General] LightningStormDuration | Sets how many frames a lightning storm rages. | Ares |
| [`Lightning.HitDelay`](../manual/content/keys/lightning.hitdelay--superweapontype.md) | [General] LightningHitDelay | Sets how often a cloud gathers over the center of a lightning storm. | Ares |
| [`Lightning.IgnoreLightningRod`](../manual/content/keys/lightning.ignorelightningrod--superweapontype.md) | `no` | Stops a lightning storm from favoring structures with LightningRod=yes. | Ares |
| [`Lightning.PrintText`](../manual/content/keys/lightning.printtext--superweapontype.md) | [General] LightningPrintText | Chooses whether a lightning storm warns the players. | Ares |
| [`Lightning.RadarOutage`](../manual/content/keys/lightning.radaroutage--superweapontype.md) | [General] LightningStormDuration | Sets how many frames a lightning storm jams radar. | Ares |
| [`Lightning.RadarOutageAffects`](../manual/content/keys/lightning.radaroutageaffects--superweapontype.md) | `enemies` | Chooses which houses a lightning storm jams radar for. | Ares |
| [`Lightning.ScatterCount`](../manual/content/keys/lightning.scattercount--superweapontype.md) | `1` | Sets how many clouds gather around a lightning storm each time. | Ares |
| [`Lightning.ScatterDelay`](../manual/content/keys/lightning.scatterdelay--superweapontype.md) | [General] LightningScatterDelay | Sets how often clouds gather around the center of a lightning storm. | Ares |
| [`Lightning.Separation`](../manual/content/keys/lightning.separation--superweapontype.md) | [General] LightningSeparation | Sets how far apart the random clouds of a lightning storm must be. | Ares |
| [`Lightning.Sounds`](../manual/content/keys/lightning.sounds--superweapontype.md) | [AudioVisual] LightningSounds | Lists the sounds a lightning bolt can play. | Ares |
| [`Message.Abort`](../manual/content/keys/message.abort--superweapontype.md) | The stock refusal message | Replaces the refusal message of a lightning storm or psychic dominator. | Ares |
| [`Message.Activate`](../manual/content/keys/message.activate--superweapontype.md) | `none` | Prints a message to every player when the effect starts. | Ares |
| [`Message.CannotFire`](../manual/content/keys/message.cannotfire--superweapontype.md) | `MSG:CannotFire` | Names the message shown when cameo-click targeting finds no target. | Ares |
| [`Message.Detected`](../manual/content/keys/message.detected--superweapontype.md) | `none` | Prints a message when an enemy builds the structure that grants the weapon. | Ares |
| [`Message.FirerColor`](../manual/content/keys/message.firercolor--superweapontype.md) | `no` | Prints the weapon's messages in the firing house's color. | Ares |
| [`Message.InsufficientFunds`](../manual/content/keys/message.insufficientfunds--superweapontype.md) | `none` | Prints a message when a shot is refused for lack of money. | Ares |
| [`Message.Launch`](../manual/content/keys/message.launch--superweapontype.md) | `none` | Prints a message to every player when the weapon fires. | Ares |
| [`Message.Ready`](../manual/content/keys/message.ready--superweapontype.md) | `none` | Prints a message to the owner when the weapon is charged. | Ares |
| [`Money.Amount`](../manual/content/keys/money.amount--superweapontype.md) | `0` | Charges or pays the house each time the weapon fires. | Ares |
| [`Mutate.Explosion`](../manual/content/keys/mutate.explosion--superweapontype.md) | The rules' MutateExplosion | Chooses for one genetic mutator whether it strikes as a blast or mutates the infantry in an area. | Ares |
| [`Mutate.IgnoreCyborg`](../manual/content/keys/mutate.ignorecyborg--superweapontype.md) | `no` | Spares infantry with Cyborg=yes from a genetic mutator that works on an area. | Ares |
| [`Mutate.IgnoreNotHuman`](../manual/content/keys/mutate.ignorenothuman--superweapontype.md) | `no` | Spares infantry with NotHuman=yes from a genetic mutator that works on an area. | Ares |
| [`Mutate.KillNatural`](../manual/content/keys/mutate.killnatural--superweapontype.md) | `yes` | Kills infantry with Natural=yes outright when a genetic mutator deals damage through its warhead. | Ares |
| [`NoCursor`](../manual/content/keys/nocursor--superweapontype.md) | `Disallowed` | Names the cursor shown over cells where the weapon may not be aimed. | Ares |
| [`Nuke.Payload`](../manual/content/keys/nuke.payload--superweapontype.md) | The section's WeaponType | Names the weapon a missile weapon launches. | Ares |
| [`Nuke.SiloLaunch`](../manual/content/keys/nuke.silolaunch--superweapontype.md) | `yes` | Chooses whether a silo launches the missile. | Ares |
| [`ParaDrop.Aircraft`](../manual/content/keys/paradrop.aircraft--superweapontype.md) | `PDPLANE` | Names the aircraft that carries the paratroopers. | Ares |
| [`ParaDrop.Count`](../manual/content/keys/paradrop.count--superweapontype.md) | `1` | Sets how many planes a paradrop weapon sends. | Ares |
| [`ParaDrop.Num`](../manual/content/keys/paradrop.num--superweapontype.md) | The side's paradrop lists | Lists how many of each type a paradrop plane carries. | Ares |
| [`ParaDrop.Types`](../manual/content/keys/paradrop.types--superweapontype.md) | The side's paradrop lists | Lists the infantry a paradrop weapon carries. | Ares |
| [`Protect.Duration`](../manual/content/keys/protect.duration--superweapontype.md) | [CombatDamage] IronCurtainDuration; [General] ForceShieldDuration | Sets how many frames an iron curtain or force shield protects. | Ares |
| [`Protect.PlayFadeSoundTime`](../manual/content/keys/protect.playfadesoundtime--superweapontype.md) | [General] ForceShieldPlayFadeSoundTime | Sets how many frames before a force shield ends its `SpecialSound=` plays. | Ares |
| [`Protect.PowerOutage`](../manual/content/keys/protect.poweroutage--superweapontype.md) | [General] ForceShieldBlackoutDuration | Sets how many frames a force shield shuts off its house's power. | Ares |
| [`SidebarPCX`](../manual/content/keys/sidebarpcx.md) | (none) | The PCX picture drawn as a superweapon's sidebar cameo instead of its `SidebarImage=` shape. | Ares |
| [`SonarPulse.Delay`](../manual/content/keys/sonarpulse.delay--superweapontype.md) | `60` | Sets how long a sonar pulse keeps decloaked objects visible. | Ares |
| [`SpyPlane.Count`](../manual/content/keys/spyplane.count--superweapontype.md) | `1` | Sets how many aircraft a spy plane weapon sends. | Ares |
| [`SpyPlane.Type`](../manual/content/keys/spyplane.type--superweapontype.md) | SPYP, one for each AllyParaDropInf entry | Names the aircraft a spy plane weapon sends. | Ares |
| [`SW.ActivationSound`](../manual/content/keys/sw.activationsound--superweapontype.md) | `none` | Plays a second sound at the target cell when the effect starts. | Ares |
| [`SW.AffectsHouse`](../manual/content/keys/sw.affectshouse--superweapontype.md) | All houses; enemies for a sonar pulse | Chooses whose objects an iron curtain, chronosphere, genetic mutator, psychic dominator, force shield or sonar pulse affects. | Ares |
| [`SW.AffectsTarget`](../manual/content/keys/sw.affectstarget--superweapontype.md) | Everything; infantry and units for a chronosphere or psychic dominator | Chooses which cells and objects an iron curtain, chronosphere, genetic mutator or psychic dominator affects. | Ares |
| [`SW.AIRequiresHouse`](../manual/content/keys/sw.airequireshouse--superweapontype.md) | No limit | Limits the cells the computer may aim this superweapon at by who owns their contents. | Ares |
| [`SW.AIRequiresTarget`](../manual/content/keys/sw.airequirestarget--superweapontype.md) | No limit | Limits the cells the computer may aim this superweapon at by their contents. | Ares |
| [`SW.AITargeting`](../manual/content/keys/sw.aitargeting--superweapontype.md) | Each behavior has its own rule | Chooses how the computer, automatic fire and cameo-click targeting pick a target. | Ares |
| [`SW.AITargeting.Constraints`](../manual/content/keys/sw.aitargeting.constraints--superweapontype.md) | `none` | Names conditions that must hold before the computer fires this superweapon. | Ares |
| [`SW.AllowAI`](../manual/content/keys/sw.allowai--superweapontype.md) | `yes` | Whether a house the computer controls may hold this superweapon. | Ares |
| [`SW.AllowPlayer`](../manual/content/keys/sw.allowplayer--superweapontype.md) | `yes` | Whether a house a person controls may hold this superweapon. | Ares |
| [`SW.AlwaysGranted`](../manual/content/keys/sw.alwaysgranted--superweapontype.md) | `no` | Gives every house the weapon without a structure. | Ares |
| [`SW.Animation`](../manual/content/keys/sw.animation--superweapontype.md) | `none` | Plays an animation on the target cell when the effect starts. | Ares |
| [`SW.AnimationHeight`](../manual/content/keys/sw.animationheight--superweapontype.md) | `5` | Sets how high above the ground `SW.Animation=` plays. | Ares |
| [`SW.AnyDesignator`](../manual/content/keys/sw.anydesignator--superweapontype.md) | `no` | Accepts any of the player's objects as a designator. | Ares |
| [`SW.AnyInhibitor`](../manual/content/keys/sw.anyinhibitor--superweapontype.md) | `no` | Lets any enemy object inhibit this superweapon. | Ares |
| [`SW.AutoFire`](../manual/content/keys/sw.autofire--superweapontype.md) | `no` | Fires the weapon as soon as it is ready, at the cell the computer's rules pick. | Ares |
| [`SW.AuxBuildings`](../manual/content/keys/sw.auxbuildings--superweapontype.md) | `none` | Holds this superweapon only while the house owns one of the listed structures. | Ares |
| [`SW.ChargeToDrainRatio`](../manual/content/keys/sw.chargetodrainratio--superweapontype.md) | [General] ChargeToDrainRatio | Sets how long a charge-draining weapon runs compared with its charge time. | Ares |
| [`SW.CreateRadarEvent`](../manual/content/keys/sw.createradarevent--superweapontype.md) | `no` | Flashes a superweapon event on every player's radar at the target. | Ares |
| [`SW.Damage`](../manual/content/keys/sw.damage--superweapontype.md) | `-1` | Sets the damage a generic warhead, missile strike, lightning bolt or dominator blast deals. | Ares |
| [`SW.Deferment`](../manual/content/keys/sw.deferment--superweapontype.md) | 0, or 20 for UnitDelivery | Delays the weapon's effect after the shot. | Ares |
| [`SW.Designators`](../manual/content/keys/sw.designators--superweapontype.md) | `none` | Requires one of the player's objects of the listed types near the target. | Ares |
| [`SW.FireIntoShroud`](../manual/content/keys/sw.fireintoshroud--superweapontype.md) | `yes` | Whether the weapon may be aimed at unexplored shroud. | Ares |
| [`SW.ForbiddenHouses`](../manual/content/keys/sw.forbiddenhouses--superweapontype.md) | No country | Withholds this superweapon from the listed countries. | Ares |
| [`SW.Group`](../manual/content/keys/sw.group--superweapontype.md) | `0` | Reads a group number for the weapon; the number has no effect yet. | Ares |
| [`SW.Inhibitors`](../manual/content/keys/sw.inhibitors--superweapontype.md) | `none` | Blocks this superweapon near enemy objects of the listed types. | Ares |
| [`SW.InitialReady`](../manual/content/keys/sw.initialready--superweapontype.md) | `no` | Makes the weapon fully charged the first time the house holds it. | Ares |
| [`SW.ManualFire`](../manual/content/keys/sw.manualfire--superweapontype.md) | `yes` | Whether clicking the cameo fires or aims the weapon. | Ares |
| [`SW.MaxCount`](../manual/content/keys/sw.maxcount--superweapontype.md) | `1` | Sets how many launch structures a hunter seeker or EM pulse shot uses. | Ares |
| [`SW.NegBuildings`](../manual/content/keys/sw.negbuildings--superweapontype.md) | `none` | Withdraws this superweapon while the house owns any of the listed structures. | Ares |
| [`SW.PostDependent`](../manual/content/keys/sw.postdependent--superweapontype.md) | the first PostClick=yes weapon whose PreDependent= names ChronoSphere | Names the weapon whose second click completes a chronosphere shot. | Ares |
| [`SW.Range`](../manual/content/keys/sw.range--superweapontype.md) | Computed; see the page | Sets the area an iron curtain, chronosphere, genetic mutator, psychic dominator, psychic reveal, sonar pulse or lightning storm covers. | Ares |
| [`SW.RangeMaximum`](../manual/content/keys/sw.rangemaximum--superweapontype.md) | No limit | Sets how far from its launch structure this superweapon may be aimed. | Ares |
| [`SW.RangeMinimum`](../manual/content/keys/sw.rangeminimum--superweapontype.md) | No limit | Sets how close to its launch structure this superweapon may be aimed. | Ares |
| [`SW.RequiredHouses`](../manual/content/keys/sw.requiredhouses--superweapontype.md) | All countries | Limits this superweapon to the listed countries. | Ares |
| [`SW.RequiresHouse`](../manual/content/keys/sw.requireshouse--superweapontype.md) | No limit | Limits where this superweapon may be aimed by who owns the object on the cell. | Ares |
| [`SW.RequiresTarget`](../manual/content/keys/sw.requirestarget--superweapontype.md) | No limit | Limits where this superweapon may be aimed by what is on the cell. | Ares |
| [`SW.Shots`](../manual/content/keys/sw.shots--superweapontype.md) | `-1` | Limits how many times each house may fire this superweapon. | Ares |
| [`SW.ShowCameo`](../manual/content/keys/sw.showcameo--superweapontype.md) | `yes` | Hides this superweapon's sidebar cameo when set to no. | Ares |
| [`SW.Sound`](../manual/content/keys/sw.sound--superweapontype.md) | `none` | Plays a sound at the target cell when the effect starts. | Ares |
| [`SW.Unstoppable`](../manual/content/keys/sw.unstoppable--superweapontype.md) | `no` | Reads a flag that is meant to keep a running weapon from being switched off; it has no effect yet. | Ares |
| [`SW.UseAITargeting`](../manual/content/keys/sw.useaitargeting--superweapontype.md) | `no` | Fires at the cell the computer's rules pick, when a player clicks the cameo. | Ares |
| [`SW.VirtualCharge`](../manual/content/keys/sw.virtualcharge--superweapontype.md) | `no` | Keeps a weapon's charge while its house does not hold it. | Ares |
| [`SW.Warhead`](../manual/content/keys/sw.warhead--superweapontype.md) | `none` | Names the warhead a generic warhead, missile strike, lightning bolt or dominator blast detonates. | Ares |
| [`Text.Active`](../manual/content/keys/text.active--superweapontype.md) | `Activated` | Names the cameo caption of a charge-draining weapon while it runs. | Ares |
| [`Text.Charging`](../manual/content/keys/text.charging--superweapontype.md) | Charging | Names the cameo caption of a charge-draining weapon while it charges. | Ares |
| [`Text.Hold`](../manual/content/keys/text.hold--superweapontype.md) | On Hold | Names the cameo caption shown while the weapon is suspended. | Ares |
| [`Text.Preparing`](../manual/content/keys/text.preparing--superweapontype.md) | `none` | Reads a caption label for the weapon; it has no effect yet. | Ares |
| [`Text.Ready`](../manual/content/keys/text.ready--superweapontype.md) | `Ready` | Names the cameo caption shown while the weapon is ready. | Ares |

### UnitType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`Totable`](../manual/content/keys/totable.md) | `yes` | Whether a carryall may pick this vehicle up. | OpenYR |
| [`TurretBob`](../manual/content/keys/turretbob--unittype.md) | `no` | With `yes`, the turret and barrel of a voxel vehicle shift up by the hull's frame number. | OpenYR |
| [`WreckReleasesPassengers`](../manual/content/keys/wreckreleasespassengers--unittype.md) | `yes` | A vehicle whose artwork sets `DeathFrames` above `0` becomes a wreck when it is destroyed, and stays on the map until its `MaxDeathCounter` passes. | OpenYR |

### WarheadType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AffectsEnemies`](../manual/content/keys/affectsenemies.md) | `yes` | With no, the warhead does nothing to objects whose owner is not an ally of its attacker. | Ares |
| [`AttachEffect.Animation`](../manual/content/keys/attacheffect.animation--warheadtype.md) | `none` | The animation shown on an object while an attached effect lasts. | Ares |
| [`AttachEffect.AnimResetOnReapply`](../manual/content/keys/attacheffect.animresetonreapply--warheadtype.md) | `no` | `AttachEffect.AnimResetOnReapply=yes` restarts the effect animation when a hit restarts an effect the object already carries. | Ares |
| [`AttachEffect.ArmorMultiplier`](../manual/content/keys/attacheffect.armormultiplier--warheadtype.md) | `1.0` | Divides the damage an object takes while an attached effect lasts. | Ares |
| [`AttachEffect.Cloakable`](../manual/content/keys/attacheffect.cloakable--warheadtype.md) | `no` | Whether an object can cloak while an attached effect lasts. | Ares |
| [`AttachEffect.Cumulative`](../manual/content/keys/attacheffect.cumulative--warheadtype.md) | `no` | `AttachEffect.Cumulative=yes` makes every hit of this warhead add a new copy of its effect, each with its own duration and animation, and their multipliers combine. | Ares |
| [`AttachEffect.DiscardOnEntry`](../manual/content/keys/attacheffect.discardonentry--warheadtype.md) | `no` | Whether an attached effect is removed when the object leaves the map. | Ares |
| [`AttachEffect.Duration`](../manual/content/keys/attacheffect.duration--warheadtype.md) | `0` | How many frames an attached effect lasts; 0 attaches nothing. | Ares |
| [`AttachEffect.FirepowerMultiplier`](../manual/content/keys/attacheffect.firepowermultiplier--warheadtype.md) | `1.0` | Multiplies the damage of the shots an object fires while an attached effect lasts. | Ares |
| [`AttachEffect.ForceDecloak`](../manual/content/keys/attacheffect.forcedecloak--warheadtype.md) | `no` | Whether attaching an effect uncloaks the object. | Ares |
| [`AttachEffect.PenetratesIronCurtain`](../manual/content/keys/attacheffect.penetratesironcurtain--warheadtype.md) | `no` | Whether an effect can be attached to an object under the Iron Curtain or a Force Shield. | Ares |
| [`AttachEffect.ROFMultiplier`](../manual/content/keys/attacheffect.rofmultiplier--warheadtype.md) | `1.0` | Multiplies the reload time of an object while an attached effect lasts. | Ares |
| [`AttachEffect.SpeedMultiplier`](../manual/content/keys/attacheffect.speedmultiplier--warheadtype.md) | `1.0` | Multiplies the movement speed of an object while an attached effect lasts. | Ares |
| [`AttachEffect.TemporalHidesAnim`](../manual/content/keys/attacheffect.temporalhidesanim--warheadtype.md) | `no` | Whether an attached effect's animation is hidden while a temporal weapon warps the object out. | Ares |
| [`EffectsRequireDamage`](../manual/content/keys/effectsrequiredamage.md) | `no` | With yes, the extra effects of a warhead reach an object only when the hit damages it by at least one point. | Ares |
| [`EffectsRequireVerses`](../manual/content/keys/effectsrequireverses.md) | `yes` | With no, the extra effects of a warhead reach an object whose armor has a Verses of 0%. | Ares |
| [`EMP.Cap`](../manual/content/keys/emp.cap.md) | -1, so a hit sets the stun to its duration unless the object already has more | How an EMP.Duration hit combines with the stun frames an object already has. | Ares |
| [`EMP.Duration`](../manual/content/keys/emp.duration.md) | 0, so the warhead stuns nothing through this key | Makes a warhead stun the objects its blast damages for this many frames, or reactivate them when negative. | Ares |
| [`InfDeathAnim`](../manual/content/keys/infdeathanim.md) | (none) | The animation an infantryman dies with when this warhead kills him, in place of InfDeath. | Ares |
| [`KillDriver`](../manual/content/keys/killdriver.md) | `no` | With yes, a hit takes the driver of a vehicle in place of damaging it. | Ares |
| [`KillDriver.Chance`](../manual/content/keys/killdriver.chance.md) | `100%` | The chance that a hit takes the driver of a vehicle that has met every other condition. | Ares |
| [`KillDriver.KillBelowPercent`](../manual/content/keys/killdriver.killbelowpercent.md) | `100%` | The share of its health a vehicle must be at or below for the warhead to take its driver. | Ares |
| [`KillDriver.Owner`](../manual/content/keys/killdriver.owner.md) | `special` | The house that takes a vehicle whose driver a KillDriver warhead killed. | Ares |
| [`KillDriver.RemoveVeterancy`](../manual/content/keys/killdriver.removeveterancy.md) | `no` | With yes, a vehicle whose driver was killed goes back to the rookie rank. | Ares |
| [`PreImpactAnim`](../manual/content/keys/preimpactanim.md) | `NUKEBALL` for the warhead named `NUKE`, otherwise none, so the blast goes off at once | An animation played where a projectile with this warhead hits, before the blast goes off. | Ares |
| [`PreventScatter`](../manual/content/keys/preventscatter.md) | `no` | With yes, an object this warhead damages does not scatter because of the hit. | Ares |

### WeaponType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`Abductor`](../manual/content/keys/abductor.md) | `no` | With yes, a hit from this weapon pulls its target into the firer's passenger hold instead of damaging it. | Ares |
| [`Abductor.AbductBelowPercent`](../manual/content/keys/abductor.abductbelowpercent.md) | `100%` | The largest share of its strength a target may have left for an Abductor weapon to take it. | Ares |
| [`Abductor.Anim`](../manual/content/keys/abductor.anim.md) | `none` | The animation an Abductor weapon plays where it takes its target from. | Ares |
| [`Abductor.ChangeOwner`](../manual/content/keys/abductor.changeowner.md) | `no` | With yes, an object an Abductor weapon takes aboard changes to the firer's house. | Ares |
| [`Beam.Color`](../manual/content/keys/beam.color.md) | `ChronoBeamColor` for a weapon whose warhead is `Temporal=yes`, otherwise `RadColor` | The colour of the radiation beam of this weapon. | Ares |
| [`Bolt.Color1`](../manual/content/keys/bolt.color1.md) | Depends; see the page | The colour of the first strand of this weapon's electric bolt. | Ares |
| [`Bolt.Color2`](../manual/content/keys/bolt.color2.md) | Depends; see the page | The colour of the second strand of this weapon's electric bolt. | Ares |
| [`Bolt.Color3`](../manual/content/keys/bolt.color3.md) | The palette colour: the third strand uses colour 15 of the palette | The colour of the third strand of this weapon's electric bolt. | Ares |
| [`Emissive`](../manual/content/keys/emissive--weapontype.md) | Computed; see the page | `Emissive` on a weapon sets the strength of the beam or electric bolt the weapon draws. | OpenYR |
| [`IsDetachedRailgun`](../manual/content/keys/isdetachedrailgun.md) | `no` | Makes a weapon fire the railgun beam of IsRailgun without waiting for the previous beam to fade. | Ares |
| [`IvanBomb.AttachSound`](../manual/content/keys/ivanbomb.attachsound.md) | `BombAttachSound` from `[AudioVisual]` | The sound played when a bomb this weapon plants is attached. | Ares |
| [`IvanBomb.CanDetonateTimeBomb`](../manual/content/keys/ivanbomb.candetonatetimebomb.md) | `CanDetonateTimeBomb` from `[CombatDamage]` | Whether the player can set off a bomb this weapon plants by hand. | Ares |
| [`IvanBomb.Damage`](../manual/content/keys/ivanbomb.damage.md) | `IvanDamage` from `[CombatDamage]` | The damage a bomb this weapon plants deals when it goes off. | Ares |
| [`IvanBomb.Delay`](../manual/content/keys/ivanbomb.delay.md) | `IvanTimedDelay` from `[CombatDamage]` | The frames before a bomb this weapon plants goes off. | Ares |
| [`IvanBomb.DestroysBridges`](../manual/content/keys/ivanbomb.destroysbridges.md) | `yes` | Whether a bomb this weapon plants on a bridge repair hut destroys the bridge. | Ares |
| [`IvanBomb.Detachable`](../manual/content/keys/ivanbomb.detachable.md) | `yes` | Whether a bomb this weapon plants can be removed by a bomb disarming weapon. | Ares |
| [`IvanBomb.DetonateOnSell`](../manual/content/keys/ivanbomb.detonateonsell.md) | no, so selling the structure removes the bomb | Whether a bomb this weapon plants goes off when the structure carrying it is sold. | Ares |
| [`IvanBomb.Image`](../manual/content/keys/ivanbomb.image.md) | ``BOMBCURS`` | The icon file shown over an object carrying a bomb this weapon planted. | Ares |
| [`IvanBomb.TickingSound`](../manual/content/keys/ivanbomb.tickingsound.md) | `BombTickingSound` from `[AudioVisual]` | The sound a bomb this weapon plants makes while it counts down. | Ares |
| [`IvanBomb.Warhead`](../manual/content/keys/ivanbomb.warhead.md) | `IvanWarhead` from `[CombatDamage]` | The warhead a bomb this weapon plants goes off with. | Ares |

### [AI]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`FireSaleKeepThreshold`](../manual/content/keys/firesalekeepthreshold.md) | `8` | How strong a house must be for a Short Game fire sale to leave one structure standing. | OpenTS |
| [`FireSaleStructureWeight`](../manual/content/keys/firesalestructureweight.md) | `2` | What each structure adds to a house's forces when a Short Game fire sale decides whether to keep one. | OpenTS |

### [AudioVisual]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AnimationFires`](../manual/content/keys/animationfires.md) | `no` | Whether Flamer and Scorch animations, and damaged or destroyed structures, light SmallFire and LargeFire fires. | OpenYR |
| [`BountyDisplay`](../manual/content/keys/bountydisplay.md) | `no` | Whether bounty payments are shown over the destroyed object, for destroyers that do not set Bounty.Display. | Ares |
| [`VoxelShadeScale`](../manual/content/keys/voxelshadescale--global-rules.md) | `16` | Sets how many palette shade rows a voxel's diffuse and highlight brightness spans. | OpenYR |
| [`VoxelSpecularFromView`](../manual/content/keys/voxelspecularfromview--global-rules.md) | `no` | Takes the viewer direction for the voxel highlight from the isometric view alone, not from the voxel's own transform. | OpenYR |
| [`VoxelSpecularStrength`](../manual/content/keys/voxelspecularstrength--global-rules.md) | `3` | Sets the strength of the highlight that units, aircraft and buildings drawn as voxels take from the viewer's direction. | OpenYR |

### [CombatDamage]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AlliedSolidTransparency`](../manual/content/keys/alliedsolidtransparency.md) | `no` | Lets SubjectToBuildings shots fly through solid buildings owned by the firer's allies. | Ares |
| [`OpenTransport.DamageMultiplier`](../manual/content/keys/opentransport.damagemultiplier--global-rules.md) | `1.0` | `OpenTransport.DamageMultiplier` in `[CombatDamage]` multiplies the damage of a passenger's shots from an `OpenTopped=yes` transport, unless the passenger's own type sets ... | Phobos |
| [`OpenTransport.RangeBonus`](../manual/content/keys/opentransport.rangebonus--global-rules.md) | `0` | `OpenTransport.RangeBonus` in `[CombatDamage]` is the range bonus, in whole cells, that a passenger adds for itself when firing from an `OpenTopped=yes` transport, unless its own ... | Phobos |

### [CrateRules]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`ArmorCrateStacks`](../manual/content/keys/armorcratestacks--global-rules.md) | `no` | Lets an armor crate upgrade an object that an earlier armor crate already upgraded. | OpenTS |
| [`CrateMoneyBonus`](../manual/content/keys/cratemoneybonus--global-rules.md) | `900` | The most credits a money crate adds at random to its configured amount. | OpenTS |
| [`FirepowerCrateStacks`](../manual/content/keys/firepowercratestacks--global-rules.md) | `no` | Lets a firepower crate upgrade an object that an earlier firepower crate already upgraded. | OpenTS |

### [General]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AircraftWaypointReload`](../manual/content/keys/aircraftwaypointreload--global-rules.md) | `yes` | Lets an armed aircraft with limited ammunition take planned orders in planning mode, and resume them after it rearms at a dock. | OpenYR |
| [`BountyEnablers`](../manual/content/keys/bountyenablers.md) | `none` | The structures a house must own before it collects any bounty. | Ares |
| [`ChronoInfantryCrush`](../manual/content/keys/chronoinfantrycrush.md) | `yes` | Sets whether a chronoshifted infantryman destroys a vehicle or aircraft on its landing cell, or dies instead. | Ares |
| [`EnemyInsignia`](../manual/content/keys/enemyinsignia.md) | `yes` | Whether players not allied with an object's owner see its rank insignia, unless the type decides. | Ares, Phobos |
| [`LargeSidebarStrips`](../manual/content/keys/largesidebarstrips--global-rules.md) | `no` | Lets each sidebar strip hold 225 cameos instead of 75. | OpenYR |
| [`LightningRodRange`](../manual/content/keys/lightningrodrange.md) | `4` | How many cells away a lightning storm cloud is drawn to a structure with LightningRod=yes. | OpenYR |
| [`MultipleFactoryCap`](../manual/content/keys/multiplefactorycap--global-rules.md) | `0` | The most factories of one category that count toward the MultipleFactory build-time adjustment. | OpenTS |
| [`MutateFriendly`](../manual/content/keys/mutatefriendly.md) | `yes` | Whether the genetic mutator also mutates infantry of the firing house and its allies. | OpenYR |
| [`RecheckPrerequisites`](../manual/content/keys/recheckprerequisites--global-rules.md) | `yes` | Removes a type's sidebar cameo and cancels its production once the house no longer meets the type's prerequisites, tech level or ownership rules. | OpenTS |
| [`SelfHealCap`](../manual/content/keys/selfhealcap.md) | `-1` | The share of maximum strength at which self-healing stops. | OpenTS |
| [`SelfHealRate`](../manual/content/keys/selfhealrate.md) | `-1` | The interval between self-healing ticks. | OpenTS |
| [`SelfHealStep`](../manual/content/keys/selfhealstep.md) | `1` | The strength one self-healing tick restores. | OpenTS |

## Art

### AnimType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`CustomPalette`](../manual/content/keys/custompalette.md) | none; the animation is drawn through the shared animation palette | Names a palette file that the animation is drawn through instead of the shared animation palette. | Ares |
| [`Damage.Delay`](../manual/content/keys/damage.delay.md) | 0, so the damage applies on every animation frame | The animation frames between two applications of an animation's Damage. | Ares |
| [`Emissive`](../manual/content/keys/emissive--animtype.md) | Computed; see the page | `Emissive` sets how strongly the animation's picture can rise above paper white on HDR output. | OpenYR |

### Object types

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AltCameoPCX`](../manual/content/keys/altcameopcx.md) | (none) | The PCX picture drawn as an object's sidebar cameo in place of its alternate cameo. | Ares |
| [`CameoPCX`](../manual/content/keys/cameopcx.md) | (none) | The PCX picture drawn as an object's sidebar cameo instead of its `Cameo=` shape. | Ares |

### UnitType

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`StartTurretFrame`](../manual/content/keys/startturretframe.md) | 8 × WalkFrames, counted from frame 0, whatever Facings holds | The frame a shape-drawn vehicle's turret artwork begins at. | OpenYR |
| [`TurretFacings`](../manual/content/keys/turretfacings.md) | `32` | The number of facings a shape-drawn vehicle's turret artwork is cut into. | OpenYR |

## Client settings

Example:

```ini
[Options]
InterfaceStyle=Modern
MenuStyle=Classic
SidebarRows=4
SyncLog=no

[Video]
InterfaceScale=Auto
RenderFrameRate=60
SystemCursor=no
```

### [Debug]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`HangSeconds`](../manual/content/keys/hangseconds.md) | `5` | How long the main thread may go without handling messages before the game reports a hang. Zero turns the check off. | OpenYR |

### [Intro]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`PlayIntro`](../manual/content/keys/playintro.md) | `yes` | Controls the one-time `EVA.VQA` startup movie. | OpenTS |

### [MultiPlayer]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`CustomColor`](../manual/content/keys/customcolor--multiplayer-settings.md) | `0` | The value is a free color packed into one number: bit 24 marks it as a free color, bits 16 to 23 hold the hue, bits 8 to 15 the saturation and bits 0 to 7 the value. | OpenYR |

### [Options]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AltToRally`](../manual/content/keys/alttorally.md) | `no` | Makes the force-move key set a factory's rally point and the plain click move the structure. | OpenTS |
| [`AutoSaveInterval`](../manual/content/keys/autosaveinterval.md) | `10800` | How many frames pass between one automatic save and the next in a game started from the menu, or zero for none. | OpenTS |
| [`BitmapSystemFont`](../manual/content/keys/bitmapsystemfont--client-settings.md) | `no` | `BitmapSystemFont=yes` draws the lists, text boxes, tooltips, hotkey fields and group headings of the game's screens in the fixed-size bitmap face that Windows ships, the face the ... | OpenTS |
| [`DebugMode`](../manual/content/keys/debugmode--client-settings.md) | `no` | `DebugMode=yes` turns on the debug cheat commands for single-player games, which are campaign and skirmish games. | OpenYR |
| [`InterfaceStyle`](../manual/content/keys/interfacestyle--client-settings.md) | `Modern` | `InterfaceStyle=` chooses how the in-game interface is laid out. | OpenYR |
| [`MenuStyle`](../manual/content/keys/menustyle--client-settings.md) | `Modern` | `MenuStyle=` chooses how the menus look. | OpenYR |
| [`Mods`](../manual/content/keys/mods--client-settings.md) | The list in `Mods=` under `[Paths]` in `OPENTS.INI`, or no mods when that file gives none | The mods this player starts the game with, in the order they are read. | OpenYR |
| [`ScreenShake`](../manual/content/keys/screenshake--client-settings.md) | `yes` | `ScreenShake=yes` shakes the picture when a warhead or an ion blast detonates. | OpenYR |
| [`SidebarRows`](../manual/content/keys/sidebarrows--client-settings.md) | `4` | `SidebarRows=` sets how many rows of cameos the modern build panel shows at once. | OpenYR |
| [`SidebarSorting`](../manual/content/keys/sidebarsorting.md) | `yes` | Keeps each sidebar strip in a fixed order that depends only on the rules. | OpenTS |
| [`SimulateWhileUnfocused`](../manual/content/keys/simulatewhileunfocused.md) | `no` | Keeps a campaign or skirmish running while another program has the focus. | OpenTS |
| [`SyncLog`](../manual/content/keys/synclog--client-settings.md) | `no` | `SyncLog=yes` writes the frame check value to the debug log on every game frame, as the `-SYNCLOG` command-line switch does. | OpenYR |
| [`TextBackgroundColor`](../manual/content/keys/textbackgroundcolor--client-settings.md) | `12` | `TextBackgroundColor` is the palette index drawn behind every character of the in-game message list and of the line being typed. | OpenTS |

### [Video]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`Brightness`](../manual/content/keys/brightness--client-settings.md) | `100` | `Brightness=` sets the brightness of the picture as a whole number of percent. | OpenYR |
| [`ColorBlindMode`](../manual/content/keys/colorblindmode--client-settings.md) | `Off` | `ColorBlindMode=` chooses a colour-blind correction for the picture the game shows. | OpenYR |
| [`ColorBlindStrength`](../manual/content/keys/colorblindstrength--client-settings.md) | `100` | `ColorBlindStrength=` sets how much of the `ColorBlindMode` correction applies, as a whole number of percent. | OpenYR |
| [`CursorScale`](../manual/content/keys/cursorscale.md) | `0` | How many times larger than its artwork the mouse pointer is drawn. | OpenTS |
| [`EmissiveDebugView`](../manual/content/keys/emissivedebugview.md) | `no` | Shows the emissive masks in place of the art, even without HDR output. | OpenYR |
| [`FrameRateLimit`](../manual/content/keys/frameratelimit.md) | `-1` | The most presents per second the game makes. Zero is unlimited. | OpenYR |
| [`Fullscreen`](../manual/content/keys/fullscreen.md) | `yes` | Whether the game covers the whole screen instead of running in a resizable window. | OpenTS |
| [`Gamma`](../manual/content/keys/gamma--client-settings.md) | `1.0` | `Gamma=` sets the exponent of the curve the picture's light follows, as a number. | OpenYR |
| [`HDRHighlights`](../manual/content/keys/hdrhighlights.md) | `Enhanced` | Whether light sources may be drawn brighter than paper white on HDR output. | OpenYR |
| [`HDROutput`](../manual/content/keys/hdroutput--client-settings.md) | `Automatic` | `HDROutput=` chooses whether the game sends its picture to the display as HDR10. | OpenYR |
| [`HDRPaperWhite`](../manual/content/keys/hdrpaperwhite--client-settings.md) | `Auto` | `HDRPaperWhite=` sets the brightness, in nits, at which SDR white is drawn when HDR output is on. | OpenYR |
| [`HDRPeakBrightness`](../manual/content/keys/hdrpeakbrightness--client-settings.md) | `Auto` | `HDRPeakBrightness=` sets the highest brightness, in nits, that HDR output draws. | OpenYR |
| [`IntegerScaling`](../manual/content/keys/integerscaling.md) | `no` | Whether the picture is only ever enlarged by whole multiples. | OpenTS |
| [`InterfaceScale`](../manual/content/keys/interfacescale--client-settings.md) | `Auto` | `InterfaceScale=` sets how many times larger than its original size the in-game interface is drawn: the sidebar and its cameos, the radar, the top tabs, the command bar, tooltips ... | OpenYR |
| [`Renderer`](../manual/content/keys/renderer.md) | `0` | Which graphics interface the game draws through, as a number. | OpenTS |
| [`RenderFrameRate`](../manual/content/keys/renderframerate.md) | `-1` | The most extra pictures per second the game draws between game frames. | OpenYR |
| [`ScaleMode`](../manual/content/keys/scalemode.md) | `PixelArt` | How the picture is filtered when it is scaled into the window. | OpenTS |
| [`ShowFPS`](../manual/content/keys/showfps--client-settings.md) | `no` | `ShowFPS=yes` draws a frame count in the top-left corner of the game view. | OpenYR |
| [`SystemCursor`](../manual/content/keys/systemcursor--client-settings.md) | `yes` | `SystemCursor=yes` shows the Windows mouse pointer wherever the game would show its green arrow: over its menus and dialogs, and wherever no battlefield pointer is up. | OpenTS |
| [`VSync`](../manual/content/keys/vsync.md) | `yes` | Whether the renderer waits for the display's refresh before showing a finished frame. | OpenTS |
| [`WindowHeight`](../manual/content/keys/windowheight.md) | `-1` | The height of the drawable area of the game window, in pixels. | OpenTS |
| [`WindowWidth`](../manual/content/keys/windowwidth.md) | `-1` | The width of the drawable area of the game window, in pixels. | OpenTS |

## UI.INI

### [Ingame]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AlwaysShowActionLines`](../manual/content/keys/alwaysshowactionlines.md) | `no` | Keeps a selected object's action lines drawn for as long as it stays selected. | OpenTS |
| [`MovementLineColor`](../manual/content/keys/movementlinecolor.md) | `0,170,0` | The color of the movement line, as red, green and blue from 0 to 255. | OpenTS |
| [`MovementLineDashed`](../manual/content/keys/movementlinedashed.md) | `no` | Draws the movement line as dashes rather than a solid line. | OpenTS |
| [`MovementLineDropShadow`](../manual/content/keys/movementlinedropshadow.md) | `no` | Draws a shadow below the movement line. | OpenTS |
| [`MovementLineDropShadowColor`](../manual/content/keys/movementlinedropshadowcolor.md) | `0,0,0` | The color of the shadow below the movement line, as red, green and blue from 0 to 255. | OpenTS |
| [`MovementLineThick`](../manual/content/keys/movementlinethick.md) | `no` | Draws the movement line two rows high. | OpenTS |
| [`NavComQueueLineColor`](../manual/content/keys/navcomqueuelinecolor.md) | `74,77,255` | The color of the queued-destination lines, as red, green and blue from 0 to 255. | OpenTS |
| [`NavComQueueLineDashed`](../manual/content/keys/navcomqueuelinedashed.md) | `no` | Draws the queued-destination lines as dashes rather than solid lines. | OpenTS |
| [`NavComQueueLineDropShadow`](../manual/content/keys/navcomqueuelinedropshadow.md) | `no` | Draws a shadow below the queued-destination lines. | OpenTS |
| [`NavComQueueLineDropShadowColor`](../manual/content/keys/navcomqueuelinedropshadowcolor.md) | `0,0,0` | The color of the shadow below the queued-destination lines, as red, green and blue from 0 to 255. | OpenTS |
| [`NavComQueueLineThick`](../manual/content/keys/navcomqueuelinethick.md) | `no` | Draws the queued-destination lines two rows high. | OpenTS |
| [`ShowNavComQueueLines`](../manual/content/keys/shownavcomqueuelines.md) | `yes` | Draws the destinations queued behind a selected object's movement line as further lines. | OpenTS |
| [`TargetLaserColor`](../manual/content/keys/targetlasercolor.md) | `173,0,0` | The color of the sighting laser, as red, green and blue from 0 to 255. | OpenTS |
| [`TargetLaserDashed`](../manual/content/keys/targetlaserdashed.md) | `yes` | Draws the sighting laser as dashes rather than a solid line. | OpenTS |
| [`TargetLaserDropShadow`](../manual/content/keys/targetlaserdropshadow.md) | `no` | Draws a shadow below the sighting laser. | OpenTS |
| [`TargetLaserDropShadowColor`](../manual/content/keys/targetlaserdropshadowcolor.md) | `0,0,0` | The color of the shadow below the sighting laser, as red, green and blue from 0 to 255. | OpenTS |
| [`TargetLaserThick`](../manual/content/keys/targetlaserthick.md) | `no` | Draws the sighting laser two rows high. | OpenTS |
| [`TargetLaserTime`](../manual/content/keys/targetlasertime.md) | `15` | Frames the sighting laser stays drawn after a vehicle fires. | OpenTS |
| [`TargetLineColor`](../manual/content/keys/targetlinecolor.md) | `173,0,0` | The color of the target line, as red, green and blue from 0 to 255. | OpenTS |
| [`TargetLineDashed`](../manual/content/keys/targetlinedashed.md) | `no` | Draws the target line as moving dashes. | OpenTS |
| [`TargetLineDropShadow`](../manual/content/keys/targetlinedropshadow.md) | `no` | Draws a shadow below the target line. | OpenTS |
| [`TargetLineDropShadowColor`](../manual/content/keys/targetlinedropshadowcolor.md) | `0,0,0` | The color of the shadow below the target line, as red, green and blue from 0 to 255. | OpenTS |
| [`TargetLineThick`](../manual/content/keys/targetlinethick.md) | `no` | Draws the target line two rows high. | OpenTS |

### [Pips]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`AircraftGroupNumberOffset`](../manual/content/keys/aircraftgroupnumberoffset.md) | `-4,-4` | Where a selected aircraft whose type has no pips prints its control group number. | OpenTS |
| [`AircraftWithPipGroupNumberOffset`](../manual/content/keys/aircraftwithpipgroupnumberoffset.md) | `-4,-8` | Where a selected aircraft whose type has pips prints its control group number. | OpenTS |
| [`BuildingGroupNumberOffset`](../manual/content/keys/buildinggroupnumberoffset.md) | `-4,-4` | Where a selected structure whose type has no pips prints its control group number. | OpenTS |
| [`BuildingWithPipGroupNumberOffset`](../manual/content/keys/buildingwithpipgroupnumberoffset.md) | `-4,-8` | Where a selected structure whose type has pips prints its control group number. | OpenTS |
| [`InfantryGroupNumberOffset`](../manual/content/keys/infantrygroupnumberoffset.md) | `-4,-4` | Where a selected infantry unit whose type has no pips prints its control group number. | OpenTS |
| [`InfantryWithPipGroupNumberOffset`](../manual/content/keys/infantrywithpipgroupnumberoffset.md) | `-4,-8` | Where a selected infantry unit whose type has pips prints its control group number. | OpenTS |
| [`UnitGroupNumberOffset`](../manual/content/keys/unitgroupnumberoffset.md) | `-4,-4` | Where a selected vehicle whose type has no pips prints its control group number. | OpenTS |
| [`UnitWithPipGroupNumberOffset`](../manual/content/keys/unitwithpipgroupnumberoffset.md) | `-4,-8` | Where a selected vehicle whose type has pips prints its control group number. | OpenTS |

## Other game files

### Map and scenario files: [Basic]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`RandomMap`](../manual/content/keys/randommap.md) | `no` | Builds the scenario's map with the random map generator instead of reading it from the file. | OpenTS |
| [`UseMPAIBaseNodes`](../manual/content/keys/usempaibasenodes.md) | `no` | Whether the computer houses of a skirmish or multiplayer game follow the base plans the map wrote for their start positions. | OpenYR |

### Map and scenario files: [SpecialFlags]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`ScrapMetal`](../manual/content/keys/scrapmetal.md) | `no` | Whether wrecks leave the scrap animations their types name instead of their ordinary ones. | OpenTS |

### Map packets (`.pkt`)

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`DescriptionText`](../manual/content/keys/descriptiontext--map-packets.md) | The row shows the text that the section's Description gives it | The value is the map's row text in the multiplayer scenario list, shown as written. | OpenYR |

### Music registry (`THEMEMD.INI`)

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`RequiredAddon`](../manual/content/keys/requiredaddon-campaign--themes.md) | `0` | The expansion a campaign belongs to, or the expansion a music track needs. | OpenTS |
| [`Sound`](../manual/content/keys/sound--themes.md) | The track's ID, which is also its section name | `Sound=` names the file the music track plays, without an extension. | OpenTS |
| [`Volume`](../manual/content/keys/volume--themes.md) | `1.0` | The share of full loudness a sound effect or a music track is played at. | OpenTS |

### Music registry (`THEMEMD.INI`): [General]

| Key | Default | Effect | From |
| --- | --- | --- | --- |
| [`CrossFade`](../manual/content/keys/crossfade--themes.md) | `0` | `CrossFade=` in THEME.INI's `[General]` section sets, in seconds, how long a queued music track overlaps the track it replaces. | OpenTS |
| [`FadeOut`](../manual/content/keys/fadeout--themes.md) | `1.5` | `FadeOut=` in THEME.INI's `[General]` section sets how many seconds the current music track takes to fade out when it gives way to a queued track, when the player presses Stop on ... | OpenTS |
| [`IonStormVolume`](../manual/content/keys/ionstormvolume--themes.md) | `0.33` | `IonStormVolume=` in THEME.INI's `[General]` section sets how loud the music plays while an ion storm plays its storm sound, as a share of the music's usual level. | OpenTS |

## SPAWN.INI

Unpatched Yuri's Revenge reads no launch file. A client such as CnCNet writes `SPAWN.INI` beside the game and starts it with `-SPAWN`, and we read the keys below from it. The [launch file page](../manual/content/formats/spawn-ini.md) owns the details, including what makes a launch fail.

Example:

```ini
[Settings]
Scenario=spawnmap.ini
Name=Alice
Side=0
Color=0
AIPlayers=1
Credits=10000
```

| Key | Default | Effect |
| --- | --- | --- |
| [`[Settings] Scenario`](../manual/content/formats/spawn-ini.md#what-the-file-asks-for) | `spawnmap.ini` | The scenario file to play. |
| [`[Settings] IsSinglePlayer`](../manual/content/formats/spawn-ini.md#what-the-file-asks-for) | `no` | `yes` plays a campaign mission instead of a match. |
| [`[Settings] LoadSaveGame`](../manual/content/formats/spawn-ini.md#resuming-a-saved-game) | `no` | `yes` resumes the saved game `SaveGameName` names. |
| [`[Settings] SaveGameName`](../manual/content/formats/spawn-ini.md#resuming-a-saved-game) | (none) | The saved game to resume, written as a file name from the saved-games folder. |
| [`[Settings] Host`](../manual/content/formats/spawn-ini.md#the-host) | `no` | `yes` marks this machine as the host of a game against other machines. |
| [`[Settings] SyncLog`](../manual/content/formats/spawn-ini.md#what-the-file-asks-for) | `no` | `yes` writes the frame check value to the debug log on every frame. |
| [`[Settings] CampaignID`](../manual/content/formats/spawn-ini.md#a-campaign-mission) | `-1` | The campaign a campaign mission belongs to, counted from `0`. `-1` plays it outside any campaign. |
| [`[Settings] DifficultyModeHuman`](../manual/content/formats/spawn-ini.md#a-campaign-mission) | `1` | The difficulty slot, `0` to `2`, of the player's houses in a campaign mission. |
| [`[Settings] DifficultyModeComputer`](../manual/content/formats/spawn-ini.md#a-campaign-mission) | `1` | The difficulty slot, `0` to `2`, of the computer's houses in a campaign mission. |
| [`[Settings] DifficultyName`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | (none) | Replaces the difficulty name in the message a campaign mission opens with. |
| [`[Settings] Bases`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | Whether houses start with a base unit. A match option. |
| [`[Settings] Credits`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `10000` | The credits each house starts with. A match option. |
| [`[Settings] BridgeDestroy`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | Whether bridges can be destroyed. A match option. |
| [`[Settings] Crates`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | Whether crates appear. A match option. |
| [`[Settings] ShortGame`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | The Short Game option. A match option. |
| [`[Settings] SuperWeapons`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | `no` withholds the superweapons whose `DisableableFromShell` is `yes`. |
| [`[Settings] GameSpeed`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `0` | The game speed, `0` the fastest to `6` the slowest. |
| [`[Settings] MultiEngineer`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | The Multi Engineer option. A match option. |
| [`[Settings] UnitCount`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `0` | How many starting units each house gets. A match option. |
| [`[Settings] AIPlayers`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `0` | The number of computer players, from `0` to 8 minus the number of people. |
| [`[Settings] AIDifficulty`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `1` | The computer players' difficulty: `0` easy, `1` normal, `2` hard. |
| [`[Settings] AlliesAllowed`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `no` stops players from changing alliances during play. |
| [`[Settings] GameMode`](../manual/content/formats/spawn-ini.md#the-multiplayer-mode) | `1` | The index of a multiplayer mode in `MPModesMD.ini`; `1` is Battle. |
| [`[Settings] HarvesterTruce`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | The harvester truce option. A skirmish ignores it. |
| [`[Settings] FogOfWar`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | The Fog Of War option. A match option. |
| [`[Settings] MCVRedeploy`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | The Re-Deployable MCV option. A match option. |
| [`[Settings] AutoDeployMCV`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `yes` deploys every house's starting base unit as the match opens. |
| [`[Settings] TechLevel`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `10` | The tech level of the match. A match option. |
| [`[Settings] Firestorm`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | `yes` plays the expansion when it is installed. |
| [`[Settings] Seed`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `0` | A nonzero value makes a launch repeatable. In a game against other machines every machine uses it as written. |
| [`[Settings] CoachMode`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | Decides what a defeated player keeps. |
| [`[Settings] AutoSurrender`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `yes` | `yes` destroys the base of a player who leaves; `no` hands it to the computer. |
| [`[Settings] BuildOffAlly`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `yes` lets a player place buildings next to an ally's buildings whose type allows it. |
| [`[Settings] AttackNeutralUnits`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `yes` lets a target scan consider a neutral house. |
| [`[Settings] ScrapMetal`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `yes` makes a wreck play its `ScrapExplosion` animations instead of `Explosion`. |
| [`[Settings] PlayMoviesInMultiplayer`](../manual/content/formats/spawn-ini.md#the-options-every-house-plays-under) | `no` | `yes` plays the scenario's movies in a skirmish or a game against other machines. |
| [`[Settings] AutoSaveGame`](../manual/content/formats/spawn-ini.md#automatic-saves) | `0` | The number of frames between automatic saves. `0` or less turns them off. It wins over `AutoSaveInterval`. |
| [`[Settings] AutoSaveInterval`](../manual/content/formats/spawn-ini.md#automatic-saves) | `0` | The number of frames between automatic saves when `AutoSaveGame` is not written. |
| [`[Settings] NextSPAutoSaveId`](../manual/content/formats/spawn-ini.md#automatic-saves) | `1` | The next slot, `1` to `5`, of the campaign's rotating automatic saves. |
| [`[Settings] NextSkirmishAutoSaveId`](../manual/content/formats/spawn-ini.md#automatic-saves) | `1` | The next slot, `1` to `5`, of the skirmish's rotating automatic saves. |
| [`[Settings] Name`](../manual/content/formats/spawn-ini.md#who-is-playing) | (none) | The person's name as UTF-8 text, shortened to 63 bytes. Also read in `[Other1]` to `[Other7]`. |
| [`[Settings] Side`](../manual/content/formats/spawn-ini.md#who-is-playing) | (none) | The person's country, numbered from `0` in the order of the rules' `[Countries]` list. Required. |
| [`[Settings] Color`](../manual/content/formats/spawn-ini.md#who-is-playing) | (none) | The person's player color, `0` to `7`. Required. |
| [`[Settings] IsSpectator`](../manual/content/formats/spawn-ini.md#who-is-playing) | `no` | `yes` makes the person watch instead of play. |
| [`[Other1] Ip`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `0.0.0.0` | The address another person's machine answers on, in `[Other1]` to `[Other7]`. A tunnel makes it unused. |
| [`[Settings] Port`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `1234` | The port this machine listens on, or the tunnel number the tunnel knows it by. `[Other1]` to `[Other7]` hold the other machines' values. |
| [`[Settings] BindAddress`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | (none) | The address the listening socket binds. Left out, it binds every address. |
| [`[Tunnel] Ip`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `0.0.0.0` | The address of the tunnel server. |
| [`[Tunnel] Port`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `0` | The port of the tunnel server. A nonzero value plays the match through the tunnel. |
| [`[Settings] ConnTimeout`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `3600` | How long loading may go without progress, in ticks of 16 milliseconds. |
| [`[Settings] ReconnectTimeout`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | `2400` | How long a machine may go quiet during play, in ticks of 16 milliseconds. |
| [`[Settings] TimingPolicy`](../manual/content/formats/spawn-ini.md#a-game-against-other-machines) | (none) | `Adaptive` makes this machine's timing master use the adaptive timing table. |
| [`[Settings] SkipScoreScreen`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | `no` | `yes` ends a skirmish or network match without its score screen. |
| [`[Settings] CustomLoadScreen`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | (none) | The picture shown while the scenario loads. |
| [`[Settings] CustomLoadScreenPos`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | (none) | Places the loading bars at `x,y` within the picture. |
| [`[Settings] DifficultyBasedAINames`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | `no` | Names each computer player `Hard AI`, `Medium AI` or `Easy AI`. |
| [`[Settings] QuickMatch`](../manual/content/formats/spawn-ini.md#what-a-player-is-shown) | `no` | Shows every player as `Player 1` to `Player 8` instead of by name. |
| [`[HouseColors] Multi1`](../manual/content/formats/spawn-ini.md#who-is-playing) | `-1` | The color, `0` to `7`, a computer seat plays. `Multi1` to `Multi8` name the seats. `-1` leaves the choice to the game. |
| [`[HouseCountries] Multi1`](../manual/content/formats/spawn-ini.md#who-is-playing) | `-1` | The country a computer seat plays. `Multi1` to `Multi8` name the seats. |
| [`[HouseHandicaps] Multi1`](../manual/content/formats/spawn-ini.md#who-is-playing) | `-1` | The difficulty slot a computer seat plays at. `Multi1` to `Multi8` name the seats. `-1` follows `AIDifficulty`. |
| [`[SpawnLocations] Multi1`](../manual/content/formats/spawn-ini.md#who-is-playing) | `-1` | The map start position, `0` to `7`, a seat begins at. `Multi1` to `Multi8` name the seats. |
| [`[IsSpectator] Multi1`](../manual/content/formats/spawn-ini.md#who-is-playing) | `no` | `yes` makes the person in a seat watch instead of play. `Multi1` to `Multi8` name the seats. |
| [`[Multi1_Alliances] HouseAllyOne`](../manual/content/formats/spawn-ini.md#who-is-playing) | `-1` | One seat this seat allies with. `HouseAllyOne` to `HouseAllyEight` hold up to eight, and `[Multi1_Alliances]` to `[Multi8_Alliances]` name the seats. |
| [`[GlobalFlags] GlobalFlag0`](../manual/content/formats/spawn-ini.md#a-campaign-mission) | `no` | A scenario flag a campaign mission starts with set. `GlobalFlag0` to `GlobalFlag49`. |

## OPENTS.INI and the launcher

`OPENTS.INI` describes a deployment: where its files are and which mods and files it starts with. Yuri's Revenge has no such file. The [OPENTS.INI page](../manual/content/formats/opents-ini.md) owns the details, and [the launcher guide](LAUNCHER.md) owns the `[Update]` keys.

Example:

```ini
[Paths]
SearchPaths=INI,MIX,Maps
Mods=HighTech

[Update]
Check=yes
```

| Key | Default | Effect |
| --- | --- | --- |
| [`[Paths] SearchPaths`](../manual/content/formats/opents-ini.md) | `INI,MIX,Maps` | Extra folders the game searches for its files, in the order written. A written list replaces the default. |
| [`[Paths] Mods`](../manual/content/formats/opents-ini.md#the-mods-it-starts-with) | (none) | The mods the game starts with, in the order they are read. A player's own `Mods=` in `RA2MD.INI` replaces it. |
| [`[Paths] DataDirectory`](../manual/content/formats/opents-ini.md#the-data-and-user-directories) | (the game's own directory) | The game data directory, in place of `-DATADIR`. |
| [`[Paths] UserDirectory`](../manual/content/formats/opents-ini.md#the-data-and-user-directories) | (none; the game's own directory) | The user data directory, in place of `-USERDIR`. |
| [`[Saves] CarryScenarioFile`](../manual/content/formats/opents-ini.md#what-a-save-carries) | `no` | `yes` stores a copy of the scenario file in every saved game, and a restart reads that copy. |
| [`[Files] Settings`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `RA2MD.INI` | The file a player's options are read from and saved to. |
| [`[Files] Rules`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `RULESMD.INI` | The rules file. |
| [`[Files] RulesExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] Art`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `ARTMD.INI` | The artwork definitions. |
| [`[Files] ArtExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] RA2Art`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `ART.INI` | Red Alert 2's artwork definitions, whose movie list its campaigns' missions index. |
| [`[Files] AI`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `AIMD.INI` | The computer player's data. |
| [`[Files] AIExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] Sound`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `SOUNDMD.INI` | The sound registry. |
| [`[Files] SoundExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] Theme`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `THEMEMD.INI` | The music registry. |
| [`[Files] ThemeExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] Eva`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `EVAMD.INI` | The announcer's lines. |
| [`[Files] Battle`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `BATTLEMD.INI` | The campaign list. |
| [`[Files] BattleExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] LanguageRules`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `LANGRULE.INI` | The translated rules, read over the other rules. |
| [`[Files] LanguageRulesExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] MultiplayerRules`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `MPLAYER.INI` | The rules read only outside a campaign. |
| [`[Files] MultiplayerRulesExpansion`](../manual/content/formats/opents-ini.md#the-files-it-reads) | (none) | The expansion's copy of the matching base file, read after it. |
| [`[Files] Tutorial`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `TUTORIAL.INI` | The numbered text lines. |
| [`[Files] UI`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `UIMD.INI` | The interface settings. |
| [`[Files] Keyboard`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `KEYBOARDMD.INI` | The hotkey file, read at startup and saved by the keyboard dialog. |
| [`[Palettes] Scheme`](../manual/content/formats/opents-ini.md#the-palettes-it-starts-with) | `UNITSNO.PAL` | The palette player colors are built against until a theater loads. |
| [`[Palettes] Game`](../manual/content/formats/opents-ini.md#the-palettes-it-starts-with) | `TEMPERAT.PAL` | The palette the game is drawn with until a theater loads. |
| [`[Archives] ExpandPrefix`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `EXPANDMD` | Comma separated prefixes of the numbered expansion archives. Each prefix mounts the archives numbered 99 down to 00, and an earlier prefix is searched first. |
| [`[Archives] CachePrefix`](../manual/content/formats/opents-ini.md#the-files-it-reads) | `ECACHE` | The same for the cached archives. |
| [`[Menu] FollowMenuButtons`](../manual/content/formats/opents-ini.md) | `yes` | `no` keeps every title menu button, whatever `[UISettings]` in the UI file says. |
| [`[Update] FeedURL`](LAUNCHER.md#settings-in-opentsini) | the GitHub latest release | The release feed. An `https://` address, or an absolute local path for tests. A plain `http://` address is refused. |
| [`[Update] Check`](LAUNCHER.md#settings-in-opentsini) | yes | `no`, `0`, `off` or `false` turns update checks off. The game still starts. |

## Keeping this page current

Run `python manual/tools/openyr_keys.py` after `python manual/tools/manage.py
update` and commit the result. `--check` exits 1 when the page is stale or a
change record that adds a key has no decision in `openyr_keys.yaml`.
