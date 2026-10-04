# AttachEffect (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/attacheffect.html. Ares calls it a spiritual successor to the NPatch upgrade logic.

Phobos has a different, richer system with the same name (explicit `AttachEffectTypes`, https://phobos.readthedocs.io/en/latest/New-or-Enhanced-Logics.html section "Attached Effects"). The two are not the same. This spec covers the Ares keys, which are the ones that appear directly on TechnoTypes and Warheads. A Phobos spec would be a separate, larger document.

## Keys

On a TechnoType or a WarheadType (in rules):

| Key | Type | Default |
|---|---|---|
| `AttachEffect.Duration` | integer frames; `-1` = never ends; `0` = no effect | `0` |
| `AttachEffect.Animation` | AnimationType | none |
| `AttachEffect.TemporalHidesAnim` | boolean | `no` |
| `AttachEffect.SpeedMultiplier` | float | `1.0` |
| `AttachEffect.ArmorMultiplier` | float | `1.0` |
| `AttachEffect.FirepowerMultiplier` | float | `1.0` |
| `AttachEffect.ROFMultiplier` | float | `1.0` |
| `AttachEffect.Cloakable` | boolean | `no` |
| `AttachEffect.ForceDecloak` | boolean | `no` |
| `AttachEffect.PenetratesIronCurtain` | boolean | `no` |
| `AttachEffect.DiscardOnEntry` | boolean | `no` |

TechnoType only (the unit puts the effect on itself):

| Key | Type | Default |
|---|---|---|
| `AttachEffect.Delay` | frames before the effect is created again after it ends; negative = never | `0` |
| `AttachEffect.InitialDelay` | frames before the first creation | `0` |

Warhead only:

| Key | Type | Default |
|---|---|---|
| `AttachEffect.Cumulative` | boolean; allow unlimited stacking | `no` |
| `AttachEffect.AnimResetOnReapply` | boolean; for non-stacking, restart the animation on each reapply | `no` |

## Behaviour

- An effect lives on a target for `Duration` frames. `-1` keeps it until the target dies or the effect is discarded.
- Two sources: a TechnoType puts its own effect on itself (use `Delay` and `InitialDelay` for re-creation), and a warhead puts it on every object it hits within its `CellSpread` (the normal warhead scope and `Verses` rules apply).
- Without `Cumulative`, a second hit does not add a second copy; it refreshes the single instance. With `Cumulative=yes` every hit adds an independent instance with its own timer, with no limit.
- The animation loops until the effect ends; its `LoopCount` is ignored. The animation is removed while the target is cloaked and comes back on uncloak. `TemporalHidesAnim=yes` also removes it while a temporal weapon is erasing the target.
- Multipliers apply while the effect is active:
  - `SpeedMultiplier` changes movement speed.
  - `ArmorMultiplier` changes how much damage the target takes (documented as an armor multiplier; the direction, whether `2.0` halves or doubles incoming damage, is not stated, see open questions).
  - `FirepowerMultiplier` scales outgoing damage.
  - `ROFMultiplier` scales rate of fire. The reload delay is computed once when reloading starts; it does not get shorter or longer when the effect ends, so a long reload begun under a slow effect outlasts it.
- `Cloakable` lets the target cloak while the effect lasts. `ForceDecloak` decloaks it on application.
- Iron Curtain: by default, an effect cannot attach to an object under the Iron Curtain or a force shield. `PenetratesIronCurtain=yes` lets it.
- `DiscardOnEntry=yes` removes the effect when the object leaves the map by entering a building or a transport.
- Attach effect state does not advance while the object is inside a transport, being erased by a temporal weapon, or similar. It behaves like an Ivan bomb.

## What stock YR does

Nothing equivalent. Stock has Iron Curtain, Force Shield, Berserk, and veteran bonuses. They are each separate hard coded states with their own timers.

## Where it hooks in OpenYR

This is a new runtime system, and it must be save and CRC aware:

- A per-techno list of active effects: (source key, remaining frames, owner house, animation pointer, multiplier snapshot). Add to `TechnoClass` (`code/techno.h`, `code/techno.cpp`) with `Serialize` support and a CRC contribution (`Compute_CRC`), so multiplayer stays in sync.
- Tick: `TechnoClass::AI` in `code/techno.cpp` decrements timers. Respect the transport, limbo and temporal cases above (check `IsInLimbo`, `Transporter`, temporal state in `code/temporal.cpp`).
- Warhead application: the explosion code (`BulletClass::Detonate` in `code/bullet.cpp`, plus the area damage in `code/combat.cpp`, function `Explosion_Damage` or the one the `Modify_Damage` caller sits in) applies the effect for each affected object after the `Verses` test.
- The engine already has per-object bias fields that the multipliers can drive, all saved and in the CRC: `FootClass::SpeedBias` (used in the speed computation in `code/foot.cpp`, `House->GroundspeedBias * SpeedBias`), `TechnoClass::ArmorBias` (`TechnoClass::Take_Damage` divides damage by `House->ArmorBias * ArmorBias`, so a larger value means more armor) and `TechnoClass::FirepowerBias` (multiplies `weapon->Attack` where weapons fire in `code/techno.cpp`). Rate of fire is computed in `TechnoClass::Rearm_Delay` as `weapon->ROF * House->ROFBias`; there is no per-object ROF bias yet, so add one. Because the biases are shared with other sources (Iron Curtain, veteran abilities, crates), do not overwrite them from the effect. Keep the effect multipliers in the effect list and multiply them in at these four places, or recompute the bias from all active sources whenever the list changes.
- `Rearm_Delay` fixes the delay at the time it is called, which matches the documented reload behaviour.
- Cloak: `Cloakable` and `ForceDecloak` hook into the cloak ability tests in `code/techno.cpp`.
- Animation: spawn an `AnimClass` (`code/anim.cpp`) attached to the target, looping, hidden while cloaked.
- Type read: `TechnoTypeClass` (`code/techtype.cpp`) and `WarheadTypeClass` (`code/warhead.cpp`).
- Ownership change (mind control, capture) keeps the effects; the documentation says nothing otherwise.

## Open questions

1. Direction of `ArmorMultiplier`: whether `2.0` means twice the armor (half damage) or twice the damage taken. The page says "multiplies the unit's armor", so likely half damage, which matches the way `ArmorBias` already works in the engine; test with Ares.
2. How several different sources with different multipliers combine (assume multiplicative, one per source).
3. What happens to an effect that has `Duration=-1` when the owner changes house.
4. Whether `Delay` counts from the moment the effect ends or from creation.
5. Whether more than one `AttachEffect.*` set can be given to one type. The page shows one set per section only.
