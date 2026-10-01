---
key: VoiceCapture
summary: "What a unit of this type says when the player orders it to capture a structure."
see_also: [VoiceEnter, VoiceMove]
when_omitted:
  kind: value
  value: none
---

When the player orders a unit of this type to capture a structure, it says this. Without it, the unit uses its [`VoiceEnter`](/keys/voiceenter/), and without that its [`VoiceMove`](/keys/voicemove/).

```ini title="rulesmd.ini"
[MYENGINEER] ; example InfantryType
VoiceCapture=MyEngineerCapture
```
