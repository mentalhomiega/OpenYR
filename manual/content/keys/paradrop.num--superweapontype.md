---
key: ParaDrop.Num
scope: superweapontype
summary: "Lists how many of each type a paradrop plane carries."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's paradrop lists"
---

One count for each type in [`ParaDrop.Types=`](/keys/paradrop.types/). The lists are used only when their lengths match.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ParaDrop.Num=5,3
```

See [Paradrops](/systems/superweapons/#paradrops).
