# Save audit log, 2026-10-15

Branch `cloud/save-audit-2026-10-15`, begun from `origin/modern` (`4d36e67d`). The save revision was 69 at the start and is 70 at the end.

The range `1cb8bf7..origin/modern` has 853 commits and changes 122 headers. I listed every data member the header diff adds, removes or retypes (static members too), then followed each one through the constructor, the class's `Serialize` (one function reads and writes, so order cannot differ), the class's `Compute_CRC` where it has one, and, for artwork pointers, `Post_Load`. A load builds each object with its default constructor, so the constructor check is the check for the load path too.

## Commits

| Commit | What it does |
| --- | --- |
| Add the base node placement failure count to the base checksum | `BaseNodeClass::Attempts` joins `BaseClass::Compute_CRC`. |
| Save YuriBarracks and checksum the added building type rules | `BuildingTypeClass::Serialize` writes `IsYuriBarracks`; its checksum gains `ExtraPowerDrain`, `NumberImpassableRows`, `IsYuriBarracks` and the three secret lab items. |
| Save MobileFire and the seat offsets, and checksum the added unit rules | `TechnoTypeClass::Serialize` writes `IsMobileFire` and `AlternativeFLH`; the checksum gains the unit rules added since `1cb8bf7`. |
| Fetch the AltCameo artwork again after a load | `TechnoTypeClass::Post_Load` re-reads `AltCameo`, which the load never restored. |
| Add the assault and dig-in rules to the infantry type checksum | `IsAssaulter`, `UndeployDelay`, `IsDeployedCrushable`. |
| Add Culling to the warhead type checksum | `IsCulling`. |
| Save FireWhileMoving and checksum it with the assault animation | `WeaponTypeClass::Serialize` writes `IsFireWhileMoving`; the checksum gains it and `AssaultAnim`. |
| Save the added rules keys that a loaded game read back as defaults | `RulesClass::Serialize` writes 14 members listed below. |
| Save TeamsPresent and checksum the dominator and nuke lighting | `ScenarioClass::Serialize` writes `TeamsPresent`; the checksum gains it and nine lighting values. |
| Save the house a particle credits its kills to | `ParticleClass::Serialize` writes `SourceHouse`. |
| Add the threat, charge delay and control houses to the techno checksum | `TechnoClass::Compute_CRC`. |
| Add TargetNotAssigned to the team checksum | `TeamClass::Compute_CRC`. |
| Add the landing and mirage blink state to the unit checksum | `UnitClass::Compute_CRC`. |
| Add the list position to the side checksum | `SideClass::Compute_CRC`. |
| Add the country, gatherer counts and base cells to the house checksum | `HouseClass::Compute_CRC`. |
| Raise the save revision to 70 for the audited save fields | One bump for the branch. Six classes changed their save layout. |
| Record the rules and state a load now keeps | `manual/changes/load-keeps-rules-and-flags.md`, plus one `AltCameo` entry in `manual/data/ini-read-exclusions.yaml` (the extractor demanded a classification for the new read in `Post_Load`, as it has for `Cameo` and `CameoPCX`). |

## Verdicts

"Ok" means the constructor sets the member, `Serialize` carries it and, where the class has `Compute_CRC`, the checksum covers it or the member cannot affect play. "Fixed" names the commit above. "By design" means a header comment says the member is not saved and I judged the effect visual or interface only.

### Fixed

| Class | Member | Gap | Verdict |
| --- | --- | --- | --- |
| BaseNodeClass | Attempts | not in checksum; decides when a computer house drops a node in skirmish | Fixed |
| BuildingTypeClass | IsYuriBarracks | not saved, not in checksum; a loaded game lost the extra exit cell | Fixed |
| BuildingTypeClass | ExtraPowerDrain, NumberImpassableRows | not in checksum | Fixed |
| BuildingTypeClass | SecretInfantry, SecretUnit, SecretBuilding | not in checksum | Fixed |
| TechnoTypeClass | IsMobileFire | not saved; a loaded game fired on the move again | Fixed |
| TechnoTypeClass | AlternativeFLH | not saved; a loaded game used zero seat offsets | Fixed |
| TechnoTypeClass | AltCameoData | artwork pointer never re-fetched after a load | Fixed |
| TechnoTypeClass | IsJumpJet, VHPScan, SuperGapRadiusInCells, IsCloseRange, IsCanRecalcApproachTarget, JumpjetCrash, IsChargeTurret, RadialFireSegments, IsOmniCrusher, IsOmniCrushResistant | not in checksum | Fixed |
| InfantryTypeClass | IsAssaulter, UndeployDelay, IsDeployedCrushable | not in checksum | Fixed |
| WarheadTypeClass | IsCulling | not in checksum | Fixed |
| WeaponTypeClass | IsFireWhileMoving | not saved, not in checksum | Fixed |
| WeaponTypeClass | AssaultAnim | not in checksum | Fixed |
| RulesClass | AlliesAllowed, WaterCrate, LetsDoTheTimeWarpOutAgain, LetsDoTheTimeWarpInAgain, VoxelSpecularStrength, VoxelShadeScale, VoxelSpecularFromView | not saved; a loaded game read the constructor defaults | Fixed |
| RulesClass | IsMPShortGame, IsMPBuildOffAlly, IsMPFogOfWar, IsMPMCVRedeploys, IsMPMultiEngineer, IsMPSuperWeapons, MPGameSpeed | not saved; the skirmish dialog reads them as defaults, and the older `IsMP*` members beside them are saved | Fixed |
| ScenarioClass | TeamsPresent | not saved, not in checksum; the header said so; decides whether computer houses change alliances | Fixed |
| ScenarioClass | DominatorAmbientLight, DominatorRedTint, DominatorGreenTint, DominatorBlueTint, DominatorGroundLight, DominatorLevelLight, DominatorAmbientChangeRate, NukeAmbientChangeRate | saved; not in checksum, unlike the `Ion*` siblings | Fixed |
| ParticleClass | SourceHouse | not saved; a loaded gas cloud credited no house | Fixed |
| TechnoClass | ThreatPosed, ChargeTurretDelay, OriginallyOwnedByHouse, MindControlledByHouse, LetGoBy, LetGoByHouse | saved; not in checksum | Fixed |
| TeamClass | IsTargetNotAssigned | saved; not in checksum | Fixed |
| UnitClass | IsLandingToDeploy, MirageBlinkUntil | saved; not in checksum | Fixed |
| SideClass | ListPosition | saved; not in checksum | Fixed |
| HouseClass | OwnCountry, CountResourceGatherers, CountResourceDestinations, PreferredDefensiveCell, PreferredDefensiveCell2, PreferredDefensiveCellStartTime, BaseSpawnCell | saved; not in checksum | Fixed |

### Ok

| Class | Members |
| --- | --- |
| AnimTypeClass | SpawnsParticle, NumParticles |
| BaseNodeClass | Placed |
| BuildingClass | IsC4Applied, GapRadius, IsGapSuperCharged, HasExtraPowerDrain, HasEngineer, SecretProduct |
| BuildingTypeClass | IsPrimaryFireDualOffset, QueueingCell, IsEligibileForAllyBuilding, ProtectWithWall, WantsExtraSpace, IsTurretRecoil, TurretRecoilControl, BarrelRecoilControl, IsSecretLab; CreateUnitSound is audio only |
| BulletClass | NextAnim, SpawnNextAnim, CourseLockCounter (saved; no checksum, see open findings) |
| BulletTypeClass | FlakScatter, CourseLockDuration |
| HouseClass | SecretLabs (a list of buildings, which carry their own checksum; `ConYards` is treated the same), DefenseFactory, ShipFactory (the four older factories are not in the checksum either), Base_Edge, StartingObjects including the new `Kind`, the bias doubles (comment change only) |
| HouseTypeClass | the 13 country multipliers, VeteranInfantry, VeteranUnits, VeteranAircraft |
| InfantryClass | ShouldDeploy |
| KamikazeClass | Nodes, NextUpdateFrame (saved in `Serialize_Misc_Values`, cleared when a scenario starts; no checksum exists for it) |
| MissionClass | MissionStartFrame |
| NukeFlashClass | Status, StartTime, Duration |
| ObjectClass | EstimatedHealth |
| ParasiteClass | GrappleAnim, GrappleState, GrappleAnimFrame, GrappleAnimDelay (saved; the class has no checksum) |
| RulesClass | the other members added since `1cb8bf7` (WarpIn, the AI and sound keys, the Secret lists, the Chrono keys and so on) |
| SessionClass | AIOnly (saved in `Serialize_Misc_Values`), GameOptionsType::SWAllowed |
| SuperWeaponTypeClass | AIDefendAgainst, IsDisableableFromShell |
| TeleportLocomotionClass | WarpPhase, WarpEnd (saved; the class has no checksum) |
| TechnoClass | TargetingTimer, RadialFireIndex |
| UnitTypeClass | WreckReleasesPassengers |
| WarheadTypeClass | IsSonic, IsDirectRocker, IsCLDisableRed, IsCLDisableGreen, IsCLDisableBlue |
| WeaponTypeClass | IsTerrainFire, DisguiseFakeBlinkTime |

### By design, or not part of a save

| Member | Why it is left alone |
| --- | --- |
| BuildingClass TurretRecoil, BarrelRecoil | Read only by the voxel draw. The header says a save mid-recoil loads with the part at rest. |
| UnitClass MirageCreationFrame | Read only by `Mirage_Cycle` and the draw. The header says the cycle restarts after a load. |
| AnimTypeClass IsArtMissing, ArtMissingTheater | A cache of a failed art lookup; a load retries once. |
| TechnoTypeClass IsClearAllWeapons | Applied while the rules are read; nothing reads it later. |
| IonStormClass Applied_Light | Rebuilt by `Restore_Special_Lighting` after a load. |
| DisplayClass IsPlanningMode, PlanningHoldKey, IsPlaceBeaconMode | Interface state; `Load_Game` calls `Reset_Planning_Mode`. |
| SpotLightClass DisableFlags | The class is transient and never saved. |
| SessionClass NetworkTimingAdaptive, NetworkThroughTunnel, NetworkPeerLag | Network pacing set at launch, not game state. |
| BeaconClass, BeaconManagerClass | The header says beacons are not part of the simulation or of saves. See the question below. |
| HUD rectangles, UI screen state, parsers, spawner and CnCNet configuration | Not game objects; no save path. |
| Static `HouseClass::TraceBuilding`, `IsometricTileTypeClass::WaterBridge`, `TabClass::IsCommandIcons` | Autotest switch, theater tile lookup and interface mode. |

## Open findings

1. `FootClass::PlannedOrders`, `PlannedLeg` and `IsPlanRunning` (`code/foot.h:168`, `:175`, `:410`) are neither saved nor in `FootClass::Compute_CRC` (`code/foot.cpp`, `Serialize` at `:4075`). They are simulation state: `PLAN_ORDER` and `PLAN_COMMIT` run on every machine (`code/event.cpp:1317`, `:1354`) and decide which mission a unit gets when it goes idle. Failing case: save while a unit has two queued orders left, then load; the unit drops the rest of its plan. A multiplayer save is loaded on every machine, so the machines agree with each other but not with the game that was saved. The header and `Load_Game` (`code/saveload.cpp:1329`) state this on purpose, so I did not change it. Suggested fix: give `PlannedOrderClass` a `Serialize` template that writes `Mission` and the two targets through `xTargetClass::Encode`/`Decode` (an invalid target encodes as -1, so decode that case with `Invalidate`), serialize the three members in `FootClass::Serialize`, add them to the checksum, and update the comment at `code/saveload.cpp:1328`, which says plans are not saved. Raise the revision again. I could not check whether gamemd saves its planning nodes.
2. `BulletClass`, `ParasiteClass`, `TeleportLocomotionClass` and `KamikazeClass` have no checksum, so `NextAnim`, `SpawnNextAnim`, `CourseLockCounter`, the `Grapple*` members, `WarpPhase`, `WarpEnd` and the kamikaze list cannot show a desync. The gap covers the whole class, not just the new members. Suggested fix: add a `Compute_CRC` to each class and call it from the object loop; the bullet and locomotor checksums would first have to be added to the sync test on the Windows machine.
3. `SidebarClass::CompletedBuilding` (`code/sidebar.h:495`) is not saved. It is set only when a factory reports completion (`code/sidebar.cpp:2154`), so after a load the structure and defense tab hotkeys may not place a building that finished before the save. I did not trace whether the first factory update after a load raises `Has_Changed`. Suggested fix: in the post-load sidebar rebuild, call `Note_Completed_Building` for each factory that `Has_Completed`.
4. Beacons (`code/beacon.h`) vanish when a multiplayer save is loaded, because the manual lets the master load a `.NET` save mid-match. Suggested fix: serialize the `Slots` table in `Serialize_Misc_Values` and raise the revision.
5. `manage.py update` on unmodified `origin/modern` regenerates 153 lines of `manual/data/ini-keys.yaml` (the building turret recoil keys), and `manage.py check` fails on the tracked catalog until it does. I discarded that churn, so this branch does not carry it. Whoever merges can run `update` once.

## Ran and not run

Ran:

- `git fetch origin`, then the header diff and `grep`/`python3` scripts over the `.cpp` files to list members and match constructors, `Serialize` and `Compute_CRC` bodies. The scripts are in the session scratchpad and are not committed.
- `python3 manual/tools/manage.py update`, then `check`: every validation step passes; the only message is "site dependencies are missing". `ini-keys.yaml` churn was discarded after each run.
- A read of the final `git diff origin/modern -- code` for type and name errors.

Not run:

- Any engine build. The machine has no Windows SDK, so none of the code changes has been compiled. The additions are single `crc(...)`/`stream.Serialize(...)` calls on members whose types match existing calls, plus one `Get_String` and `Retrieve` pair copied from the `Cameo` lines of the same function.
- Unit tests (`tests/save`, `tests/saverevision`): they build on Windows only here, and nothing they cover changed.
- The game: no save was written or loaded, and no sync check was run. Everything above about loaded behaviour comes from reading the code.

## Ideas for the next session

- Add a test that builds a class list from the header diff and fails when a member is in a header but absent from `Serialize`. Today the check is a manual read.
- Move the artwork re-fetch of `TechnoTypeClass::Post_Load` into one function that `Read_INI` also calls, so a new art key cannot be added to one and forgotten in the other.

## Questions for MentalHomiega

- Do you want the plan queue (finding 1) saved? It needs the event and checksum change on the Windows machine and one more revision.
- Should multiplayer saves keep beacons (finding 4)?
- Should I have left the documented "not saved" members (`ParticleClass::SourceHouse`, `ScenarioClass::TeamsPresent`) alone? I saved both because each changes what a loaded game does; the fix is two lines each to revert.
