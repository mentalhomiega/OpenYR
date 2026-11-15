---
key: SW.AIRequiresTarget
scope: superweapontype
summary: "Limits the cells the computer may aim this superweapon at by their contents."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

The list takes the names of [`SW.RequiresTarget=`](/keys/sw.requirestarget/) and applies to targets the computer, automatic fire and cameo-click targeting pick.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AIRequiresTarget=land
```

See [Ai targeting modes](/systems/superweapons/#ai-targeting-modes).
