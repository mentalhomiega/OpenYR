---
key: Deliver.Buildups
scope: superweapontype
summary: "Whether delivered structures play their construction animation."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a delivered structure builds up. Otherwise it appears complete.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Deliver.Buildups=yes
```

See [Unit delivery](/systems/superweapons/#unit-delivery).
