---
key: Nuke.PsiWarning
scope: superweapontype
label: Target warning animation of a missile weapon
summary: "The animation that marks the target of this super weapon's missile, in place of PSIWARN."
see_also: [PsiWarning, "system:superweapons"]
when_omitted:
  kind: computed
  note: "The PSIWARN animation, when the rules load it."
---

`Nuke.PsiWarning` is an Ares key. When a silo begins a launch of this super weapon, or its missile drops from above without a silo, the named animation marks the target cell in place of `PSIWARN`, and it stays until the missile is gone. `none`, or a name that is not an animation type, places no mark. A super weapon without the key places `PSIWARN`, so stock rules mark targets as before.

Which players see the mark depends on the animation's [`PsiWarning=`](/keys/psiwarning/) key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Nuke.PsiWarning=none
```

See [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile).
