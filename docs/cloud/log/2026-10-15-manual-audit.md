# Manual audit log, 2026-10-15

Branch `cloud/manual-audit-2026-10-15`, begun from `origin/modern` (`4d36e67d`). The job was to check every change record added to `manual/changes/` since `1cb8bf7`, and the key and system pages those commits changed, against the current source.

## Coverage

`git diff --name-only --diff-filter=A 1cb8bf7 origin/modern -- manual/changes` lists 364 records. I read every one. For each I found the commit that added it, read its code diff, and compared the record with the code on `modern` as it stands now, because later commits changed some of the same code. Depth varied:

- **Checked against the current code line by line:** the dock and repair-bay records, mind control and entry refusals, hospitals and armories, disguises and the Mirage tank, OmniCrusher, spy effects, veterancy and the five `Veteran*` bonuses, kill credit, crates, spawners and carriers, prone damage and the damage order, deployers, the DrainWeapon records, the shrapnel records, flak scatter, planning mode, silo and lightning storm records, power and build time, and the survivor and secret lab records.
- **Read and compared with the code diff and a spot check of the current code:** the remaining records. I did not trace every sentence of every record to a line.
- **Pages:** I read the added and changed lines of the pages the commits touched where a record pointed at them, and searched the manual for sibling pages that repeat a fact I found wrong (for example every page that said "plus one" after `VeteranSpeed` and friends became plain multipliers).

A script listed every commit whose added code lines no longer exist on `modern`; those got the closest reading.

## Commits

Each commit changes one page or record.

| Subject | What it fixes |
| --- | --- |
| Correct the repair-every-dock record to the first-dock rule; Drop the unreleased every-dock state from the repair bay record | A repair bay serves only its first dock (`a5891d09` reverted the every-dock work). The two records said the opposite or described an unreleased state. |
| Narrow the dock wake record to leaving guard; Limit the sell-every-dock record to objects near the center; Limit the SellSound depot sale to units near the center | The wake only leaves guard, and a sale takes only objects within half a cell of the center. |
| Drop the unsupported quarter-cell rule from NumberOfDocks; Describe the UnitRepair idle wake and first-dock repair as coded | The pages said a depot never takes on an object whose dock lies more than a quarter cell from the center. The arrival message starts the repair mission whatever the offset, as a naval yard shows. |
| Note that dominated infantry are still refused at hospitals; Fold the hospital mind-control test into one condition; State every infantry test for the armory enter cursor; Correct the hospital Ammo and admission tests on the repair page | `building.cpp` refuses infantry that holds a mind-controlled unit, is mind controlled, or was dominated. A hospital with no `Ammo` is unlimited, which the repair page still denied. |
| Limit the garrison mind-control record to Occupier soldiers | The check sits in the `Occupier` branch only. |
| Say when a Mirage tank counts as disguised in its record; Correct the detection percent default for a side with no value; Remove the duplicate spy retaliation sentence from the disguises page | A side with no `DisabledDisguiseDetectionPercent` entry counts as 0, and a roll of 0 still passes. |
| Name the Crusher requirement in the OmniCrusher record; Name the simple deployer scope in the packed weapons record | Both conditions were missing. |
| Correct the suspended superweapon record: the charge is emptied; Correct the suspended superweapon spy row on the capture page | `SuperClass::Reset` sets the countdown to the full delay and stops it. The pages said "full charge", which is the opposite. |
| Correct where VeteranCombat applies in the damage order; Name the country speed multipliers on the VeteranSpeed page; Fix the VeteranSpeed order and crate scope on the veterancy page; Drop the plus one from VeteranSpeed in the movement chain; Correct the VeteranROF step in the ROF adjustment order; Add the bunker and garrison steps to the ROF adjustment order; Fix the prone and veteran armor steps on the warheads page | Pages still described the bonuses as "plus one" and a house ground-speed multiplier that no longer exists. |
| Limit the veterancy crate money record to games outside a campaign | `CellClass::Goodie_Check` converts a veterancy crate to money only outside a campaign. |
| Add the water crate step and the Veteran conversion to the crates page; Limit WaterCrate to the campaign crates the overlay decides; Limit WaterCrateImg to the campaign crates the overlay decides; Name the map-drawn crate in the water crate record | The campaign overlay list lacked `WaterCrateImg`, and the seven-result conversion list lacked `Veteran`. The overlay decides only a crate with overlay data 0. |
| Correct what a destroyed carrier does to its missiles; Correct the in-flight missile case in the takeoff removal record | `Kill_Nodes` removes docked, reloading and takeoff nodes and sends every other spawn to the kamikaze list. |
| Correct the prone damage record to the rounding the code uses; Correct the ProneDamage rounding | `493b1eef` changed prone damage back to truncation without a manual update. |
| Add the AreaFire and guard conditions to the Deployer page; Add the AreaFire exception to the pack-up record | `Is_Area_Fire_Deployer` and the guard conditions. |
| Say the virus cloud damages objects in its cells; Say the virus cloud particles damage objects in their cell | The particle damages every occupier of its cell, not only infantry. |
| Limit the loss-wins rule in the skirmish win record to campaigns | `Finish_Decided_Game` lets a win beat a loss outside a campaign. |
| Say PlaceBeacon and Delete work in the default keys record | Beacons landed after the record. |
| Match the planning cue record to the once-per-scenario messages | Superseded by the gamemd messages record. |
| Drop the schema version number from the GameMode record; Drop the schema version number from the autosave key record | Schema version is 8 now. |
| Drop the save revision number from seven AI, flak and power plant records and from the prerequisite override record | `REVISION` is 69 now; a record for release 0.2.0 should not name an intermediate number. |
| Limit the silo launch announcement to games without an announcer file | With an announcer file the silo is silent and the fired line plays at fire time. |
| Regenerate the INI key catalog for the building recoil keys | `manual/data/ini-keys.yaml` on `modern` lacked nine keys (`BarrelCompressFrames`, `BarrelHoldFrames`, `BarrelRecoverFrames`, `BarrelTravel`, `TurretCompressFrames`, `TurretHoldFrames`, `TurretRecoil`, `TurretRecoverFrames`, `TurretTravel`), so `manage.py check` reported drift on `modern` itself. The commit is the output of `manage.py update`. |

## Findings left open

1. **Records the check will not let me delete.** `manage.py` refuses to remove a change ID present in the base. `yr-repair-every-dock.md` and `prone-damage-rounds-to-nearest.md` now describe behavior that equals 0.1.0, so neither describes a visible change. I rewrote both to true statements. Decide whether to retire them by another route.
2. **Comment in `code/super.cpp` (`SuperClass::Reset`):** "A suspended weapon instead holds a full charge". The countdown is set to the full recharge time, so the charge is empty. The manual now says so; the comment is wrong.
3. **Comment in `code/building.cpp` (`BuildingClass::How_Many_Survivors`):** "Civilians hold the third place in the stock rules". `systems/capture.md` says Civilian is fourth, after `ThirdSide`, so a civilian house gets no survivors. One of the two is wrong; the stock `[Sides]` list decides.
4. **Two meanings of "ally" for a disguised spy.** `InfantryClass::Is_Disguised_As` fools a house when the copied house is an ally of the spy's own house; `TechnoClass::Looks_Friendly_To` fools it when the copied house is an ally of the viewing house. The retaliation test calls both. The disguises page says "that house's soldier, or an ally's soldier" and does not say whose ally.
5. **`AIRestrictReplaceTime` record and key page:** the code also holds back a gatherer-and-destination structure (a slave miner) until the gatherer count drops; neither text says so.

## Claims I could not check

These cite the stock rules, which are not in the repository. They may be right; I only checked the code half of each sentence.

- `yr-ai-trigger-super-and-civilian-conditions`: "13 triggers of these types".
- `yr-virus-cloud` and `spawnsparticle--animtype.md`: the shipped `VIRUSD` animation sets `SpawnsParticle=VirusCloud1` and `NumParticles=3`.
- `added-radialfiresegments`, `added-closerange`, `added-numberimpassablerows`, `added-culling`, `added-supergapradiusincells`, `added-courselockduration`, `added-yuribarracks`, `added-firewhilemoving`, `harvester-queueing-cell`, `deploy-dir`, `structure-create-unit-sound`, `building-turret-recoil`: which stock types set each key and to what.
- Records that quote a test map ("Checked on SOV04DMD", "Checked on KILLER", "Checked on Deadman's"): runtime observations I cannot repeat.

## What ran

- `git fetch origin`; `git switch -c cloud/manual-audit-2026-10-15 origin/modern`.
- `python3 manual/tools/manage.py update`: passed. It rewrote `ini-keys.yaml` as described above and left `commands.yaml` and `scripting.yaml` unchanged.
- `python3 manual/tools/manage.py check`: passed, apart from the expected "site dependencies are missing" message. On `modern` before my commits it failed with `DRIFTED manual/data/ini-keys.yaml`.
- No build, no unit test, no game run. I could not run the game or check any of this against `gamemd.exe`; every correction follows the code on `modern`, so each one is a proposal until the Windows build confirms it.
- The manual site was not built (`npm ci` was not run), so no page layout was inspected.

## Ideas for the next session

- Repeat the drift search after each merge into `modern`: list the commits whose added code lines no longer exist, and read their records first. It found the prone damage, `VeteranSpeed`, planning and win-or-loss problems in minutes.
- Search the manual for `plus one`, `ground-speed` and `Groundspeed` after any change to the veteran bonuses or to `Current_Speed`; several pages repeated the same fact.
- `manage.py check` on `modern` should be part of the local merge routine; the catalog drift would have shown at once.

## Questions for MentalHomiega

- Which of the two survivor comments is right: is Civilian third or fourth in the stock `[Sides]` list?
- Should `yr-repair-every-dock.md` and `prone-damage-rounds-to-nearest.md` stay as true restatements, or do you want the check relaxed so superseded unreleased records can be dropped?
- Is the spy disguise rule meant to use the spy's allies or the viewer's allies?
