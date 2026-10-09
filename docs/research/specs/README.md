# Ares and Phobos feature specs

Each file describes one feature group: the keys, their sections, value types and defaults, the exact behaviour, what stock Yuri's Revenge does without it, and where it would hook into the OpenYR engine in `code/`.

Rules for these files:

- Ares is closed source. Everything marked Ares comes from the public Ares documentation and ModEnc. Anything the documentation leaves open is listed under "Open questions" and needs a comparison test with the real Ares before it is treated as settled.
- Phobos behaviour comes from its documentation. The text here is written from that behaviour; no Phobos code is copied.
- Engine names (classes, functions, files) were checked against `code/` when these specs were written. Line numbers are not given because they drift.

| File | Keys |
|---|---|
| `armor-types.md` | `[ArmorTypes]`, `Versus.<armor>` and its `.ForceFire`, `.Retaliate`, `.PassiveAcquire` variants, `EffectsRequireVerses` |
| `bounty.md` | `Bounty`, `Bounty.Value`, `Bounty.RookieValue/VeteranValue/EliteValue`, `Bounty.Display`, `BountyEnablers`, `BountyDisplay`, `GivesBounty` |
| `chronoshift.md` | `Chronoshift.Allow`, `Chronoshift.Crushable`, `Chronoshift.IsVehicle`, `ChronoInfantryCrush` |
| `custom-palettes-and-pcx-cameos.md` | `CustomPalette`, `CameoPCX`, `AltCameoPCX`, `SidebarPCX` |
| `insignia.md` | `Insignia.*`, `InsigniaFrame*`, `Insignia.ShowEnemy`, `EnemyInsignia` |
| `attach-effect.md` | Ares `AttachEffect.*` |
| `emp.md` | `EMP.Threshold` and the rest of the Ares EMP model |
| `vehicle-theft-and-abduction.md` | `VehicleThief.*`, `Abductor*`, `ImmuneToAbduction` |
| `kill-driver.md` | `KillDriver*`, `ProtectedDriver`, `ProtectedDriver.MinHealth` |
| `reverse-engineering.md` | `CanBeReversed`, `ReversedAs`, `ReverseEngineersVictims` |
| `build-time.md` | `BuildTime.MultipleFactory` and the other `BuildTime.*` keys |
| `groupas-and-keepalive.md` | `GroupAs`, `TypeSelectUseDeploy`, `KeepAlive` |
| `crashable-and-promotion.md` | `Crashable`, `CrashSpin`, `Promote.VeteranSound/EliteSound` and the other promotion keys |
| `airburst-scatter-zadjust.md` | `AirburstSpread`, `AroundTarget`, `BallisticScatter.Min/Max`, projectile `ZAdjust`, `JumpjetCrash` |
| `electric-bolt-colours.md` | `Bolt.Color1-3`, `Beam.Color`, `Beam.Amplitude` |
| `targeting-flags.md` | `AttackFriendlies`, `AttackCursorOnFriendlies`, `DefaultToGuardArea` |
| `ui-and-spy-small-flags.md` | `HealthBar.Hide`, `SpyEffect.Custom` and its superweapon keys |
| `phobos-per-type-rules.md` | Phobos per-type copies of global rules (`CurleyShuffle`, harvester rates, bunker and occupant multipliers, repair rates, wakes, `OpenTopped.*` and others) |
| `phobos-weapon-selection.md` | Phobos `ForceWeapon.*`, `NoSecondaryWeaponFallback`, `MultiWeapon.*`, `NoAmmoWeapons`, berzerk targeting and the target-scan switches |
| `phobos-attached-effects.md` | Phobos `[AttachEffectTypes]` and the `AttachEffect.*` keys on technos and warheads |
| `phobos-shields.md` | Phobos `[ShieldTypes]`, `Shield` and the `Shield.*` keys on technos and warheads |
| `phobos-autodeath-convert-animunit.md` | Phobos `AutoDeath.*`, `Convert.*` (ammo, health, owner change, deploy, superweapon) and `CreateUnit.*` on animations |
