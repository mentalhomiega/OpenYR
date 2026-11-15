---
key: Dominator.ControlAnim
scope: superweapontype
summary: "Names the animation shown over a unit a psychic dominator takes over."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[CombatDamage] PermaControlledAnimationType"
---

Each unit the blast takes over carries this animation, drawn [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.ControlAnim=MINDANIMR
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
