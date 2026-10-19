---
key: Beam.Duration
scope: weapontype
summary: "How many frames the radiation beam of this weapon shows."
see_also: ["Beam.Amplitude", "Beam.Color", IsRadBeam, IsRadEruption]
when_omitted:
  kind: value
  value: "15 frames for a beam, and a random 5 to 20 frames for each arc of an eruption"
---

The key applies to a weapon with `IsRadBeam=yes` or `IsRadEruption=yes`. The value is in frames.

```ini title="rulesmd.ini"
[MyRadWeapon] ; example WeaponType
IsRadBeam=yes
Beam.Duration=45
```
