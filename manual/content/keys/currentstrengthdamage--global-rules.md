---
key: CurrentStrengthDamage
scope: global-rules
label: Falling damage from current strength
see_also: [FallingDamageMultiplier]
when_omitted:
  kind: value
  value: "no"
  note: The stock rulesmd.ini sets yes in [CombatDamage].
---

Read from `[CombatDamage]`. When a Magnetron lets go of a vehicle and it lands somewhere it cannot stay (on a structure, on another vehicle, or a ship on dry land), the vehicle explodes. Each of its two explosions does damage equal to the vehicle's strength times [`FallingDamageMultiplier`](/keys/fallingdamagemultiplier/#scope-global-rules).

With `yes`, the vehicle's strength at the moment it lands is used, so a damaged vehicle does less harm. With `no`, the full `Strength=` of its type is used. A Rhino Tank at full health (`Strength=400`) dropped on a structure deals 400 with the Crush warhead and then 400 with the C4 warhead.
