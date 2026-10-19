---
key: SpyPlane.Count
scope: superweapontype
summary: "Sets how many aircraft a spy plane weapon sends."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

Used only when [`SpyPlane.Type=`](/keys/spyplane.type/) is set.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SpyPlane.Count=2
```

See [Spy plane](/systems/superweapons/#spy-plane).
