---
key: Lightning.CloudHeight
scope: superweapontype
summary: "Sets the height of a lightning storm's clouds above the ground."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

The height in leptons at which each cloud hangs above the ground of its cell. A negative value hangs the cloud just high enough for the first bolt animation to reach the ground.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.CloudHeight=1200
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
