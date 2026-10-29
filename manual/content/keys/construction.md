---
key: Construction
summary: The sound a structure makes as its construction begins, when its type has no BuildupSound.
see_also: [BuildupSound, BuildingSlam, BuildingDrop, CrumbleSound]
when_omitted:
  kind: value
  value: none
---

A structure whose type sets no [`BuildupSound`](/keys/buildupsound/) plays this sound at its position when its construction animation begins. With neither set, the structure is silent. Placing a structure plays [`BuildingSlam`](/keys/buildingslam/), and a vehicle deploying into one plays [`BuildingDrop`](/keys/buildingdrop/).

```ini title="rulesmd.ini"
[AudioVisual]
Construction=BuildingGoingUp ; a sound ID registered in SOUND.INI
```
