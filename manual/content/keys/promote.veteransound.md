---
key: Promote.VeteranSound
summary: The sound played at one of the player's objects of this type as it becomes a veteran.
see_also: [Promote.EliteSound, UpgradeVeteranSound, "system:veterancy"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] UpgradeVeteranSound, which is none when it is also absent."
---

This sound replaces [`UpgradeVeteranSound`](/keys/upgradeveteransound/) for the type. It plays at the object's position when its rank rises to veteran, and only for the player who owns it. An object created as a veteran plays nothing. [Veterancy](/systems/veterancy/#when-a-promotion-takes-effect) covers what else a promotion announces.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType
Promote.VeteranSound=MyPromotion ; a sound ID registered in SOUNDMD.INI
```
