---
key: SpyEffect.RevealRadar
summary: "With yes, a spy entering this radar structure makes the owner's units reveal the map to the spy's side."
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", Radar, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

`SpyEffect.RevealRadar` is an Ares key. It works only with [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/) on a [`Radar=yes`](/keys/radar/) structure. After a spy enters the structure, every vehicle, infantryman and aircraft of the structure's owner reveals the map around it to the spy's side as it moves, as far as its own sight reaches. A unit that stands still reveals nothing new, and the owner's structures reveal nothing to the spy's side.

The reveal lasts while the structure stands with the same owner. It ends when the structure is destroyed or sold, or changes owner by any means. Cells already revealed stay revealed.

Like a spy's other effects, the reveal counts for every player of the spy's country.

```ini title="rulesmd.ini"
[MYRADAR] ; example BuildingType
Radar=yes
SpyEffect.Custom=yes
SpyEffect.RevealRadar=yes
```
