---
key: SW.AIRequiresHouse
scope: superweapontype
summary: "Limits the cells the computer may aim this superweapon at by who owns their contents."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

A list of `owner`, `allies`, `team`, `enemies`, `others` (allies and enemies), `all` and `none`. When the key lists any house, the picked cell must hold an object of a listed relation to the firing house.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AIRequiresHouse=enemies
```

See [Ai targeting modes](/systems/superweapons/#ai-targeting-modes).
