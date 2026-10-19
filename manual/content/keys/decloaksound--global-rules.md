---
key: DecloakSound
scope: global-rules
label: Default decloak sound
see_also: ["system:cloaking", CloakSound]
when_omitted:
  kind: inherited
  note: "[AudioVisual] CloakSound."
---

```ini title="rulesmd.ini"
[AudioVisual]
DecloakSound=NavalUnitEmerge
```

`DecloakSound` is the sound that plays at an object as it starts to reappear, for an object type that sets neither [`DecloakSound`](/keys/decloaksound/#scope-aircrafttype) nor [`CloakSound`](/keys/cloaksound/#scope-aircrafttype). `none` makes the reappearance silent. Without the key, [`CloakSound`](/keys/cloaksound/#scope-global-rules) plays for both directions.

A critically damaged object that gives up its cloak partway through hiding makes no sound, as [`CloakSound`](/keys/cloaksound/#scope-global-rules) describes.
