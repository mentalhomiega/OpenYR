---
key: UpgradeEliteSound
summary: The sound played at one of the player's objects as it becomes elite.
see_also: [UpgradeVeteranSound, Promote.EliteSound, EliteFlashTimer, "system:veterancy"]
when_omitted:
  kind: value
  value: none
---

A type's [`Promote.EliteSound`](/keys/promote.elitesound/) replaces it for that type. Plays at the object's position, and only for the player who owns it, whether it was a veteran or a rookie before. An object created elite plays nothing. [Veterancy](/systems/veterancy/#when-a-promotion-takes-effect) covers what else a promotion announces.

```ini title="rulesmd.ini"
[AudioVisual]
UpgradeEliteSound=MyElitePromotion ; a sound ID registered in SOUNDMD.INI
```
