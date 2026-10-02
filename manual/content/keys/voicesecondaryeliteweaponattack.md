---
key: VoiceSecondaryEliteWeaponAttack
summary: "The voice for an attack order the secondary weapon will carry out, for an elite object."
see_also: [VoiceAttack, VoicePrimaryWeaponAttack, VoicePrimaryEliteWeaponAttack, VoiceSecondaryWeaponAttack]
when_omitted:
  kind: value
  value: none
---

When the player orders an elite object of this type to attack a target it would attack with its secondary weapon, this voice answers instead of [`VoiceAttack`](/keys/voiceattack/). Without it, `VoiceAttack` answers as before. The weapon is the one the object would choose for that target, so a unit with a separate anti-air weapon can answer differently for aircraft.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
VoiceSecondaryEliteWeaponAttack=MyEliteRocketVoice
```
