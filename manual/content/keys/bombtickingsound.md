---
key: BombTickingSound
summary: "The sound an Ivan bomb ticks with until it goes off."
see_also: [BombAttachSound, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: none
---

Plays at an object carrying an [Ivan bomb](/systems/ivan-bombs/#the-countdown) the player planted, until the bomb goes off or is lost.

```ini title="rulesmd.ini"
[AudioVisual]
BombTickingSound=MyBombTick ; a sound ID registered in SOUNDMD.INI
```
