---
key: VoiceIFVRepair
summary: "The voice the IFV answers an order to repair with."
when_omitted:
  kind: value
  value: none
---

When the player orders the IFV, the vehicle whose type is `FV`, to act on a target with a weapon that repairs, such as the one it gets with an engineer aboard, it answers with this voice instead of its weapon's attack voice. Other types never use it.

```ini title="rulesmd.ini"
[AudioVisual]
VoiceIFVRepair=IFVMove
```
