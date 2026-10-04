---
key: Promote.EliteSound
summary: The sound played at one of the player's objects of this type as it becomes elite.
see_also: [Promote.VeteranSound, UpgradeEliteSound, "system:veterancy"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] UpgradeEliteSound, which is none when it is also absent."
---

This sound replaces [`UpgradeEliteSound`](/keys/upgradeelitesound/) for the type. It plays at the object's position when its rank rises to elite, and only for the player who owns it. An object created elite plays nothing. [Veterancy](/systems/veterancy/#when-a-promotion-takes-effect) covers what else a promotion announces.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType
Promote.EliteSound=MyElitePromotion ; a sound ID registered in SOUNDMD.INI
```
