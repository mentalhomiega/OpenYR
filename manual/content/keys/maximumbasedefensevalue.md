---
key: MaximumBaseDefenseValue
summary: Read with the AI settings but has no effect; defense values come from each BuildingType's own keys.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "60"
---

The game reads this key and nothing uses the value. A BuildingType's defense values are the numbers its [`AntiAirValue`](/keys/antiairvalue/), [`AntiArmorValue`](/keys/antiarmorvalue/) and [`AntiInfantryValue`](/keys/antiinfantryvalue/) set, and they are not capped. [Defense values](/systems/ai-base-building/#defense-values) describes how the computer uses them.
