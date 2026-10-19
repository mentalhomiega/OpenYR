---
key: DecloakSound
scope: aircrafttype
label: Decloak sound of one type
see_also: ["system:cloaking", CloakSound]
when_omitted:
  kind: inherited
  note: "The type's own CloakSound, then [AudioVisual] DecloakSound, then [AudioVisual] CloakSound."
---

`DecloakSound` on an object type names the sound that plays at an object of the type as it starts to reappear. `none` makes the reappearance silent. The sound applies whether the object decloaks by itself or leaves a cloak generator's field.

```ini title="rulesmd.ini"
[SUB] ; example VehicleType
Cloakable=yes
DecloakSound=NavalUnitEmerge
```
