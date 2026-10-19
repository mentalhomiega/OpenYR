---
key: SlamSound
summary: The sound played to the owner when a structure of this type is placed on the map.
see_also: [BuildingSlam, "system:production"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] BuildingSlam."
---

`SlamSound` replaces [`BuildingSlam`](/keys/buildingslam/) for a structure of this type. `none` plays nothing. Only the player who places the structure hears it.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
SlamSound=PLACE2 ; a sound ID registered in SOUND.INI
```
