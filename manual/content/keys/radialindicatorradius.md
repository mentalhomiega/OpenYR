---
key: RadialIndicatorRadius
summary: The radius in cells of the ring drawn around a selected object of this type.
see_also: [HasRadialIndicator, RadialColor, CloakRadiusInCells]
when_omitted:
  kind: computed
  note: For a sensor array or cloak generator, CloakRadiusInCells; otherwise no ring.
---

`RadialIndicatorRadius` sets the radius of the ring an object draws while it is selected and the type sets [`HasRadialIndicator=yes`](/keys/hasradialindicator/). It takes a number of cells, with a fraction allowed, and a value of `0` or less keeps the default. A structure with the key draws the ring when it is switched on, even if it is neither a sensor array nor a cloak generator. A vehicle, soldier or aircraft draws a plain ring around itself. The ring is drawn only for the player who owns the object, in the type's [`RadialColor`](/keys/radialcolor/), or in the owner's color when that is black.

```ini title="rulesmd.ini"
[GGI] ; example InfantryType
HasRadialIndicator=yes
RadialIndicatorRadius=6
```
