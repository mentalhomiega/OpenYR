---
key: CloakSound
scope: aircrafttype
label: Cloak sound of one type
see_also: ["system:cloaking", DecloakSound]
when_omitted:
  kind: inherited
  note: "[AudioVisual] CloakSound."
---

`CloakSound` on an object type names the sound that plays at an object of the type as it starts to hide. `none` makes that start silent. A type that also lacks [`DecloakSound`](/keys/decloaksound/#scope-aircrafttype) plays the same sound as it reappears; a type that sets neither plays the global sound for both.

```ini title="rulesmd.ini"
[SUB] ; example VehicleType
Cloakable=yes
CloakSound=none ; hides in silence
DecloakSound=NavalUnitEmerge ; and surfaces with a sound
```
