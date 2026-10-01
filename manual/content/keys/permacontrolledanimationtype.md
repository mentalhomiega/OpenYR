---
key: PermaControlledAnimationType
summary: "The animation over a unit the psychic dominator has taken over."
see_also: [MindControlRingOffset, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays [`MindControlRingOffset`](/keys/mindcontrolringoffset/) leptons above each unit the psychic dominator takes over, and follows it.

```ini title="rulesmd.ini"
[CombatDamage]
PermaControlledAnimationType=MYRING ; an AnimType registered in [Animations]
```

With the key unset, dominated units show nothing.
