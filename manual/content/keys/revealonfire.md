---
key: RevealOnFire
summary: "With no, firing the weapon does not reveal the firer to the target's owner."
see_also: ["system:map-visibility"]
when_omitted:
  kind: value
  value: "yes"
---

A shot from a `RevealOnFire=no` weapon does not uncover the ground around a hidden firer for the player it hits, as [firing reveals](/systems/map-visibility/#who-looks-and-when) otherwise do, so a sniper can shoot from the dark.

```ini title="rulesmd.ini"
[MySniperRifle] ; example Weapon
RevealOnFire=no
```
