# Priorities for Ares and Phobos support

This ranking is provisional. It rests on the Ares and Phobos documentation and on the partial tag tables in [`ares-tags.md`](ares-tags.md) and [`phobos-tags.md`](phobos-tags.md); the Phobos attached-effect, shield, terrain and AI scripting pages are not catalogued yet. Run [`tools/mod_scan.py`](tools/mod_scan.py) on Mental Omega to replace the guesses below with real usage counts.

## Ranked plan

1. **Per-armor warhead rules (`Versus.<armor>` and its ForceFire, Retaliate and PassiveAcquire variants, Ares).** Mods that define extra armor types need these before their weapons behave sensibly, and the change stays inside warhead and armor lookup.
2. **Small per-type switches that only add a parsed field and one check.** Examples are the Ares veteran and elite ability names that mirror stock immunities (EMPIMMUNE, RADIMMUNE, POISONIMMUNE), and the Phobos weapon and warhead flags marked small in the tables. They are cheap, common in mod files, and raise the scan coverage figure quickly.
3. **Per-type copies of global rules (Phobos technos and buildings).** Keys such as `CurleyShuffle`, `HarvesterDumpRate`, `HarvesterLoadRate`, `OpenTopped.RangeBonus`, `OccupyDamageMultiplier`, `Units.RepairRate` and `Wake` override a global key the engine already reads (checked in `manual/data/ini-keys.yaml`), so each needs one new type field and a fallback to the existing global.
4. **Veterancy, experience and bounty tags (Ares).** Economy and balance tuning in large mods leans on them, and the engine already tracks experience, so the change reuses existing state.
5. **Weapon selection and targeting controls on technos (Phobos `ForceWeapon.*`, `NoSecondaryWeaponFallback`, `MultiWeapon`, targeting delays).** Mods use them to make multi-weapon units pick sensible weapons, and they change only the weapon choice and scan timing in `code/techno.cpp`.
6. **Attached effects (Ares `AttachEffect.*`, Phobos attached-effect types).** Many unit abilities in big mods are built from them. The work is medium to large because effects need per-techno state in saves, so plan it after the cheaper items.
7. **Automatic death, type conversion and anim-to-unit (Phobos `AutoDeath.*`, `Convert.*`, `CreateUnit`).** Temporary units, deploy-to-transform units and wreck-to-unit effects depend on them; type conversion is shared with Ares and needs careful save handling, so it is medium to large.
8. **Projectile trajectories and interception (Phobos).** They unlock whole weapon families but add a new bullet-motion subsystem, so they rank below the tags that only read data.
9. **Superweapon extensions (Phobos and Ares).** Mental Omega depends on many custom superweapons, so these unlock mods rather than single units; sizes range from small to large and need triage once the scan shows which ones the mod uses.
10. **Building behaviour (Phobos grinder, factory plant, power plant enhancer, upgrade and placement tags).** Each is small to medium and self-contained in `code/building.cpp` and `code/builtype.cpp`, but fewer mods depend on any single one than on the techno items above.
11. **AI scripts, trigger events and trigger actions (Ares).** Campaign and skirmish content rely on them, and they extend the numbered script system already present in `code/_script.cpp`.
12. **Spawner, hijacker, driver and harvester behaviour (Ares and Phobos).** Each is a distinct feature on specific unit types and can follow demand from the scan.
13. **Animation drawing and damage tags (Phobos draw offsets, visibility, fire animations, animation damage).** Most are small art-side reads; they affect looks more than play, except the animation damage settings, which change balance and should move up if the scan shows them in use.
14. **Interface tags (Ares and Phobos sidebar, tooltips, digital displays, select boxes, hotkeys).** They affect presentation only, so they come after behaviour that changes gameplay; `[Phobos]` keys in RA2MD.INI are player settings and need a settings page rather than rules parsing.

## Not yet ranked

The Phobos attached-effect, shield, laser-trail and radiation types, the terrain, ore, overlay, particle, country and voxel-animation pages, the AI scripting and mapping page, and the miscellaneous page (insignia types and debugging aids). The Ares bugfix pages and `whatsnew` are also not tabled. The next session adds them to the tables first.
