---
key: IvanBomb.AttachSound
summary: "The sound played when a bomb this weapon plants is attached."
see_also: [IvanBomb, "system:ivan-bombs", BombAttachSound]
when_omitted:
  kind: context-dependent
  note: "`BombAttachSound` from `[AudioVisual]`."
---

The sound plays at the target when a soldier of the player plants the bomb. A weapon that does not write it plants a bomb that makes `BombAttachSound`. A sound name that matches no sound plays nothing.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.AttachSound=MyBombAttachSound
```
