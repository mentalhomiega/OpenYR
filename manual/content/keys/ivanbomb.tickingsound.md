---
key: IvanBomb.TickingSound
summary: "The sound a bomb this weapon plants makes while it counts down."
see_also: [IvanBomb, "system:ivan-bombs", BombTickingSound]
when_omitted:
  kind: context-dependent
  note: "`BombTickingSound` from `[AudioVisual]`."
---

The sound repeats at the object while the bomb is on it, where the player who planted the bomb can hear it. A weapon that does not write it plants a bomb that makes `BombTickingSound`. A sound name that matches no sound plays nothing.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.TickingSound=MyBombTick
```
