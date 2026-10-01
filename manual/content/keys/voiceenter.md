---
key: VoiceEnter
summary: "What a unit of this type says when the player orders it into a building or transport."
see_also: [VoiceCapture, VoiceMove]
when_omitted:
  kind: value
  value: none
---

When the player orders a unit of this type to enter something, it says this instead of its [`VoiceMove`](/keys/voicemove/). A capture order also uses it when the type has no [`VoiceCapture`](/keys/voicecapture/).

```ini title="rulesmd.ini"
[MYENGINEER] ; example InfantryType
VoiceEnter=MyEngineerEnter
```
