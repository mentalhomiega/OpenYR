---
key: Dominator.FirstAnimHeight
scope: superweapontype
summary: "Sets how high above the ground the first dominator animation plays."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

The height in leptons above the target cell at which [`Dominator.FirstAnim=`](/keys/dominator.firstanim/) plays. The rules animation plays on the ground.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.FirstAnimHeight=750
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
