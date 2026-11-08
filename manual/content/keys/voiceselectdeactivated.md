---
key: VoiceSelectDeactivated
summary: The responses an object speaks when it is selected while it is shut down.
see_also: [VoiceSelect, PoweredUnit, DeactivateSound]
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[ROBO] ; a UnitType registered in [VehicleTypes]
VoiceSelectDeactivated=RobotTankDeactivatedSelect ; sound IDs registered in SOUND.INI
```

An object that is shut down speaks one entry of this list when the player selects it, instead of its [`VoiceSelect`](/keys/voiceselect/) list. A [`PoweredUnit=yes`](/keys/poweredunit/) object is shut down while its owner has no working control structure for it. A type with no list speaks nothing while it is shut down. A slave of a Slave Miner speaks its `VoiceSelectEnslaved` list instead.

Names are matched as described in [Writing the list](/keys/voiceselect/#writing-the-list).
