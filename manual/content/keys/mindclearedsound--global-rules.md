---
key: MindClearedSound
scope: global-rules
label: Default mind control release sound
see_also: [YuriMindControlSound, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Plays where an object is [let go](/systems/mind-control/#letting-go) by mind control, unless the object's type names its own [`MindClearedSound`](/keys/mindclearedsound/#scope-aircrafttype).

```ini title="rulesmd.ini"
[AudioVisual]
MindClearedSound=MyMindCleared ; a sound ID registered in SOUNDMD.INI
```
