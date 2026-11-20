---
key: IRepairStep
summary: The strength one repair step restores to an infantry.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "1"
---

Infantry use this value in place of [`RepairStep`](/keys/repairstep/) for each healing step. It applies in [a hospital](/systems/repair/#hospitals-and-armories). A value below `1` counts as `1`, so `0` still heals one point a step.

A hospital charges nothing for a step, however large `IRepairStep` is.
