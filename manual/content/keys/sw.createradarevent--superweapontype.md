---
key: SW.CreateRadarEvent
scope: superweapontype
summary: "Flashes a superweapon event on every player's radar at the target."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the effect adds a superweapon radar event at the target cell when it starts.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.CreateRadarEvent=yes
```

See [Presentation](/systems/superweapons/#presentation).
