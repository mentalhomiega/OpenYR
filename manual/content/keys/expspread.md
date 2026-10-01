---
key: ExpSpread
summary: "Parsed death blast divisor that nothing uses any more."
see_also: ["Explodes", "CollateralDamageCoefficient", "Strength", "MaxDamage"]
when_omitted:
  kind: value
  value: ".5"
no_effect: true
---

The value is read but changes nothing. A dying [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) object sets off its [`DeathWeapon`](/keys/deathweapon/#scope-aircrafttype), whose reach comes from that weapon's warhead, instead of a blast sized by this figure.
