---
key: DropPod.Veterancy
scope: superweapontype
summary: "Sets the rank the pod infantry land with."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "2.0"
---

A value of 2 or more lands elite infantry, from 1 up to 2 veteran infantry, and below 1 rookies.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
DropPod.Veterancy=1.0
```

See [Drop pods](/systems/superweapons/#drop-pods).
