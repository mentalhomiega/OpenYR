---
key: SW.Animation
scope: superweapontype
summary: "Plays an animation on the target cell when the effect starts."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

Every machine creates the animation, so every player sees it. [`SW.AnimationHeight=`](/keys/sw.animationheight/) sets how high above the ground it plays. An iron curtain with this key set skips `IronCurtainInvokeAnim`, and a genetic mutator skips `IonBlast`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Animation=FLARERED
```

See [Presentation](/systems/superweapons/#presentation).
