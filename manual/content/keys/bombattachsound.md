---
key: BombAttachSound
summary: "The sound played as an Ivan bomb is planted."
see_also: [BombTickingSound, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: none
---

Plays at the object when the player's soldier plants an [Ivan bomb](/systems/ivan-bombs/#the-countdown) on it.

```ini title="rulesmd.ini"
[AudioVisual]
BombAttachSound=MyBombAttach ; a sound ID registered in SOUNDMD.INI
```
