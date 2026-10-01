---
key: Agent
summary: Makes a soldier spy on the structure it walks into.
see_also: ["system:capture"]
when_omitted:
  kind: value
  value: "no"
---

A spy can be sent into another house's [`Spyable=yes`](/keys/spyable/) structure. When it walks in, the structure's owner loses something that depends on the structure, such as its power or its money, and the spy is consumed. [Infiltrating it](/systems/capture/#infiltrating-it) lists the effects.

Write [`Infiltrate=yes`](/keys/infiltrate/) in the same section. `Agent=yes` does not turn `Infiltrate` on, and without it the soldier cannot be sent into a structure at all.

A type that also sets [`Engineer=yes`](/keys/engineer/#scope-infantrytype) behaves as an engineer and never spies, because the engineer handling takes the arrival first.

A spy sent at another house's deployed vehicle takes that vehicle instead of spying on it, the same as any other soldier. [The vehicle branch](/systems/capture/#the-vehicle-branch) covers it.
