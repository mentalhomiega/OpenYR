---
key: File.Taunt
summary: The sound files the country's taunt commands are meant to play, in Ares's naming.
see_also: ["system:skirmish-setup"]
when_omitted:
  kind: value
  value: none
---

The value is a path under the `Taunts` folder with `~~` standing for the taunt number, such as `Taunts\tauam~~.wav`, which names `tauam01.wav` to `tauam08.wav`. The game reads the key and plays nothing from it: the eight taunt commands report that taunts are not available in this build.

```ini title="rules.ini"
[UnitedStates]
File.Taunt=Taunts\tauam~~.wav
```
