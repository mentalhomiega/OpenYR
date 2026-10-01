---
key: PsychicDominatorActivateSound
summary: "The sound played where a psychic dominator is fired."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the target as a psychic dominator is fired, before its blast.

```ini title="rulesmd.ini"
[AudioVisual]
PsychicDominatorActivateSound=MyDominatorSound ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored.
