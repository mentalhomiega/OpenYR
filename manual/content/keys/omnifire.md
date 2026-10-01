---
key: OmniFire
summary: "Lets a vehicle fire the weapon in any direction without turning to the target."
see_also: [Range]
when_omitted:
  kind: value
  value: "no"
---

A vehicle fires an `OmniFire=yes` weapon at once, whichever way its body or turret is facing, instead of [turning toward the target first](/systems/firing-geometry/#effects-that-hold-the-weapon-shut). Other kinds of objects ignore the key.

```ini title="rulesmd.ini"
[MyGasRelease] ; example Weapon
OmniFire=yes
```
