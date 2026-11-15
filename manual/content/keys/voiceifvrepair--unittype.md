---
key: VoiceIFVRepair
scope: unittype
summary: "The voice a vehicle of this type answers an order to repair with."
see_also: [VoiceAttack]
when_omitted:
  kind: inherited
  note: "[AudioVisual] VoiceIFVRepair for the FV type; VoiceAttack for other types."
---

`VoiceIFVRepair` on a vehicle type is an Ares key. When the player orders a vehicle of this type to act on a target with a weapon that repairs, such as the one an IFV gets with an engineer aboard, it answers with this voice instead of its weapon's attack voice. Orders its other weapons carry out keep their usual answers. With the key set to nothing, a repair order is answered with its weapon's attack voice or [`VoiceAttack`](/keys/voiceattack/), even for the IFV.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
VoiceIFVRepair=IFVMove
```
