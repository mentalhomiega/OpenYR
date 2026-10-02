---
key: VoicePrimaryWeaponAttack
summary: "The voice for an attack order the primary weapon will carry out."
see_also: [VoiceAttack, VoicePrimaryEliteWeaponAttack, VoiceSecondaryWeaponAttack, VoiceSecondaryEliteWeaponAttack]
when_omitted:
  kind: value
  value: none
---

When the player orders an object that is not elite of this type to attack a target it would attack with its primary weapon, this voice answers instead of [`VoiceAttack`](/keys/voiceattack/). Without it, `VoiceAttack` answers as before. The weapon is the one the object would choose for that target, so a unit with a separate anti-air weapon can answer differently for aircraft.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
VoicePrimaryWeaponAttack=MyLaserVoice
```
