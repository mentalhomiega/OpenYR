---
key: EnterWaterSound
summary: "The sound played when an amphibious soldier walks from land into water."
see_also: [LeaveWaterSound, MovementZone, SpeedType]
when_omitted:
  kind: value
  value: none
---

This sound plays at an infantry soldier whose type sets [`MovementZone=AmphibiousDestroyer`](/keys/movementzone/) when it walks from land into water. Water and beach cells count as water, unless the soldier is on a bridge. Such a soldier in water also swaps its walking, standing, idle, firing and first two death sequences for its `Swim`, `Tread`, `WetIdle1`, `WetIdle2`, `WetAttack`, `WetDie1` and `WetDie2` art sequences, keeping the dry one for any sequence its art leaves empty. To reach water at all, the type also needs a [`SpeedType`](/keys/speedtype/) whose `[Water]` figure is above `0`, such as `Amphibious`.

```ini title="rulesmd.ini"
[MYSWIMMER] ; example InfantryType
SpeedType=Amphibious
MovementZone=AmphibiousDestroyer
EnterWaterSound=TanyaEntersWater
```
