---
key: YuriMindControlSound
summary: "The sound played at an object as a mind control weapon takes it."
see_also: [MindClearedSound, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Plays at each object a [mind control](/systems/mind-control/#taking-an-object-over) weapon takes, when the firer's house or the object's former house is the player's.

```ini title="rulesmd.ini"
[AudioVisual]
YuriMindControlSound=MyMindControl ; a sound ID registered in SOUNDMD.INI
```
