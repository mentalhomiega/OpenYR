---
key: UpgradeVeteranSound
summary: The sound played at one of the player's objects as it becomes a veteran.
see_also: [UpgradeEliteSound, EliteFlashTimer, "system:veterancy"]
when_omitted:
  kind: value
  value: none
---

Plays at the object's position, and only for the player who owns it. An object created as a veteran plays nothing. [Veterancy](/systems/veterancy/#when-a-promotion-takes-effect) covers what else a promotion announces.

```ini title="rulesmd.ini"
[AudioVisual]
UpgradeVeteranSound=MyPromotion ; a sound ID registered in SOUNDMD.INI
```
