---
key: Dominator.FireAtPercentage
scope: superweapontype
summary: "Sets how far through its first animation a psychic dominator fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] DominatorFireAtPercentage"
---

The blast goes off once [`Dominator.FirstAnim=`](/keys/dominator.firstanim/) has played this percentage of its frames. Write a whole number, so that 20 means 20 percent.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.FireAtPercentage=95
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
