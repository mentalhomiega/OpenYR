---
key: AttachEffect.AnimResetOnReapply
scope: warheadtype
label: Restart animation on hit
see_also: ["system:attach-effects"]
when_omitted:
  kind: value
  value: "no"
---

`AttachEffect.AnimResetOnReapply=yes` restarts the [effect animation](/keys/attacheffect.animation/#scope-warheadtype) when a hit restarts an effect the object already carries. It has no use with [`AttachEffect.Cumulative=yes`](/keys/attacheffect.cumulative/), where every hit adds a copy with its own animation.
