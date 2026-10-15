# Projectile trajectories and interception (Phobos)

Sources: `docs/New-or-Enhanced-Logics.md` sections "Projectile trajectories" (with "Straight trajectory", "Bombard trajectory", "Parabola trajectory"), "Projectile interception logic", "Parabombs", "Return weapon", "Projectiles blocked by land or water" and "Attack technos underground"; code in `src/Ext/Bullet/Trajectories/*`, `src/Ext/Bullet/{Body.cpp,Hooks.cpp,Hooks.Obstacles.cpp,Hooks.DetonateLogics.cpp}`, `src/Ext/BulletType/Body.cpp`, `src/New/Type/Affiliated/InterceptorTypeClass.cpp`, `src/Ext/Techno/{Body.Update.cpp,Hooks.Firing.cpp,Hooks.cpp}` and `src/Ext/WarheadType/Detonate.cpp`.

This Phobos checkout (develop) defines three selectable trajectories: `Straight`, `Bombard` and `Parabola`. The type enum reads `Straight=0, Bombard=1, Parabola=4` and the INI parser accepts only those three names. `SampleTrajectory.*` is a template that nothing selects. The docs mention `Trajectory=Disperse` as a parent of the other three; no such type exists in the source, so the `UseDisperseBurst` keys have no partner. Engrave, Spiral, Meteor, Tracing and Missile trajectories are not in this tree. Homing missiles remain the stock `ROT` logic.

## Keys

All keys below sit in the projectile's own `rules.ini` section (the BulletType section) unless a table says otherwise. Distances written "cells" are read as floating point cells and stored as leptons (256 per cell). Distances written "leptons" are read as plain numbers.

### BulletType: selection and speed

| Key | Value type | Default | Notes |
|---|---|---|---|
| `Trajectory` | `Straight`, `Bombard` or `Parabola` | unset | An unknown name logs a parse error and leaves the previous value. |
| `Trajectory.Speed` | double, leptons per frame | `100.0` | Raised to at least `0.001`. Read once for every trajectory type. |
| `Gravity` | double | `[AudioVisual] Gravity` (stock rules: 6) | Per-projectile gravity; floaters use half. Bombard and Parabola read it; Straight reads it only to cancel the stock pull. Documented under Fixed-or-Improved-Logics, listed here because the motion depends on it. |
| `BallisticScatter.Min`, `BallisticScatter.Max` | cells | `0`, `[CombatDamage] BallisticScatter` | Ares 0.7 keys that the trajectories reuse for `Inaccurate=yes` scatter. |

At read time a projectile with `Trajectory` set has `Arcing`, `Inviso`, `ROT` (non-zero) and `Vertical` forced off, each with a developer warning in the log.

### BulletType: Straight

| Key | Value type | Default |
|---|---|---|
| `Trajectory.Straight.ApplyRangeModifiers` | boolean | `false` |
| `Trajectory.Straight.DetonationDistance` | cells | `0.4` (102 leptons) |
| `Trajectory.Straight.TargetSnapDistance` | cells | `0.5` (128 leptons) |
| `Trajectory.Straight.PassThrough` | boolean | `false` |
| `Trajectory.Straight.PassDetonate` | boolean | `false` |
| `Trajectory.Straight.PassDetonateWarhead` | WarheadType | none |
| `Trajectory.Straight.PassDetonateDamage` | integer | `0` |
| `Trajectory.Straight.PassDetonateDelay` | integer frames | `1` |
| `Trajectory.Straight.PassDetonateInitialDelay` | integer frames | `0` |
| `Trajectory.Straight.PassDetonateLocal` | boolean | `false` |
| `Trajectory.Straight.LeadTimeCalculate` | boolean | `false` |
| `Trajectory.Straight.OffsetCoord` | forward,lateral,height leptons | `0,0,0` |
| `Trajectory.Straight.RotateCoord` | double degrees | `0` |
| `Trajectory.Straight.MirrorCoord` | boolean | `true` |
| `Trajectory.Straight.UseDisperseBurst` | boolean | `false` |
| `Trajectory.Straight.AxisOfRotation` | forward,lateral,height | `0,0,1` |
| `Trajectory.Straight.ProximityImpact` | integer | `0` |
| `Trajectory.Straight.ProximityWarhead` | WarheadType | none |
| `Trajectory.Straight.ProximityDamage` | integer | `0` |
| `Trajectory.Straight.ProximityRadius` | cells | `0.7` (179 leptons) |
| `Trajectory.Straight.ProximityDirect` | boolean | `false` |
| `Trajectory.Straight.ProximityMedial` | boolean | `false` |
| `Trajectory.Straight.ProximityAllies` | boolean | `false` |
| `Trajectory.Straight.ProximityFlight` | boolean | `false` |
| `Trajectory.Straight.ThroughVehicles` | boolean | `true` |
| `Trajectory.Straight.ThroughBuilding` | boolean | `true` |
| `Trajectory.Straight.SubjectToGround` | boolean | `false` |
| `Trajectory.Straight.ConfineAtHeight` | integer leptons | `0` |
| `Trajectory.Straight.EdgeAttenuation` | double, clamped to at least 0 | `1.0` |
| `Trajectory.Straight.CountAttenuation` | double, clamped to at least 0 | `1.0` |

### BulletType: Bombard

| Key | Value type | Default |
|---|---|---|
| `Trajectory.Bombard.Height` | double leptons, clamped to at least 0 | `0.0` |
| `Trajectory.Bombard.FallPercent` | double ratio (1.0 is 100%) | `1.0` |
| `Trajectory.Bombard.FallPercentShift` | double ratio | `0.0` |
| `Trajectory.Bombard.FallScatter.Max`, `.Min` | cells | `0`, `0` |
| `Trajectory.Bombard.FallScatter.Linear` | boolean | `false` |
| `Trajectory.Bombard.FreeFallOnTarget` | boolean | `true` |
| `Trajectory.Bombard.NoLaunch` | boolean | `false` |
| `Trajectory.Bombard.FallSpeed` | double leptons per frame | `0.0`, which reads as `Trajectory.Speed` |
| `Trajectory.Bombard.DetonationDistance` | cells | `0.4` |
| `Trajectory.Bombard.DetonationHeight` | integer leptons | `-1` (off) |
| `Trajectory.Bombard.EarlyDetonation` | boolean | `false` |
| `Trajectory.Bombard.TargetSnapDistance` | cells | `0.5` |
| `Trajectory.Bombard.TurningPointAnims` | list of AnimTypes | empty |
| `Trajectory.Bombard.LeadTimeCalculate` | boolean | `false` |
| `Trajectory.Bombard.OffsetCoord`, `.RotateCoord`, `.MirrorCoord`, `.UseDisperseBurst`, `.AxisOfRotation` | as Straight | as Straight |
| `Trajectory.Bombard.SubjectToGround` | boolean | `false` |

### BulletType: Parabola

| Key | Value type | Default |
|---|---|---|
| `Trajectory.Parabola.DetonationDistance` | cells | `0.4`; negative retargets (see Behaviour) |
| `Trajectory.Parabola.TargetSnapDistance` | cells | `0.5` |
| `Trajectory.Parabola.OpenFireMode` | `Speed`, `Height`, `Angle`, `SpeedAndHeight`, `HeightAndAngle`, `SpeedAndAngle` | `Speed`; blank or unknown text also gives `Speed` with no log |
| `Trajectory.Parabola.ThrowHeight` | integer leptons | `600`; a non-positive value is replaced by `600` per bullet |
| `Trajectory.Parabola.LaunchAngle` | double degrees | `30` |
| `Trajectory.Parabola.LeadTimeCalculate` | boolean | `false` |
| `Trajectory.Parabola.DetonationAngle` | double degrees | `-90` (off) |
| `Trajectory.Parabola.DetonationHeight` | integer leptons | `-1` (off) |
| `Trajectory.Parabola.BounceTimes` | integer | `0` |
| `Trajectory.Parabola.BounceOnWater` | boolean | `false` |
| `Trajectory.Parabola.BounceDetonate` | boolean | `false` |
| `Trajectory.Parabola.BounceAttenuation` | double | `0.8` |
| `Trajectory.Parabola.BounceCoefficient` | double | `0.8` |
| `Trajectory.Parabola.OffsetCoord`, `.RotateCoord`, `.MirrorCoord`, `.UseDisperseBurst`, `.AxisOfRotation` | as Straight | as Straight |

### BulletType: interception, parachute, terrain, return and underground

| Key | Value type | Default |
|---|---|---|
| `Interceptable` | boolean | `[CombatDamage] ProjectileInterceptable` |
| `Interceptable.DeleteOnIntercept` | boolean | `false` |
| `Interceptable.WeaponOverride` | WeaponType | none |
| `Strength` | integer | `0` (stock key, read for every object type) |
| `Armor` | ArmorType | unset; "unset" matters (see Behaviour) |
| `Parachuted` | boolean | `false` |
| `Parachuted.FallRate` | integer | `1` |
| `Parachuted.MaxFallRate` | integer | `[General] ParachuteMaxFallRate` |
| `BombParachute` | AnimType | `[General] BombParachute` |
| `SubjectToLand`, `SubjectToWater` | boolean | unset; "unset" matters |
| `SubjectToLand.Detonate`, `SubjectToWater.Detonate` | boolean | `true` |
| `SubjectToGround` | boolean | `false`; a different key from `Trajectory.*.SubjectToGround` |
| `ReturnWeapon` | WeaponType | none |
| `ReturnWeapon.ApplyFirepowerMult` | boolean | `[CombatDamage] ReturnWeapon.ApplyFirepowerMult` |
| `AU` | boolean | `false` |

### TechnoType: interceptor

| Key | Value type | Default |
|---|---|---|
| `Interceptor` | boolean | `false`; `false` discards all other `Interceptor.*` settings of the type |
| `Interceptor.Weapon` | integer weapon slot | `0` |
| `Interceptor.TargetingDelay` | integer frames | `1`; `0` is replaced by `1` with a log warning |
| `Interceptor.CanTargetHouses` | affected-house list | `enemies` |
| `Interceptor.GuardRange`, `.VeteranGuardRange`, `.EliteGuardRange` | cells | `0` (never intercepts); rank values default to the rookie value |
| `Interceptor.MinimumGuardRange`, `.VeteranMinimumGuardRange`, `.EliteMinimumGuardRange` | cells | `0` |
| `Interceptor.GuardRange.IsCylindrical` | boolean | `[CombatDamage]` value |
| `Interceptor.ApplyFirepowerMult` | boolean | `[CombatDamage]` value |
| `Interceptor.DeleteOnIntercept` | boolean | unset, falls back to the projectile key |
| `Interceptor.WeaponOverride` | WeaponType | unset, falls back to the projectile key |
| `Interceptor.WeaponReplaceProjectile` | boolean | `false` |
| `Interceptor.WeaponCumulativeDamage` | boolean | `false` |
| `Interceptor.KeepIntact` | boolean | `false` |

### rules.ini globals

| Key | Section | Value type | Default |
|---|---|---|---|
| `ProjectileInterceptable` | `[CombatDamage]` | boolean | `false` |
| `Interceptor.GuardRange.IsCylindrical` | `[CombatDamage]` | boolean | `false` |
| `Interceptor.ApplyFirepowerMult` | `[CombatDamage]` | boolean | `true` |
| `ReturnWeapon.ApplyFirepowerMult` | `[CombatDamage]` | boolean | `false` |

The warhead key `AffectsUnderground` (needed for `AU` to do damage) is a separate WarheadType key and is not specified here.

### Where docs and code disagree

- The docs describe `Trajectory.Disperse`; the code has none. Setting `UseDisperseBurst=yes` only turns off the projectile's own burst fan.
- The Straight docs say `PassDetonateDelay` controls the delay of `Trajectory.Straight.Warhead`. That key does not exist; the delay belongs to `PassDetonateWarhead`.
- The Parabola docs say `Angle` mode is expensive and may have no solution. Without lead time the code now solves it by formula (changed May 2026). With lead time the code still runs a ten-step Newton search.
- The Bombard docs call `FallPercent` a percentage. The value is a ratio, and `1.0` is the whole distance.
- The Straight docs say `ConfineAtHeight` is cancelled "above 256". The code cancels it at `Trajectory.Speed` of 256 or more.
- The Parabola docs list `HeightAndAngle` as "0 to 90 exclusive" and `Angle` as "-90 to 90 exclusive". Out-of-range angles do not fail; the fixed-height modes and `SpeedAndAngle` substitute 30 degrees. `Angle` mode instead fails to solve (open question 3).

## Behaviour

### Common to all three trajectories

- A projectile with `Trajectory` gets a trajectory object when it is placed on the map, after the stock launch has set its source and target coordinates. The object replaces the launch velocity.
- On every frame the trajectory runs first, before the stock bullet update. It sets this frame's velocity and may ask for detonation. The stock update then moves the bullet by that velocity. Stock gravity is cancelled for all three types, because each one applies its own vertical motion.
- The trajectory skips these stock checks: the "reached target coordinate" test, the "close to a techno" proximity test, the `Ranged=yes` range test and the aircraft inertia rewrite of the launch velocity. Straight and Bombard also skip the stock bridge-deck test unless their own `SubjectToGround` is on; Parabola keeps it.
- Stock snapping of the explosion onto the target is suppressed. The explosion happens where the bullet is, unless the trajectory's own snap rule (`TargetSnapDistance`) fires.
- The snap rule: just before detonation, if the target (or its stored coordinate once the target is gone) is within `TargetSnapDistance` of the bullet, the bullet is moved onto the target coordinate and flagged as snapped. A value of 0 disables it. Straight with `PassThrough` never snaps.
- Aircraft that fire a trajectory projectile do not strafe unless the weapon's `Strafing` key says so.
- Firer death clears the bullet's firer. The bullet keeps flying, damage is credited to the stored firer house, and anything that needs the firer (burst index, firepower) was copied at launch.
- Cloaking, EMP, temporal effects, owner change and transport entry of the target have no effect on a trajectory. It flies to the stored target coordinate. Only the lead-time and snap rules read the live target position.
- Scatter for `Inaccurate=yes` projectiles is applied after lead time and offset. The scatter radius is a random whole number between `0.0004 x distance x Min` and `0.0004 x distance x Max` leptons, where distance runs from source to aim point and Min and Max are the `BallisticScatter` values in leptons (defaults 0 and the global `BallisticScatter`). The aim point then moves to a random spot around itself at about that radius, using the game's random-coordinate function. All draws come from the synchronized random number generator.

### Aim point, offset and burst fan (shared by all three)

- Aim point order: target coordinate, then lead time (if enabled), then `OffsetCoord`, then `Inaccurate` scatter.
- `OffsetCoord` is given as forward, lateral, height in leptons, relative to the direction from source to target. With angle A = atan2(target y - source y, target x - source x), the aim point moves by (forward x cos A + lateral x sin A, forward x sin A - lateral x cos A) on the map and by the height value vertically. With `MirrorCoord=yes`, odd burst indices (1, 3, ...) use the negated lateral value.
- If the aim point equals the source point (a case the docs tie to Disperse and airburst splitting), lead time is off and the firer exists, A is measured from the firer to the aim point.
- `RotateCoord` fans a burst. It applies when `RotateCoord` is not 0, the weapon's `Burst` is greater than 1 and `UseDisperseBurst` is off. Let i be the burst index, B the burst count and R the key in degrees. Without mirroring, the launch direction is rotated about the axis by R x (i / (B - 1) - 0.5) degrees, so the bursts spread evenly from -R/2 to +R/2. With mirroring, the angle uses floor(i / 2) in place of i and odd indices rotate about the reversed axis, so shots alternate sides. `AxisOfRotation` is given in the same forward, lateral, height frame; a zero-length axis disables the fan. Bombard rotates again at the turning point and uses twice the angle there.
- Lead time (`LeadTimeCalculate`) has an effect only against foot targets (infantry, vehicles, aircraft); other targets never move, so the aim point stays. Straight and Parabola then wait two updates before launching: the first records the target's position, the second computes the launch. Bombard waits only with `NoLaunch`; otherwise it computes the lead at the turning point. The target's velocity is its position change over that one frame. With target position T, velocity v, source S and projectile speed s, the aim point is T + v x t, where t is the smaller positive root of |(T - S) + v t| = s t, truncated to whole frames, plus 1 frame when the target is closing and 2 when it is not. If the target is stationary or no root exists, the aim point is the current position. Bombard with `FreeFallOnTarget` uses the free-fall time sqrt(2 x drop / gravity) and with `NoLaunch` the time drop / `FallSpeed` instead.

### Straight

- Launch: the velocity points from source to aim point in three dimensions and is scaled to `Trajectory.Speed`. Gravity plays no part. If source and aim point coincide, the bullet detonates on its first update.
- Travel budget: without `PassThrough`, the bullet may travel `distance(source, aim point) + Trajectory.Speed + 1` leptons in total and then detonates. With `PassThrough` and `DetonationDistance` above 0 the budget is `DetonationDistance + speed + 1`. With a negative value it is `distance to aim point + |DetonationDistance| + speed + 1`. With 0 the bullet has no distance limit and ends at the map edge or on another stop condition. `ApplyRangeModifiers` applies the firer's current weapon range modifiers to `DetonationDistance` and to the damage-attenuation range.
- `PassThrough` height: if source and target stand on the same terrain level (or their aim heights differ by 32 leptons or less), the height difference is spread over the whole planned travel distance instead of the distance to the target: the vertical component is multiplied by distance(source, target) / planned distance. The bullet is then not yet at the target's height when it passes the target. With an unlimited budget it flies level. On different levels the line simply runs through the aim point and on.
- Per-update order of checks:
  1. A pending obstacle (the bullet's stored "extra check" object) detonates the bullet.
  2. The budget is reduced by the speed; below 0 detonates.
  3. Without `PassThrough`, a bullet less than `DetonationDistance` from the aim point at the start of the update detonates. This test runs before the move, so the bullet can stop up to one speed step short.
  4. With `Trajectory.Straight.SubjectToGround`, a bullet 16 leptons or more below the ground detonates.
  5. A bullet outside the map detonates.
  6. Obstacles: if `ThroughBuilding` or `ThroughVehicles` is off, the objects in the cell the bullet is in are searched (below speed 256), or in every cell it will cross this update (speed 256 or more). At speed 256 or more the same sweep also stops the bullet at ground below it (`SubjectToGround`) and at a wall cell (projectile `SubjectToWalls`). An allied object is ignored unless it is the target; invisible buildings are ignored. A building blocks when `ThroughBuilding` is off (a single-cell building that undeploys uses `ThroughVehicles` instead); a ground vehicle or aircraft blocks when `ThroughVehicles` is off. The first blocker is stored, the bullet retargets onto it and its step is shortened to reach it, and it detonates on the next update.
  7. `PassDetonate`: when the timer (started with `PassDetonateInitialDelay`, then `PassDetonateDelay`) has run out, `PassDetonateWarhead` detonates at the bullet's position with `PassDetonateDamage`. `PassDetonateLocal` moves the detonation to ground height.
  8. Proximity fuse (see below).
  9. `ConfineAtHeight` (only below speed 256): the vertical velocity is adjusted so the next position sits `ConfineAtHeight` leptons above the ground there (bridges count as ground). If that ground differs from the bullet's height by 384 or more and the projectile is `SubjectToCliffs`, the bullet detonates. The velocity is then rescaled to the full speed; with `PassDetonateLocal` it is not, so the speed varies.
  10. If an obstacle was stored (step 6, or the last proximity use), the proximity warhead also detonates at it when `ProximityImpact` is not 0, and the damage attenuation step runs.
- Proximity fuse: active when `ProximityImpact` is not 0 and `ProximityRadius` is above 0. Each update it collects ground technos in the cells within `ProximityRadius` of the segment the bullet will cover this update, plus aircraft within the same cylinder when `ProximityFlight` is on. Dead, limboed, sinking, off-map and invisible-building objects are skipped, as are allies of the firer's house unless `ProximityAllies` is on or the object is the target. An object must lie ahead of the launch point and not beyond the end of this update's step, and a non-building must be within `ProximityRadius` of the line; buildings are accepted by the cell test alone. Objects recorded in the last 20 frames are skipped, and every object seen again refreshes its entry to 20 frames, so one object is hit once while it stays in the fuse area and again only after leaving it for 20 frames. If more objects qualify than `ProximityImpact` has left, they are served nearest to the launch point first (ties by object id). Each hit uses `ProximityWarhead` and `ProximityDamage`:
  - `ProximityDirect`: the object takes the damage directly from the firer.
  - `ProximityMedial`: the warhead detonates at the bullet.
  - Otherwise it detonates at the object with the object as its target.
  - A positive `ProximityImpact` is reduced by 1 per hit. When one use is left, the next qualifying object (even one the bullet may not pass) becomes the bullet's last target: the bullet retargets onto it, the proximity warhead hits it and the bullet detonates there. A negative value never runs out.
  - With two or more uses left, objects the bullet may not pass (`ThroughVehicles=no`, `ThroughBuilding=no`) are skipped here and handled by the obstacle rule.
- Damage scaling: `EdgeAttenuation` is the damage factor at the weapon's range. The factor is 1 within the first cell, then falls linearly to `EdgeAttenuation` at the range, and stays there beyond. It uses the distance from the launch point to the object (or bullet). The main damage is multiplied by the factor; proximity and pass damage are multiplied by the factor and by the firer's firepower multiplier at launch. Results truncate toward 0, with a minimum magnitude of 1 when `EdgeAttenuation` is above 0. When `EdgeAttenuation` is exactly 1.0 all damage values are passed through unchanged, and the firepower multiplier is not applied to proximity or pass damage. `CountAttenuation` multiplies the main, proximity and pass damage after every extra detonation (minimum magnitude 1; a value of 0 sets them to 0).

### Bombard

- Turning point: the bullet first flies to a point above the line from source to target. Its horizontal position is source + (target - source) x f, where f = `FallPercent` - `FallPercentShift` + a random value between 0 and 2 x `FallPercentShift` (in steps of 0.01). Its altitude is the target's height plus `Height` leptons. `FallScatter` then moves the point horizontally by a random radius between `.Min` and `.Max`; with `.Linear` the move is along the line perpendicular to the flight direction, in a random one of its two senses. The launch velocity points to this point at `Trajectory.Speed`.
- The bullet travels a budget of distance + speed + 1 leptons. When the remaining budget drops below one step, the velocity is shortened so the next step ends exactly on the turning point. On the update after that, the descent begins:
  - `FreeFallOnTarget=yes` (default): the bullet jumps to the point above the aim point at its current height, its velocity is set to 0, and from then on it falls under gravity (vertical speed reduced by gravity each frame).
  - `FreeFallOnTarget=no`: the bullet keeps its position, is aimed at the aim point and moves at `FallSpeed` (or `Trajectory.Speed` if `FallSpeed` is 0), with the fan rotation applied again. A budget of distance + `FallSpeed` + 1 ends the descent.
  - With `LeadTimeCalculate`, the aim point at the turning point is moved by how far the target has travelled since launch plus the lead computed at that moment. Without it the bullet falls on the original aim point and never tracks.
  - The line trail and laser trails restart at the turning point, and one animation from `TurningPointAnims` (random pick) plays there.
- `NoLaunch=yes`: the bullet is created already at the turning point (above the aim point for free fall, otherwise on the line as above) and starts its descent at once. The firer still shows its normal firing effects.
- Detonation checks each update, before the move: aim point closer than `DetonationDistance`; `DetonationHeight` reached (when 0 or more: while descending, below that height above the launch height; with `EarlyDetonation`, while ascending, above it); for free fall or `SubjectToGround`, the bullet is 15 leptons or more below the ground. The snap rule applies as listed under "Common".
- Stock building and unit proximity tests are skipped. A Bombard shell passes over buildings and units and detonates at the ground, the aim point or its height rules. No sweep is made over cells crossed in one step, so very high speeds can pass through the ground.

### Parabola

- The bullet follows a gravity arc in three dimensions, with horizontal velocity constant and vertical velocity reduced by gravity every frame. With gravity of 0 or less it detonates on its first update.
- Launch velocity depends on `OpenFireMode`. Let D be the vector from source to aim point, h its horizontal length, dz its height, g gravity and s `Trajectory.Speed`:
  - `Speed`: horizontal speed s, flight time h / s, vertical velocity dz / t + g x t / 2.
  - `Height`: the apex is `ThrowHeight` above the higher of source and target (the target counts only when dz is above 0). Vertical velocity is sqrt(2 g x rise to apex), flight time is the rise time plus the fall time sqrt(2 x drop / g), and horizontal speed is h divided by the flight time. Every shot to the same height difference takes the same time.
  - `Angle`: fixed pitch A. Total speed v follows from v^2 = g h^2 / (2 cos^2 A (h tan A - dz)). If h tan A - dz is 0 or less, or cos A is near 0, there is no solution (open question 3).
  - `SpeedAndHeight`: speed s horizontally, vertical velocity from the apex rule above.
  - `HeightAndAngle`: vertical velocity from the apex rule, horizontal speed vertical / tan A.
  - `SpeedAndAngle`: horizontal speed s, vertical velocity s x tan A. The target only gives the direction.
  - All modes add g / 2 to the vertical velocity to offset the first frame's gravity step.
- With `LeadTimeCalculate` and a live foot target, the same modes aim at the target's position when the bullet lands. For the fixed-horizontal-speed modes the landing time t solves |horizontal(T - S) + horizontal velocity of target x t| = s x t, as a quadratic. For `Height`, `Angle` and `HeightAndAngle` it comes from ten Newton steps. If the search fails, the shot falls back to the no-lead solution.
- If `DetonationDistance` is negative and the target is a foot object, the bullet retargets to the cell under the target at launch and uses no lead time.
- Per-update order: wait-state for lead time; early detonation (see below); out-of-map detonation; bounce handling if one is pending; then gravity is applied and the next position is tested against the ground (the cell floor height), against walls when `SubjectToWalls` is on, and, above 256 leptons of horizontal speed, against every cell on the way. A ground hit shortens the velocity so the bullet lands on the surface and sets either a bounce or a detonation for the next update; the snap rule or a clamp to the floor height then applies.
- Early detonation (any one of): descending and below the launch height plus `DetonationHeight` (when 0 or more); `DetonationAngle` crossed (for 0, vertical velocity 0 or lower; for angles between -90 and 90, the ratio of vertical to horizontal speed is below tan of the angle); aim point closer than `DetonationDistance`.
- Bounce: allowed while `BounceTimes` has uses left. On a water cell it detonates instead unless `BounceOnWater` is on. The velocity is reflected about the ground normal (flat ground, a ramp from the cell's slope index, or a cliff face estimated from neighbouring cell heights; the normals are fixed unit vectors) and multiplied by `BounceCoefficient`. With `BounceDetonate`, the projectile's warhead detonates at the bounce point first with the current damage. The damage is then multiplied by `BounceAttenuation` (minimum magnitude 1). The final detonation uses the last damage.
- Stock building and unit proximity tests are skipped. A Parabola shell passes through buildings and units.

### Interception

- Stock YR has no way to target a projectile. Phobos makes a bullet a valid target for a weapon when the weapon's firer has `Interceptor=yes`.
- Scan (per interceptor, in the techno update): it runs only on frames where (current frame + a per-techno random factor 0 to 15, fixed when the techno is created) is a multiple of `Interceptor.TargetingDelay`. It is skipped if no bullets exist, the techno is burrowed or unarmed, a building is selling or under construction, or the techno's current target is something other than a bullet. If the current target is a bullet that is not locked, it keeps it. Otherwise it walks the bullet list in list order and takes the first bullet that is not already targeted or locked, or failing that the first targeted or locked one, if the bullet passes all of:
  - the type is interceptable and the bullet is not a nuke build-up;
  - squared distance from the interceptor is within [minimum, guard range] squared for the techno's rank (height ignored if cylindrical);
  - if the projectile has `Armor`, the interceptor weapon's warhead has a non-zero `Verses` for it;
  - `CanTargetHouses` accepts the bullet's firer house (the stored house if the firer is dead).
- A building with a chosen bullet target is moved to the attack mission at once. Weapon choice for a bullet target is always `Interceptor.Weapon`, and the usual ground-or-air check is skipped for bullet targets. The weapon's projectile needs `AA=yes` to reach bullets above ground level.
- On firing, the shot is marked as an interceptor shot. If the interceptor's `ApplyFirepowerMult` is off, its damage is the weapon's plain `Damage`. If the target bullet has no `Armor`, it becomes "locked" so other interceptors choose a different bullet; each update the interceptor shot re-locks it.
- Resolution: when the interceptor shot's warhead detonates, bullets are matched like this. With `CellSpread=0`, only the shot's own target bullet qualifies, and only if the detonation is within 32 leptons of it (any distance for `Inviso=yes` shots). With `CellSpread` above 0, every interceptable bullet within CellSpread x 256 leptons (a sphere) qualifies, whatever its owner.
- Hit effect on a qualifying bullet:
  - No `Armor` on the projectile: it is marked intercepted (unless `KeepIntact`).
  - With `Armor`: the damage is the interceptor shot's damage times the warhead's `Verses` for that armor, truncated. A `Verses` of 0 stops the processing here. The bullet's current strength (starts at the type's `Strength`) is reduced by the damage; at 0 or below it is marked intercepted (unless `KeepIntact`).
  - Next, whether or not it died: the bullet's `DeleteOnIntercept` flag takes the interceptor key if set, else the projectile key. If an override weapon is set (interceptor key first, then projectile key), the bullet takes that weapon, warhead and brightness, and its damage becomes the weapon's `Damage` (or, with `WeaponCumulativeDamage`, its current damage plus that `Damage`). With `WeaponReplaceProjectile` it also takes the weapon's projectile type and speed; if the new type is no longer interceptable or no longer takes damage from the warhead, the interceptor drops its target.
- An intercepted bullet is processed at the start of its next update: it detonates where it is (unless delete-on-intercept), then is removed. If the firer was a limbo-launched missile unit, the unit is destroyed.
- `Inviso=yes` shots aimed at bullets detonate at once on their first update.

### Return weapon

- When a projectile with `ReturnWeapon` detonates and its firer is still alive, a second projectile from that weapon is created at the detonation point, aimed at the firer, with the weapon's damage, warhead and speed. It flies in a straight line to the firer's centre at the weapon's speed, or on a lob if its projectile is `Arcing`. Its damage is multiplied by the original shot's firepower multiplier (firer, house, veterancy, attached effects, occupant and bunker bonuses at launch) when `ReturnWeapon.ApplyFirepowerMult` is on. The default is off.
- This runs in the detonation, after the extra warheads and before the unlimbo-detonate effects. The return shot plays the weapon's laser, electric bolt, radiation beam and particle system, but not its firing animation or report sound. If the firer is dead, nothing is fired. The return projectile may itself have a trajectory.

### Parabombs

- `Parachuted=yes` makes any projectile of that type (the docs say "fired by an aerial unit"; the code does not check the firer) appear as a parachuting object instead of flying: when it is placed on the map it gets a parachute animation (`BombParachute`, owned by the firer house, drawn with the house colour scheme when the animation type uses the cell drawer), its velocity is set to 0, and the stock movement is skipped for the rest of its life.
- Each frame the fall rate becomes (previous rate - `Parachuted.FallRate`), limited at the low end by `Parachuted.MaxFallRate`. Rates are negative, so with the default `FallRate=1` the bullet's descent speed grows by 1 each frame until it reaches the cap.
- Phobos disables Ares's own parabomb patch; if both exist, Phobos wins. `ROT` above 0 or `Vertical=yes` together with `Parachuted` is advised against.

### Projectiles blocked by land or water

- `SubjectToLand` and `SubjectToWater` count as set only when the key appears in the INI, even as `no`. A projectile with either key set is checked against terrain. A projectile with neither set is checked only if it has `Level=yes`.
- Water means the cell's land type is Water or Beach. A projectile with `SubjectToLand=yes` is stopped by non-water cells; one with `SubjectToWater=yes` by water cells. `.Detonate` (default `yes`) decides whether a projectile that reaches such a cell during flight detonates.
- Three places use the test:
  - Targeting: for every cell on the line from the firer's weapon position to the target (including the target cell), a stopping cell makes the target out of range, and the firer looks for another firing position. The `.Detonate` value is ignored here.
  - Placing an `Inviso=yes` projectile: if the line to the target has a stopping cell that would detonate, the projectile is placed on that cell's centre with speed 0.
  - Each frame of a flying projectile: the cell it was last in is tested, and a stopping cell that would detonate ends the flight.
- `Level=yes` projectiles with neither key set keep the stock rule of detonating on floor cells; they are not treated as obstacles by the targeting test.
- `SubjectToGround=yes` (the plain key) affects targeting only. The firer predicts its muzzle position, samples the straight line to the target and counts the target as blocked if the line dips below the ground. It does not apply to buildings, aircraft, objects in the air, passengers or targets in the air. The trajectory keys of the same name change the flight itself.

### Attack technos underground

- `AU=yes` marks a projectile that may be aimed at objects in the underground layer (units that are tunnelling). Without it a weapon cannot fire at such an object, and the player cannot order the attack on a tunnelling unit that has not surfaced.
- With `AU` the weapon's threat mask gains the underground flag. Automatic target scans for such a weapon include infantry, units and aircraft bits and a scan over a scenario-wide list of tunnelling objects, so the weapon can pick a tunnelling unit by threat value. When a weapon is chosen for a target in the underground layer, the weapon with `AU=yes` is used.
- Damage requires the warhead key `AffectsUnderground`; the projectile must also be able to travel underground (`Inviso=yes`, or Straight/Bombard with `SubjectToGround` off). Tracking sideways movement needs `SensorsSight` on the attacker.

### Multiplayer determinism (Phobos)

- All trajectory maths is in double precision with truncating casts to integer leptons, using the C library for sine, cosine, tangent, arctangent and square root. Nothing is fixed-point or table based. Machines agree only if they run the same binary and math library.
- Random numbers all come from the synchronized scenario generator. Bombard draws: the turning point shift (once per launch, only when `FallPercentShift` is 0.005 or more), the scatter radius (only when `.Min` and `.Max` differ), the scatter angle or side (always once per launch that computes a turning point), and the `TurningPointAnims` pick (only when the list has more than one entry). `Inaccurate=yes` adds one radius draw plus whatever the random-coordinate function draws, in all three types. Phobos also draws a per-techno interceptor factor (0 to 15) when each techno is created, for every techno, used or not. Straight and Parabola use no other random numbers. The engine's range pick (`Pick_Random_Number`) returns the lower bound without a draw when both bounds are equal and otherwise redraws until a value fits, so the number of draws depends on the values.
- Frame counts used: the pass-detonate timer, the 20-frame recent-hit memory, the interceptor delay, and the "wait two updates" lead-time start.
- Container order: interceptors scan the bullet list in list order, proximity candidates come from a fixed cell walk and the cell's object chain, and ties sort by object id.

## What stock YR does

- A projectile flies one of three stock ways: vertical (`Vertical=yes`), steered (`ROT` above 0) or ballistic. Ballistic flight, including `Arcing=yes` and level shots, starts with a launch velocity worked out by the firer from the weapon speed, range and gravity. Gravity then pulls it down each frame, it stops at the ground, a building or wall, or when it comes within half a cell of a non-allied object, and `Bouncy=yes` projectiles bounce up to three times.
- There is no straight, bombard or bouncing-arc flight with per-type speed and no proximity or pass-through damage. A weapon cannot target a projectile. `Strength` and `Armor` on a projectile type have no effect.
- A projectile has no parachute behaviour of its own: a `Dropping=yes` bomb falls with the carried aircraft speed and stock gravity.
- Projectiles are blocked only by cliffs (`SubjectToCliffs`), walls (`SubjectToWalls`) and, for `Level=yes`, floor cells. Weapons without a clear cliff or wall line of fire do not fire.
- A weapon cannot target a unit that is underground.

## Where it hooks in OpenYR

The engine already contains the stock flight models, so the new motion layer sits beside them.

What exists today (names checked in `code/`):

- `BulletClass::AI` in `code/bullet.cpp` runs one of three branches: `Class->IsVertical`; `Class->ROT > 0` (calls `Projectile_Motion` in `code/missile.cpp`); the ballistic `else` branch (gravity from `Rule->Gravity` or `Get_Floater_Gravity` in `code/combat.cpp`, bounce, obstacle and nearby-techno tests, `BounceCount`). After the branches, a shared tail handles firestorm walls, `Is_Forced_To_Explode`, the fuse, snap-to-target rules in `Bullet_Explodes` and the explosion in `BulletClass::Detonate`.
- Velocity is `TVelocity3D<double>` (`code/velocity.h`) and the position is truncated to integer leptons every frame, so the engine already moves bullets in double precision. Trigonometry in simulation code goes through `Gamemd_Sine`, `Gamemd_Cosine`, `Gamemd_Atan2`, `Gamemd_Acos` and `Gamemd_Sqrt` (`code/gmmath.h`), which reproduce gamemd's tables bit for bit. `/fp:precise` with SSE2 is set in `code/CMakeLists.txt`.
- Launch: `TechnoClass::Fire_At` in `code/techno.cpp` creates the bullet with `Create_Bullet`, works out the direction and pitch (`Calculate_Projectile_Pitch` and `Calculate_Projectile_Angle` for arcing shots), applies `Ballistic_Offset` scatter for inaccurate arcing shells, and calls `BulletClass::Unlimbo` with the velocity. `AircraftClass::Fire_At` in `code/aircraft.cpp` then rewrites the velocity for `ROT` 0 and 1.
- `BulletClass::Unlimbo` records `SourceCoord`, applies flak scatter to invisible shells and arms `Fuse` (`code/fuse.h`). There is no stored target coordinate; the local `tcoord` goes into the fuse only.
- `fly.cpp` (`FlyLocomotionClass`) and `rocket.cpp` (`RocketLocomotionClass`) move aircraft and rocket units. They have no part in projectile motion.
- `BulletTypeClass` (`code/bullettype.h`, `code/bullettype.cpp`) holds the per-type flags and ints, reads them in `Read_INI`, saves them in `Serialize` and hashes the simulation-relevant ones in `Compute_CRC`. `ObjectTypeClass` already reads `Strength` (`MaxStrength`) and `Armor` for every type.
- `ObjectClass::AI` already runs the falling logic (`IsFalling`, `Riser.Z`, `ParachuteMaxFallRate`) and `ObjectClass::Paradrop` already attaches `Rule->BombParachute` to a bullet. Nothing calls `Paradrop` for a bullet today.
- `BulletClass::Nuke_Maker` and `BulletClass::Shrapnel` are examples of a bullet creating further bullets. `xTargetClass::As_Bullet` in `code/target.cpp` shows a bullet can be an encoded target.
- Targeting: `TechnoClass::Can_Fire`, `What_Weapon_Should_I_Use`, `In_Range`, `Is_Close_Enough` and `Clear_Line_Of_Fire` in `code/techno.cpp`. `Find_Projectile_Obstacle` (static, same file) already walks the line for cliff and wall stops. No `Level` projectile key and no `Interceptor`, `Trajectory`, `Parachuted`, `ReturnWeapon`, `SubjectToLand` or `AU` handling exists (checked by grep of `code/` and `manual/data/ini-keys.yaml`).
- Layers: `LAYER_UNDERGROUND` objects are listed in `DisplayClass::Layer`; `TunnelLocomotionClass::In_Which_Layer` in `code/tunnel.cpp` returns it while tunnelling.

What a motion layer replaces and wraps:

- It replaces the ballistic `else` branch of `BulletClass::AI` for projectiles that have a `Trajectory`. The steered and vertical branches are untouched, because the validation forces `ROT`, `Vertical`, `Arcing` and `Inviso` off for such projectiles.
- It wraps the shared tail. The trajectory needs these changes there: no snap onto the target in `Bullet_Explodes` (the `Fuse_Target`, 32-lepton and 42-lepton snaps) unless the trajectory snapped; no anti-air proximity test in `Is_Forced_To_Explode` for trajectory shells; the fuse is not armed or checked for them.
- It replaces the launch velocity for these projectiles. The simplest cut is at the end of `BulletClass::Unlimbo`: create the state, compute the launch (it overrides the velocity passed in), and skip the aircraft rewrite in `AircraftClass::Fire_At` when a trajectory is present.
- New pure functions for the maths belong in new files (suggested `code/bulletmotion.h` and `code/bulletmotion.cpp`), working on a plain state struct with no virtuals. That keeps them unit-testable like `tests/fuse` and `tests/gmmath`, and fits the component direction in `docs/DIRECTION.md`. `BulletClass` holds one state struct.
- Math choice (owner decision): `Gamemd_Sqrt`, `Gamemd_Sine`, `Gamemd_Cosine` and `Gamemd_Atan2` are deterministic everywhere but quantised (sine and cosine in steps of pi / 4096, arctangent coarse near the x axis). The C library's `std::sin`, `std::tan` and `std::atan2` are finer but may differ in the last bit between the Win32 and x64 builds and between runtime versions. A one-bit difference changes an integer lepton after truncation. Plain `+ - * /` and `std::sqrt` are exact in IEEE doubles. Trajectories do not exist in gamemd, so only agreement between OpenYR builds matters.

Build order (smallest first):

1. Plumbing: `Trajectory` and `Trajectory.Speed` parse in `BulletTypeClass::Read_INI`, a trajectory parameter block per type with `Serialize` and `Compute_CRC`, a state struct on `BulletClass` with `Serialize`, the validation warnings, a `Gravity` per-type key, and the hook points above. Bump `SaveVersionInfo::REVISION` in `code/savever.h`.
2. Parabombs: `Parachuted*` keys; in `Unlimbo` call `ObjectClass::Paradrop`-style setup for the bullet, zero the velocity, skip stock motion while `IsAnimAttached`, and drive `Riser.Z` from the fall-rate rule.
3. `ReturnWeapon`: a method next to `Shrapnel`/`Nuke_Maker` called from `BulletClass::Detonate`, plus a helper that launches a bullet from a point toward a target (Phobos's `SimulatedFiringUnlimbo`; the cluster code in `Detonate` is the nearest existing equivalent).
4. Straight core: aim, speed, offset, inaccurate scatter, detonation distance, snap, travel budget. Then `PassThrough`, `ThroughVehicles`/`ThroughBuilding`, `SubjectToGround`. Then proximity, `PassDetonate`, attenuation, `ConfineAtHeight`. Then lead time and the burst fan, which Bombard and Parabola reuse.
5. `SubjectToLand`/`SubjectToWater`/`SubjectToGround`: extend `Find_Projectile_Obstacle` and `Clear_Line_Of_Fire` (targeting), `Unlimbo` for invisible shells and `Is_Forced_To_Explode` (flight). Decide first whether to add stock `Level` (open question 9).
6. Interception: per-type `Interceptor` block on `TechnoTypeClass` (`code/techtype.cpp`, with `Serialize` and CRC), `Interceptable`/`Armor`/`Strength` handling, per-bullet state, bullet targets in `Can_Fire` and `What_Weapon_Should_I_Use`, the scan in `TechnoClass::AI`, resolution in warhead detonation, handling at the start of `BulletClass::AI`.
7. Bombard (shares the code from step 4; adds random draws and the two-phase flight).
8. Parabola: solvers for the six modes, lead-time search, ground sweep, bounce normals from the cell slope index.
9. `AU`: projectile flag, `Can_Fire`/scan changes, with the warhead `AffectsUnderground` key. Scan `DisplayClass::Layer[LAYER_UNDERGROUND]` instead of keeping a second list.

## Saved state

Rules for the engine: any member added to a `Serialize` changes the save layout, so raise `SaveVersionInfo::REVISION` (`docs/SAVE-FORMAT.md`). Pointer members go through `stream.Serialize` and need clearing in `BulletClass::Detach`.

Per bullet (new, all saved; hash into a new `BulletClass::Compute_CRC` override, since `ObjectClass::Compute_CRC` covers the position but not velocity or flight state):

- Common: trajectory kind (or none), burst index and count, wait-for-lead counter (0 to 2), last target coordinate, aim point (`TargetCoords`), per-bullet offset (mirrored), snapped flag, firepower multiplier at launch (double), firer house (a dead firer's house must be kept).
- Straight: remaining travel budget (int), remaining detonation distance, pass-detonate damage, proximity uses left, proximity damage, extra-check object pointer, recent-hit list (object id and counter, up to a few dozen entries), edge-attenuation range, pass-detonate timer (frame timer).
- Bombard: current height, current fall percent after random shift (double), falling flag, turning flag, remaining budget, initial target coordinate, rotation angle (double).
- Parabola: remaining throw height, bounce uses left, should-detonate, should-bounce and extra-check flags, last velocity (3 doubles).
- Interception: current strength (int), intercepted-status bits, delete-on-intercept flag, interceptor type pointer, plus the swapped weapon and projectile when overridden (the bullet's weapon, warhead and type already exist as saved members).
- Parabombs: parachute fall rate (int); the parachute animation is already linked through `IsAnimAttached`.

Per type (saved with the type's `Serialize`, simulation keys also in `Compute_CRC`): the trajectory parameter block (about 30 values for Straight, 22 for Bombard, 19 for Parabola), `Interceptable*`, `Armor` set-flag, `SubjectTo*`, `Parachuted*`, `ReturnWeapon*`, `AU`, `Gravity`. Per TechnoType: the interceptor block (13 values). Rules: the four `[CombatDamage]` defaults.

Per techno: none needed if the scan stagger is taken from the object id (for example `Fetch_ID() % 16`) instead of a random draw at creation. A random draw would change the random sequence of every game, even those that never use the feature.

Per house or global: none. `AU` can scan the existing underground layer.

## Open questions

1. Lead time under-leads receding targets. The Straight and Bombard formula takes the along-track distance as an unsigned value, so it assumes the target approaches. Worked example: target 1000 leptons away moving directly away at 50 per frame, projectile speed 100: the correct time is 20 frames; the code gives 6, plus 2, which is 8. For a closing target it matches. Confirm by test (test 5) before deciding whether OpenYR copies it.
2. With `EdgeAttenuation` at 1.0, proximity and pass damage skip the firepower multiplier; with any other value they apply it. This looks unintended.
3. A failed fixed-angle `Angle` solution only writes a log line. The bullet keeps a zero horizontal velocity and falls at the firer. Decide whether OpenYR should detonate instead.
4. The cell-spread branch of interception (`CellSpread` above 0) ignores `CanTargetHouses` and hits friendly interceptable bullets in the sphere. Also, `WeaponCumulativeDamage` adds the weapon's damage again on every hit, even when the bullet survives. Both look unintended.
5. `Interceptor.GuardRange` is described as limited by weapon range; the scan code does not clamp it. Whether a mobile interceptor chases a bullet out of range is unclear.
6. Whether a `Parachuted` projectile detonates on landing without `Dropping=yes` (a bullet that reaches height 0 does not meet the stock "below ground" test). Test before copying.
7. Which side of the firing line positive `OffsetCoord` lateral values fall on. The formula is given above; the visual side needs a test.
8. A shell that moves several cells in one frame can skip the ground in Bombard, and Straight sweeps cells only for obstacle rules. Decide whether OpenYR sweeps in every type.
9. `Level` is not implemented in OpenYR (no `Level` projectile key). Decide whether `SubjectToLand`/`SubjectToWater` ship without it.
10. Math library policy (see the choice above) and whether Phobos's `ScenarioClass::Random` draw order must be matched. It does not matter for compatibility with gamemd replays, since gamemd has no trajectories.
11. Interceptor scan stagger: Phobos draws a random factor at techno creation. The proposal above uses the object id instead. This differs from Phobos only in which frame a given interceptor scans.
12. `ReturnWeapon` firepower: Phobos stores the firer's multiplier at launch. OpenYR can store it or recompute from the firer at detonation with `TechnoClass::Fire_Damage`; the two differ if the multiplier changed in flight.
13. `Interceptor.GuardRange` and the other range keys are read in cells but compared as squared lepton distances from the interceptor's map position, not its centre. For tall buildings this shifts the reach; whether to copy that is undecided.

## Test plan

Use a skirmish map and a small mod. Copy a stock ground weapon with its projectile and warhead for each test; call them `TestGun`, `TESTSHELL`, `TestWH`. `TestWH` has `Verses=100%` for every armor and `CellSpread=0` unless a test says otherwise. Targets are stationary enemy units with `Strength=1000`, so damage can be read from the health bar or the damage numbers. Positions are in cells from the firer.

1. Straight against stock. `TESTSHELL`: `Trajectory=Straight`, `Trajectory.Speed=20`, `Image` of a visible shell, weapon `Speed=100`. Fire at a target 10 cells away on flat ground. Expected: the shell crawls in a straight line at 20 leptons per frame (10 cells take about 125 frames) and hits the centre of the target. With the `Trajectory` lines removed and `Arcing=no`, the stock shell arrives in a fraction of that time. Edge: set `Trajectory.Speed=300`. Expected: the shell still detonates on or next to the target and does not fly past it, because the travel budget shortens the last step.
2. Straight pass-through and proximity. Targets `T1`, `T2`, `T3` in one line at 4, 7 and 12 cells. Projectile: `Trajectory=Straight`, `Trajectory.Speed=40`, `PassThrough=yes`, `DetonationDistance=10`, `ProximityImpact=-1`, `ProximityWarhead=TestWH`, `ProximityDamage=30`, `ProximityRadius=0.7`; the weapon's own damage is 1. Expected: `T1` and `T2` each lose 30 once (not 30 per frame); `T3` is untouched; the main warhead goes off 10 cells from the firer. Edge: `ProximityImpact=1`. Expected: only `T1` is hit, with 30 plus the weapon damage, and the shell ends there.
3. Straight obstacles. As test 2 but `PassThrough=no`, `ThroughVehicles=no`, with an enemy vehicle at 5 cells and the target at 10. Expected: the shell detonates on the 5-cell vehicle. With `ThroughVehicles=yes` (default) it passes over the vehicle and hits the target. Edge: place a friendly vehicle at 5 cells with `ThroughVehicles=no`; the shell passes it.
4. Edge attenuation. `EdgeAttenuation=0.5`, weapon `Range=10`, `Damage=100`, target at 2, 5 and 10 cells (three runs). Expected damage: about 94, 77 and 50 (the factor is 1 - 0.5 x (distance - 256) / (2560 - 256), truncated; the muzzle offset moves each by a point or two). Edge: `EdgeAttenuation=1.0` with `ProximityDamage=100` and an elite firer with a firepower bonus: the proximity hit is exactly 100 (the multiplier is skipped); with `EdgeAttenuation=0.99` it is raised by the bonus (open question 2).
5. Lead time. Straight shell, `LeadTimeCalculate=yes`, speed 40. A target moving straight toward the firer: the shell hits. A target moving straight away at about 60% of the shell speed (use a fast unit): record whether the shell lands behind it (open question 1). Compare with `LeadTimeCalculate=no`, which always misses the moving target.
6. Burst fan. Weapon `Burst=5`, Straight, `RotateCoord=20`, `MirrorCoord=no`, target 10 cells away. Expected: five shells spread over 20 degrees (from -10 to +10) with equal spacing. With `MirrorCoord=yes` the spread alternates sides and the outermost shells reach about +/-10 degrees. Edge: `Burst=1`: no fan.
7. Bombard. Artillery, `Trajectory=Bombard`, `Speed=60`, `Height=2500`, `FallPercent=0.5`, `FreeFallOnTarget=yes`. Expected: the shell climbs toward a point half way to the target, vanishes and reappears above the target, and falls straight down onto it. With `FreeFallOnTarget=no` it turns at the same point and dives onto the target at `FallSpeed`. Edge: `NoLaunch=yes`: the shell appears above the target without leaving the muzzle (the firing animation still plays). Edge: `FallScatter.Max=3`: the turning point differs from shot to shot, impact stays on the target.
8. Bombard random shift in multiplayer. Two machines, 8 firers on `Bombard` with `FallPercentShift=0.2`, `FallScatter.Max=3` and `Inaccurate=yes`, firing at moving targets for 5 minutes. Expected: no desync report. Repeat after saving and loading in mid-fire.
9. Parabola height mode. `Trajectory=Parabola`, `OpenFireMode=Height`, `ThrowHeight=600`. Fire at targets 3 and 9 cells away on flat ground, same firer. Expected: both shells take the same time to land (about 2 x sqrt(1200 / gravity) frames, 28 at gravity 6, within one frame), and both peak 600 leptons above launch height. In `Speed` mode (speed 30) the time scales with distance. Edge: `ThrowHeight=0`: acts as 600.
10. Parabola bounce. `Speed` mode, `BounceTimes=2`, `BounceDetonate=yes`, `BounceAttenuation=0.5`, `BounceCoefficient=0.5`, weapon `Damage=100`, warhead `CellSpread=10`, `PercentAtMax=1.0`, target at 6 cells. Expected: the target loses 100 + 50 + 25 = 175 over three detonations (two bounces and the final). With `BounceTimes=0` it loses 100. Edge: fire at a target across water with `BounceOnWater=no`: the shell detonates on the water at the first touch; with `yes` it skips on.
11. Interception. Interceptor `Interceptor=yes`, `Interceptor.GuardRange=12`, `Interceptor.Weapon=0`, its weapon `Damage=30`, projectile `Inviso=yes`, `AA=yes`, warhead `Verses=100%` for armor `light`. Enemy rocket: `ROT=10`, `Interceptable=yes`, `Armor=light`, `Strength=100`, fired across the interceptor's range at a target 12 cells beyond. Expected: the rocket detonates in the air near the interceptor after four hits (30 x 4 is 120), not at its target. `Interceptable.DeleteOnIntercept=yes`: it vanishes with no explosion. Stock check: remove `Interceptor` and the rocket reaches the target. Edge: remove `Armor` from the rocket: one hit kills it. Edge: warhead `Verses=0%` for `light`: the interceptor never fires.
12. Return weapon. Tank projectile `ReturnWeapon=BackW`, `BackW` has `Damage=40`, a fast projectile and a warhead with `AllowDamageOnSelf=yes`. Fire at a target 8 cells away. Expected: after the shell detonates a second shell flies from the impact point to the tank and the tank loses 40 (more with a firepower bonus when `ReturnWeapon.ApplyFirepowerMult=yes`). Edge: use infantry as the firer and kill it while the first shell flies: no return shell appears.
13. Parabomb. Aircraft with a bomb weapon, projectile `Dropping=yes`, `Parachuted=yes`, `Parachuted.FallRate=1`, `[General] ParachuteMaxFallRate=-3` and a visible `BombParachute`. Expected: the bomb descends under a parachute and takes noticeably longer than a stock bomb to land; the descent speed ramps for a few frames and then stays constant. Edge: `Parachuted.MaxFallRate=-12`: it falls faster. Edge: drop without `Dropping=yes` and record whether it explodes on landing (open question 6).
14. Land and water. Land artillery, projectile `Inviso=yes`, `SubjectToWater=yes`, a river 3 cells wide between it and the target. Expected: the unit will not fire from the bank and tries to move to a clear line. Without the key it fires across. Edge: `SubjectToWater.Detonate=no`: targeting is still blocked; a forced fire order, if the engine lets it through, passes the water.
15. Underground. A vehicle with the tunnel locomotor moves a long distance (tunnelling). A weapon with `AU=yes` on an `Inviso=yes` projectile and a warhead with `AffectsUnderground=yes`: auto-target picks the tunnelling unit and damages it. Without `AU` the unit is not targeted and a manual order to attack it is refused. Edge: warhead without `AffectsUnderground`: the unit is targeted but takes no damage.
16. Save and load. With 6 shells in flight (Straight with proximity, Bombard in each phase, Parabola with bounces, an intercepted missile), save, load and let them land. Expected: the same impact positions and frames as an unsaved run with the same random seed, which the AUTOTEST `detonate` lines show.
