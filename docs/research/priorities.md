# Priorities for Ares and Phobos support

This ranking is provisional. It rests on the Ares and Phobos documentation and on the partial tag tables in [`ares-tags.md`](ares-tags.md) and [`phobos-tags.md`](phobos-tags.md); the Technos, animations, interface and AI parts of Phobos are not catalogued yet. Run [`tools/mod_scan.py`](tools/mod_scan.py) on Mental Omega to replace the guesses below with real usage counts.

## Ranked plan

1. **Per-armor warhead rules (`Versus.<armor>` and its ForceFire, Retaliate and PassiveAcquire variants, Ares).** Mods that define extra armor types need these before their weapons behave sensibly, and the change stays inside warhead and armor lookup.
2. **Small per-type switches that only add a parsed field and one check.** Examples are the Ares veteran and elite ability names that mirror stock immunities (EMPIMMUNE, RADIMMUNE, POISONIMMUNE), and the Phobos weapon and warhead flags marked small in the tables. They are cheap, common in mod files, and raise the scan coverage figure quickly.
3. **Veterancy, experience and bounty tags (Ares).** Economy and balance tuning in large mods leans on them, and the engine already tracks experience, so the change reuses existing state.
4. **Attached effects (Ares `AttachEffect.*`, Phobos attached-effect types).** Many unit abilities in big mods are built from them. The work is medium to large because effects need per-techno state in saves, so plan it after the cheaper items.
5. **Projectile trajectories and interception (Phobos).** They unlock whole weapon families but add a new bullet-motion subsystem, so they rank below the tags that only read data.
6. **Superweapon extensions (Phobos and Ares).** Mental Omega depends on many custom superweapons, so these unlock mods rather than single units; sizes range from small to large and need triage once the scan shows which ones the mod uses.
7. **AI scripts, trigger events and trigger actions (Ares).** Campaign and skirmish content rely on them, and they extend the numbered script system already present in `code/_script.cpp`.
8. **Spawner, hijacker, driver and harvester behaviour (Ares).** Each is a distinct feature on specific unit types and can follow demand from the scan.
9. **Interface and cursor tags, new sidebar and tooltip options (both).** They affect presentation only, so they come after behaviour that changes gameplay.

## Not yet ranked

Technos, buildings, infantry, aircraft, animations, particles, terrain, audio and hotkey tags from Phobos, and the Ares pages for `Prerequisite` lists and type conversion beyond the rows already tabled. The next session adds them to the tables first.
