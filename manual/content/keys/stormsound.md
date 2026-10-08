---
key: StormSound
summary: "The sound played when a lightning storm breaks."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays once, at full volume, when a lightning storm breaks, provided [`LightningPrintText`](/keys/lightningprinttext/) is `yes`. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[AudioVisual]
StormSound=MyStormIntro ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored.
