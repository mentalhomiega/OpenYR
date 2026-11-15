---
key: Text.Preparing
scope: superweapontype
summary: "Reads a caption label for the weapon; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

The value is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Text.Preparing=NAME:ACTIVATE
```
