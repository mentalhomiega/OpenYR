# Prism forwarding (Ares)

Source: https://ares-developers.github.io/Ares-docs/new/buildings/prismforwarding.html

Not implemented. This file records what the Ares prism network needs from our engine, so the work can be planned. The key list with defaults is in `docs/research/ares-tags.md` (the `PrismForwarding.*` rows).

## What we have

`BuildingClass::Prism_Charge` and `BuildingClass::Prism_AI` in `code/building.cpp` reproduce the stock network (0x44ACF0, 0x4503F0). Only the `[General] PrismType` building takes part. A charging tower recruits, one per call, the nearest of its owner's rested prism towers within its Secondary weapon's range, up to `PrismSupportMax`. Every supporter beams the firing tower directly: there are no chains. The firing shot gains `PrismSupportModifier` percent per supporter. A supporter draws a house-coloured laser 3 pixels thick for `PrismSupportDuration` frames and rests for `PrismSupportDelay` frames. The building keeps `PrismStage`, `PrismDelay`, `PrismTargetCoord` and `SupportingPrisms`, all saved.

## What Ares adds

- Per-type roles: `PrismForwarding=yes|no|forward|attack`. Any building type can join, not only `PrismType`.
- Which types a tower may support (`PrismForwarding.Targets`, default its own type) and whether it supports allied towers (`ToAllies`; in a chain it must be allied to the firing tower and to the tower it feeds).
- Chains: a supporter can itself be fed (`MaxChainLength`, default 1, which is the stock shape), with `MaxFeeds` direct feeders per tower and `MaxNetworkSize` towers in all, read from the firing tower only.
- Damage: `DamageAdd` per tower added to the shot before the multiplier, and a per-tower `SupportModifier` added linearly to a 100% base.
- Timing: `ChargeDelay` frames added per chain link, so beams appear to travel back to front.
- The support beam from a `SupportWeapon` / `EliteSupportWeapon`: its `Range`, `MinimumRange`, `ROF` (the rest time), `Report`, and its laser, electric bolt or radiation beam settings, fired from `AlternateFLH0` / `AlternateFLH1`. The Secondary weapon is no longer read for the range.
- `Intensity`: beam thickness grows with the number of chains behind the firing tower.
- `BreakSupport`: a supporter may drop its job at the last moment and fire as a master without recharging.

## What the work needs

1. Building type: read the role, the target list and the nine numeric keys; save them; declare them as a table, since they share one shape.
2. Building state: replace the single `SupportingPrisms` count with a link to the tower fed and the list of feeders, the chain depth, and the network's accumulated `DamageAdd` and modifier. These are new saved members, so the save revision goes up.
3. Recruitment: a breadth-first walk from the firing tower. Each level enlists every eligible tower in range on the same frame (the page says "all adjacent slave towers are enlisted on the same frame"), within `MaxFeeds`, `MaxChainLength` and `MaxNetworkSize`.
4. Firing: the master's shot applies `DamageAdd` then the summed multiplier. Each supporter fires its support weapon's beam at the tower it feeds after its depth times `ChargeDelay`, and rests for that weapon's `ROF`.
5. Beams: draw lasers through `LaserDrawClass` as now, and electric bolts and radiation beams through the existing weapon effect code, with the `Intensity` thickness.
6. Cleanup: when any tower in a network dies, is sold, undeploys, is captured or loses power mid-charge, its links must be removed from the other towers so nothing points at a dead object.
7. Tests: a two-tower network with stock defaults must give the same damage and frames as today; a three-link chain; `ToAllies`; `DamageAdd` with a negative total; a tower removed mid-charge.

## Open questions

The Ares page leaves these open; each needs a black-box run with Ares before we settle it, or an INI key with a documented default.

- What happens to the network when a member dies, is sold or changes owner mid-charge. The page only warns that undeploying a charging tower can crash the game.
- The recruitment order in deep chains, and how `MaxFeeds` and `MaxChainLength` interact when both limit a tower.
- The points the support range is measured between, and whether a supporter needs anything in range of the target.
- Whether `ChargeDelay` adds up across overlapping chains.
- Whether the multiplier from several supporters is applied with the stock 8-bit fraction rounding.

Mental Omega sets these keys on two tower types. Until this is done, the one that is `PrismType` uses the stock network and the other takes no part in one.
