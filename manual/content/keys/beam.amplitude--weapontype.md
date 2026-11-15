---
key: Beam.Amplitude
scope: weapontype
summary: "How high the radiation beam of this weapon arches."
see_also: ["Beam.Duration", "Beam.Color", IsRadBeam, IsRadEruption]
when_omitted:
  kind: value
  value: "40 for a beam, and a random 100 to 500 for each arc of an eruption"
---

The key applies to a weapon with `IsRadBeam=yes` or `IsRadEruption=yes`. A larger value makes a taller arch.

```ini title="rulesmd.ini"
[MyRadWeapon] ; example WeaponType
IsRadBeam=yes
Beam.Amplitude=120
```
