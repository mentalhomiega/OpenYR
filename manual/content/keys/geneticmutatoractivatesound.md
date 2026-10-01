---
key: GeneticMutatorActivateSound
summary: "The sound played where a genetic mutator lands."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the target of a genetic mutator.

```ini title="rulesmd.ini"
[AudioVisual]
GeneticMutatorActivateSound=MyMutatorSound ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored.
