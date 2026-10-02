---
key: RevealToAll
summary: "Shows every player the area around this structure when it is placed."
see_also: [Sight]
when_omitted:
  kind: value
  value: "no"
---

When another house places a structure whose type sets `RevealToAll=yes`, the player sees the area around it, out to the structure's `Sight`, as if the player had a unit there for a moment. The stock rules set it on the superweapon structures. The reveal happens once, when the structure is placed; later fog of war covers the area again as usual.

```ini title="rulesmd.ini"
[MYSUPERWEAPON] ; example BuildingType
RevealToAll=yes
```
