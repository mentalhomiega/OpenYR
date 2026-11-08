---
key: BuildingRepairedSound
summary: The sound played where an engineer restores a structure to full strength.
see_also: [Engineer, "system:capture"]
when_omitted:
  kind: value
  value: none
---

Plays at the structure's position when an [`Engineer=yes`](/keys/engineer/#scope-infantrytype) soldier [restores it](/systems/capture/), so it fades with distance from the view and is heard whichever house the engineer belongs to. A structure that is already at full strength when the engineer arrives plays nothing. A paid repair with the wrench does not play it.

```ini title="rulesmd.ini"
[AudioVisual]
BuildingRepairedSound=BuildingRepaired ; a sound ID registered in SOUNDMD.INI
```
