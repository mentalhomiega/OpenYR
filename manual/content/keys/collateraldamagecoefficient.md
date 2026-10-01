---
key: CollateralDamageCoefficient
summary: "Parsed collateral damage multiplier that no death blast uses any more."
no_effect: true
see_also: [Explodes, DeathWeapon]
when_omitted:
  kind: context-dependent
  note: "`1` for an AircraftType, BuildingType or UnitType and `0.66` for an InfantryType, or `0.33` for a `Cyborg=yes` one."
---

The value is read but changes nothing. A dying [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) object sets off its [`DeathWeapon`](/keys/deathweapon/#scope-aircrafttype) instead of a blast sized by this figure.
