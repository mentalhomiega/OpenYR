---
key: EnterGrinderSound
scope: global-rules
label: Grinder sound
see_also: [Grinding, LeaveGrinderSound]
when_omitted:
  kind: value
  value: none
---

Plays at the position of each object a [`Grinding=yes`](/keys/grinding/) structure takes in.

```ini title="rulesmd.ini"
[AudioVisual]
EnterGrinderSound=MyGrinderSound ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored and the sound set earlier stays.
