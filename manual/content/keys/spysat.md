---
key: SpySat
summary: Shows the structure's owner the whole map while the structure works.
see_also: [SpySatActivationSound, SpySatDeactivationSound, Powered, "system:map-visibility", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

While a `SpySat=yes` structure works, the shroud is lifted from every cell for its owner. When the owner's last working one is lost, stops being operational or is sold, the map goes dark again. [The spy satellite](/systems/map-visibility/#the-spy-satellite) gives the conditions.

```ini title="rulesmd.ini"
[MYUPLINK] ; example BuildingType
SpySat=yes
Powered=yes
```

The structure lifts the shroud only; with fog of war on, the fog stays.
