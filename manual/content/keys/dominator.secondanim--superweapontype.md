---
key: Dominator.SecondAnim
scope: superweapontype
summary: "Names the animation that plays when a psychic dominator fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] DominatorSecondAnim"
---

A `Type=PsychicDominator` weapon plays this animation over the target when the blast goes off, and it sets how long the weapon stays busy afterwards.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.SecondAnim=DOMRING
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
