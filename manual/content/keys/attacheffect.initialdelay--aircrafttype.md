---
key: AttachEffect.InitialDelay
scope: aircrafttype
label: Own effect first delay
see_also: ["system:attach-effects"]
when_omitted:
  kind: value
  value: "0"
  note: "The effect is attached as soon as the object enters the game."
---

`AttachEffect.InitialDelay` is how many frames after an object of this type enters the game its [own effect](/systems/attach-effects/#an-object-types-own-effect) is attached for the first time. Later repeats wait [`AttachEffect.Delay`](/keys/attacheffect.delay/) instead. A negative value counts as `0`.
