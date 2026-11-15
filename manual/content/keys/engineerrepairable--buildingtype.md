---
key: EngineerRepairable
scope: buildingtype
summary: "With no, an engineer cannot restore this structure to full strength."
see_also: [Repairable, ClickRepairable, Engineer]
when_omitted:
  kind: inherited
  note: The value of Repairable.
---

An engineer shows no repair cursor over a friendly `EngineerRepairable=no` structure, and a click there does not send it in. An engineer that is ordered in anyway restores nothing. The structure can still be repaired with the repair tool or by its owner's repair mode, and an engineer still captures an enemy structure that can be captured.

```ini title="rulesmd.ini"
[GAYARD] ; example BuildingType
EngineerRepairable=no
```
