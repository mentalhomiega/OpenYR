---
key: SW.Unstoppable
scope: superweapontype
summary: "Reads a flag that is meant to keep a running weapon from being switched off; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

The value is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Unstoppable=yes
```
