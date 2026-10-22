---
key: FallingDamageMultiplier
scope: global-rules
label: Falling damage multiplier
see_also: [CurrentStrengthDamage]
when_omitted:
  kind: value
  value: "1.0"
  note: The stock rulesmd.ini also sets 1.0 in [CombatDamage].
---

Read from `[CombatDamage]`. When a Magnetron lets go of a vehicle and it lands somewhere it cannot stay (on a structure, on another vehicle, or a ship on dry land), the vehicle explodes twice where it lands, first with the Crush warhead and then with the C4 warhead. Each explosion does the vehicle's strength times this number. [`CurrentStrengthDamage`](/keys/currentstrengthdamage/#scope-global-rules) picks whether that strength is the vehicle's current strength or the full `Strength=` of its type.

With `0.25`, a full-health Rhino Tank (`Strength=400`) dropped on a structure deals 100 per explosion.
