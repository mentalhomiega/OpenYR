---
key: AttachEffect.TemporalHidesAnim
scope: warheadtype
label: Hide animation while warped
when_omitted:
  kind: value
  value: "no"
---

`AttachEffect.TemporalHidesAnim=yes` hides the [effect animation](/keys/attacheffect.animation/#scope-warheadtype) while a temporal weapon is warping the object out, and shows it again if the object is let go. With `no` the animation stays. The effect's duration pauses while the object is warped out either way.
