---
key: Hospital
summary: Admits one damaged infantry at a time and heals it to full strength.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "no"
---

A hospital heals damaged infantry free of charge, one at a time. With the player's infantry selected, pointing at an allied hospital shows the enter cursor when the infantry is below full strength and **all of** the following hold:

- the hospital has finished construction and is not being sold;
- it is switched on;
- it is not already treating another infantry;
- its [`Ammo`](/keys/ammo/) count is not zero;
- the infantry neither holds a mind-controlled unit nor is mind controlled itself.

Otherwise the cursor shows that the infantry cannot enter.

Each admission uses one point of `Ammo`, unless the type's `Ammo` is `-1`. Most structures refill an empty `Ammo` count at once. A hospital never does, and neither does an [`Armory=yes`](/keys/armory/) structure, so each point of a limited count is one patient.

The patient gains [`IRepairStep`](/keys/irepairstep/) strength each time the [`IRepairRate`](/keys/irepairrate/) interval passes, and leaves once it reaches full strength. A patient that is already at full strength when the first interval passes leaves at once, and its admission point is still spent.

:::note[An unlimited hospital takes every patient]
A type that sets no `Ammo` starts with a count of `-1`. The hospital never spends that count, so it admits any number of patients. Give the type an explicit `Ammo` count to limit the admissions.
:::
