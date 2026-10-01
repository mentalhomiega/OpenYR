---
key: ControlledAnimationType
summary: "The animation over an object while a mind control weapon holds it."
see_also: [MindControlRingOffset, PermaControlledAnimationType, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Plays [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above the center of each object a [mind control](/systems/mind-control/) weapon takes, and follows it. Over a structure it plays the art `Height` in cell levels above the center instead. It is removed when the object is let go.

```ini title="rulesmd.ini"
[CombatDamage]
ControlledAnimationType=MYRING ; an AnimType registered in [Animations]
```

With the key unset, held objects show nothing.
