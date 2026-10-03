---
key: TurretRotateSound
summary: "The sound an object's turret makes while it turns."
when_omitted:
  kind: value
  value: none
---

An object with a turret plays this sound at its position when its turret starts to turn, and stops it when the turret stops. A sound that loops keeps playing for as long as the turret turns. The key has no effect on a type without a turret.

```ini title="rulesmd.ini"
[GTGCAN] ; Grand Cannon
TurretRotateSound=GrandCannonRotate
```
