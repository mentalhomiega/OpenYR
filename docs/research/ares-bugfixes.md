# Ares behaviour changes and renamed tags

Behaviour changes that the public [Ares documentation](https://ares-developers.github.io/Ares-docs/) lists for its bugfix and migration pages. Each item changes how the game plays and is not a crash fix. The list is unverified until compared with the real game, and nothing here is implemented in this engine unless the manual says so. Page names are the Ares page under `bugfixes/`.

## Renamed and removed tags

`whatsnew` records no renames or removals. The `migration` page records the following; the third column names the Ares version the rename came with.

| Old | New or status | Migration section |
|---|---|---|
| `[SuperWeapon]SW.Deliver` | renamed `Deliver.Types` | From Ares 0.1 |
| `[SuperWeapon]SW.DeliverBuildups` | renamed `Deliver.Buildups`; later removed because buildups are always on | From Ares 0.1; 0.C |
| `[SuperWeapon]SonarPulse.Range` | renamed `SW.Range` | From Ares 0.1 |
| `[SuperWeapon]GenericWarhead.Warhead` | renamed `SW.Warhead` | From Ares 0.1 |
| `[SuperWeapon]GenericWarhead.Damage` | renamed `SW.Damage` | From Ares 0.1 |
| `[SuperWeapon]Nuke.Sound` | renamed `SW.ActivationSound` | From Ares 0.1 |
| `Surface.*.Memory`, `Surface.*.Force3D` (ares.ini) | obsolete, no effect | From Ares 0.1 |
| `[Projectile]SubjectToFirewall` | removed; use `IgnoresFirestorm` | From Ares 0.B |
| `[TechnoType]Spotlight.AttachedTo=barrel` | value no longer supported; use `turret` | From Ares 0.B |
| `[General]EngineerDamageCursor`, `TogglePowerCursor`, `TogglePowerNoCursor` | removed; define the EngineerDamage, TogglePower and NoTogglePower mouse cursors instead | From Ares 0.C |
| `[SuperWeapon]Cursor`, `NoCursor` | now accept only mouse cursor names | From Ares 0.C |
| `[BuildingType]SpyEffect.UnitVeterancy` | replaced by `SpyEffect.InfantryVeterancy` (`Factory=InfantryType`) and `SpyEffect.VehicleVeterancy` (`Factory=UnitType`) | From Ares 1.0 |
| `[BuildingType]FactoryOwners.HaveAllPlans` | renamed `FactoryOwners.Permanent` | From Ares 1.0 |
| `[Warhead]Ripple.Radius` | renamed `IonCannon.Ripple` | From Ares 1.0 |
| `[BuildingType]PrismForwarding.MyHeight` | removed; it had no effect | From Ares 2.0 |
| `[BuildingType]SpyEffect.StolenMoneyAmount` | meaning changed when used with `SpyEffect.StolenMoneyPercentage`, which takes precedence | From Ares 2.0 |
| `[BuildingType]Rubble.Destroyed` | hardcoded building properties removed; set `TogglePower=no` and `Unsellable=yes` for the old behaviour | From Ares 2.0 |
| `AllowHiResModes` and the HIRES cheat | obsolete, resolutions always enabled | ui-features/newgameresolutions |

The existing table row `FactoryOwners.HasAllPlans` uses the old spelling; the current docs name the key `FactoryOwners.Permanent` (the existing table also has a `FactoryOwners.Permanent` row).

`FactoryOwners.HaveAllPlans` was renamed `FactoryOwners.Permanent`; [`ares-tags.md`](ares-tags.md) uses the new name.

## Game-rule behaviour changes

- aircraftdock: players can again choose which structure in the AircraftType's `Dock` list an aircraft lands on; the cursor shows enter or no-enter over such structures.
- aitargetingcloakedobjectswithmajorsuperweapons: the AI skips cloaked targets for LightningStorm, MultiMissile, PsychicDominator and GeneticConverter super weapons.
- battlebunkerredhealth: infantry may enter a Battle Bunker at red health.
- behindanimopentopped: `Behind` anims are no longer shown for OpenTopped passengers.
- blockedrefinerywarfactory: enemy units can no longer enter the passable cells (bib) of refineries and war factories.
- burstabuse: the burst counter is no longer reset when a target dies or changes, so no free fresh burst; reload is not restarted.
- chaosgasandstopcommand: Stop no longer works on units under Psychedelic warheads.
- chronoshiftwillsinkjumpjetunits: `SpeedType=Hover` jumpjets sink when chronoshifted into water.
- amphibiousobjectssinkwhenchronoshiftedontowater: amphibious (not hover) objects no longer sink when chronoshifted onto water.
- distanceoverflow: distances are capped, so far docks are equally bad rather than preferred.
- enemyharvesterguardmodeexploit: guard mode cannot be ordered on enemy harvesters.
- firingvoicesandveterancy: elite units without an elite voice list use the non-elite weapon voice, not `VoiceAttack`.
- forceshieldexploit: the AI uses Force Shield only against super weapons that actually fired.
- guardapproachtarget: units on Guard (not Area Guard) no longer leave their post to chase aircraft.
- ivanbombsdisappearing: Ivan Bombs move with a deploying or undeploying unit and an owner change, and detonate when the host is sold.
- ivanentergrinder: Crazy Ivan gets the enter cursor on allied Grinders and the bomb cursor on enemy ones (force fire plants a bomb).
- mindcontrolbuildings: freed mind-controlled buildings go on guard instead of idling.
- misleadingveterannavalcameos: only vehicles that will really start veteran get the veteran cameo after a spy infiltrates a war factory.
- multiplaypassiveanger: `MultiplayPassive=yes` houses cannot become an AI's favourite enemy.
- occupierpromotion: occupiers are promoted at once, silently, without leaving the building.
- parasitesexperience: Naval parasites gain no experience from allied kills.
- parasitesinairborneunits: a parasite in a destroyed airborne unit falls and is destroyed on impact.
- prismsupportbugs: a map's `[General]` no longer multiplies `PrismSupportModifier` by 100; incapacitated or building/selling towers no longer support; UMP maps stay at 150%.
- savegamecrates: crates collected before a save stay gone after loading.
- secretlabboonweighting and toomanysecretlabs: each Secret Lab picks its boon uniformly at random, independent of other labs, even with more labs than boons.
- sellingbuildingsexploits: a sell order cannot become a move order, crew is not ejected twice, and the MCV is placed or removed.
- shipyardrepairimmune: units under repair at a shipyard take damage.
- spyingalliedbuildings: spies get no effects from own or allied buildings.
- superweaponoption: the multiplayer no-super-weapons option no longer affects campaigns.
- techstructuresneedsengineer: `NeedsEngineer=yes` tech buildings stay animation-idle when damaged until captured.
- temporalwarheadsearnexperiencebykillingfriendlyunits: temporal warheads give no experience for erasing friendly units.
- temporalwarheadsgaindoubleexperience: temporal kills give experience once, not twice.
- unitselldock: unit selling depends on the unit's own dock position; the Soviet Repair Depot sells units by default.
- loadscreencolors: load screen text colour depends on the faction (Allied blue, Soviet red, Yuri `YuriLoad`).
- warheadversesspecialvalues: the 0%, 1% and 2% `Verses` special values apply consistently.
- ambientdamage: `AmbientDamage=0` deals no damage and is no attack.
- airtoaircombat: aircraft can use weapons whose projectile has `AA=yes`, with no extra flag.
- animationdamagewarheads: an animation uses its own `Warhead`, defaulting to `FlameDamage2` (`C4Warhead` for `INVISO`).
- baseunit: all `[General]BaseUnit` entries are MCVs, not just three.
- buildablesecretlabs: a Secret Lab built in play now has a boon the builder could not already build.
- buildconst: low-power EVA triggers for any building on the `[AI]BuildConst` list.
- buildingtypesandinfantrytypesdonotreloadammoproperly: buildings and infantry reload ammo; `Hospital` and `Armory` buildings still do not.
- buildingtypeupgradesarenotviableprerequisites: building upgrades satisfy prerequisites and super weapon `AuxBuilding`.
- buildlimitqueues: the last unit of a `BuildLimit` above 1 can be queued.
- c4veteranability: the veteran C4 ability works again (infantry blow up buildings).
- carryallcargothings: Carryall cargo regains its rotation speed and is dropped facing the Carryall's direction; the Carryall then guards and rejoins its team.
- chronotransportdeploy: teleporting transports cannot deploy while phasing in.
- cloakableaircrafttypesandbuildingtypes and cloakablespawners: aircraft, buildings and spawners can cloak; spawns survive a cloak.
- cloakdamage: healing or zero-damage hits no longer uncloak an object.
- cloakwrench: the repair wrench is hidden from non-observers while a building is cloaked.
- destroyablecliffs: any weapon collapses destroyable cliffs, subject to `CollapseChance`.
- ejectlastpassenger: transports with `TurretCount` above 0 eject every passenger; `Gunner=yes` vehicles keep the driver.
- factoryloadsharing: factory exit sharing covers all BuildingTypes with the same `Factory` and `Naval`; the kennel hack stops working.
- freeunit: non-harvester free units are set to Area Guard.
- hardcodedwallgateinteractions: gates can be slammed onto any `Wall=yes` overlay.
- hijackersarereimbursedwhenaunitisgrinded: grinding a hijacked vehicle also refunds the hijacker.
- infantrycloaking: cloakable infantry decloak only when needed during an attack order.
- infantrylostinspecialfunctionbuildings: infantry inside a destroyed, sold or undeployed Armory or Hospital are ejected.
- initialveterancameos: cameos of units that always start veteran get the veteran symbol.
- ivanbombscanonlybefiredbyinfantrytypes: any techno type can deliver Ivan Bombs.
- ivanbombscloak: a cloaked Ivan is immune to his own bomb.
- mindcontrolslaves: slaves change owner when their enslaver is captured or mind-controlled.
- movingalphalights: `AlphaImage` lights follow the object and support facings.
- multipleaifactoriescloneunits: AI teams respect `BuildLimit` and extra factories no longer clone units unless the new `[GlobalControls]` flags say so.
- newconstructionoptions: fewer false "new construction options" announcements.
- occupiersalwaystrained: occupiers with `Trainable=no` gain no experience.
- opentoppedjambuildings: OpenTopped vehicles can no longer jam defensive structures.
- producecash: `ProduceCashDelay` uses the full delay, resets on expiry and runs when `ProduceCashStartup` is 0.
- radbeamsandwavesusingthewrongflh: beams and waves use the FLH of the weapon that fired.
- sonicwall: sonic waves destroy walls only with `Wall=yes` warheads.
- specialweaponsnowfunctioninsecondaryslots: mind control, parasite and temporal weapons work in the secondary slot (several of one kind on one object are not recommended).
- spyplanecountdecoupledfromallyparadropnum: spy plane count is `SpyPlane.Count`, default 1.
- temporalwarheadsstillaffectobjectsthatarenotwarpable: `Warpable=no` targets no longer lose mind-controlled units or spawns.
- unitinstancesnotcountingtowardsbuildlimit: deploying vehicles, hijackers inside vehicles and AirportBound aircraft count toward `BuildLimit`.
- unitsoverpoweringbuildings: vehicles can overpower buildings.
- vehicleparadropoffset: paradropped vehicles land at the right position.
- waves: magnetron beams disconnect out of range, sonic waves reach full weapon range, laser waves draw at all detail levels.
- remappablewalls: walls draw in the owner's colour, not the local player's.
- whiteboybug: no limit of 74 cameos per sidebar tab.
- radiationcolorsandsnow: radiation colour is no longer hardcoded for the snow theater.
- magnetron move sound (unitsoundsplayedatinappropriatetimes in type1): units pulled by a Magnetron play `MoveSound` only when moving under their own power.
- miragelogicwithturretsbarrels: turrets and barrels are disguised with the body.
- whatsnew 3.0: Short Games end only when a player has no units or structures that keep them alive (listed on whatsnew only).
