---
key: EVA.Tag
summary: The announcer voice a side hears, named by one of the voices EVAMD.INI offers.
see_also: ["system:eva-speech"]
when_omitted:
  kind: computed
  note: The `Russian` voice for the second side in `[Sides]`, `Yuri` for the third and `Allied` for every other side.
---

The value names a voice, which is one of `Allied`, `Russian` and `Yuri`, or a name listed under `[EVATypes]` in the [EVA file](/formats/eva-ini/). Each announcer line then plays the sample its section gives under that name. `none` silences the announcer for the side. A name that no voice has leaves the announcer silent too, and so does a line whose section gives no sample under the voice's name.

```ini title="rules.ini"
[FourthSide]
EVA.Tag=Foehn
```

The voice is chosen when the side's speech archives are mounted at the start of a match, from the side the local player plays. A campaign mission's `SpeechSide` picks the side for this purpose instead.
