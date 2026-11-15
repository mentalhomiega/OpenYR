---
key: Dominator.FirstAnim
scope: superweapontype
summary: "Names the animation that plays over a psychic dominator's target before it fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] DominatorFirstAnim"
---

A `Type=PsychicDominator` weapon plays this animation over the target, and the blast goes off once it has played [`Dominator.FireAtPercentage=`](/keys/dominator.fireatpercentage/) percent of its frames. The weapon does nothing unless this animation and [`Dominator.SecondAnim=`](/keys/dominator.secondanim/) both resolve, from the weapon or from the rules.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.FirstAnim=PDFXCLD
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
