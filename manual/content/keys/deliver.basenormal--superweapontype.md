---
key: Deliver.BaseNormal
scope: superweapontype
summary: "Reads whether delivered structures count as part of the base; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

The value is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Deliver.BaseNormal=no
```
