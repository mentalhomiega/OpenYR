---
key: OmniCrushResistant
summary: "Keeps an object from being flattened by an OmniCrusher vehicle."
see_also: [OmniCrusher, Crushable]
when_omitted:
  kind: value
  value: "no"
---

An [`OmniCrusher=yes`](/keys/omnicrusher/) vehicle does not flatten an object of this type, whatever the object's [`Crushable`](/keys/crushable/) flag says. Ordinary crushing still follows the object's `Crushable` flag.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
OmniCrushResistant=yes
```
