# Bounty (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/bounty.html.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `Bounty` | rules, TechnoType (the killer) | boolean | `no` |
| `Bounty.Display` | rules, TechnoType (the killer) | boolean | value of `[AudioVisual] BountyDisplay` |
| `Bounty.Value` | rules, TechnoType (the victim) | integer credits, may be negative | `0` |
| `Bounty.RookieValue` | TechnoType (victim) | integer | `Bounty.Value` |
| `Bounty.VeteranValue` | TechnoType (victim) | integer | `Bounty.Value` |
| `Bounty.EliteValue` | TechnoType (victim) | integer | `Bounty.Value` |
| `BountyEnablers` | rules, `[General]` | list of BuildingTypes | empty |
| `BountyDisplay` | rules, `[AudioVisual]` | boolean | `no` |
| `GivesBounty` | rules, Country section | boolean | `yes` |

## Behaviour

- A unit or structure with `Bounty=yes` earns credits when it destroys an enemy by firing or by crushing. The amount comes from the victim's `Bounty.Value`, chosen by the victim's rank at death (rookie, veteran, elite). If the rank-specific key is unset it falls back to `Bounty.Value`. The documentation says that setting `Bounty.Value` resets the rank-specific values, so when both appear in one section, treat `Bounty.Value` as the base and let explicit rank keys win (open question 1).
- A negative value takes credits from the killer.
- `BountyEnablers` gates the system per house. If the list is not empty, a house gets bounty only while it owns at least one building from the list. An empty list means always on.
- `GivesBounty=no` on a country makes that country's objects worth nothing when killed.
- Display: when the killer has display on (its own `Bounty.Display`, else the global `BountyDisplay`), the amount shows over the victim on death.

## What stock YR does

Nothing. Kills give score and veterancy only.

## Where it hooks in OpenYR

- Kill hook: `TechnoClass::Record_The_Kill(TechnoClass * source)` (`code/techno.cpp`). It runs for each death with `source`, applies `Veterancy.Made_A_Kill`, adds score, and uses `House->Is_Ally(source)`. The award fits here: test `source->TClass->Bounty`, enemy status, `GivesBounty` of the victim's house and `BountyEnablers`, then credit the killer's house (find the money-add function in `code/house.cpp`, for example the one used for refunds) and spawn the text.
- Crush kills: confirm in `code/unit.cpp` and `code/foot.cpp` that the crush path reaches `Record_The_Kill` with the crusher as `source`; if not, add the call.
- Type parse: the `TechnoTypeClass` INI read in `code/techtype.cpp` for `Bounty`, `Bounty.Display`, `Bounty.*Value`.
- Globals: `RulesClass` general and audiovisual read (`code/rules.cpp`) for `BountyEnablers` and `BountyDisplay`; `HouseTypeClass` read (`code/houstype.cpp`) for `GivesBounty`.
- Victim rank: `TechnoClass::Veterancy` with `Is_Veteran` and `Is_Elite` (`code/veteran.cpp`).
- Floating text: search `code/tactical.cpp` for the routine that draws credit pickups before writing a new one.
- Multiplayer sync: the award happens inside the damage path inside the logic frame, so it is deterministic if it uses no wall-clock or random input.

## Open questions

1. Order of `Bounty.Value` and rank values in one section (see above).
2. Whether bounty is paid when the kill comes from a passenger, a spawn or a slave rather than the unit itself.
3. Whether kills among allies or of the killer's own house are excluded. Assume only enemies; confirm by test.
4. Format and colour of the floating amount.
