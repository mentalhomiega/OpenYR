---
key: ChargedAnimTime
summary: How many minutes of charge a superweapon building must have left before it switches to its charged animation.
see_also: ["SuperWeapon", "SuperAnim", "RechargeTime"]
when_omitted:
  kind: value
  value: "999"
---

`ChargedAnimTime` sets when a building that owns a superweapon changes the animation it shows for that weapon. While the weapon has more than this many minutes left to charge, the building plays its [`SuperAnim`](/keys/superanim/). Once it has no more than that many left, the building swaps to its [`SuperAnimTwo`](/keys/superanimtwo/), which plays through once. When the weapon is ready the building plays [`SuperAnimThree`](/keys/superanimthree/) for as long as it stays ready. After the weapon is fired and starts charging again, [`SuperAnimFour`](/keys/superanimfour/) plays through once and the first animation begins again.

A minute counts as 900 game frames. Without the key, the value is large enough that the building never changes its superweapon animation, and none of these four animations play.

```ini title="rules.ini"
[NAIRON]            ; a BuildingType registered in [BuildingTypes]
SuperWeapon=IronCurtainSpecial
ChargedAnimTime=1   ; switches to SuperAnimTwo in the last minute of charge
```
