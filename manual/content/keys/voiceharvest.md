---
key: VoiceHarvest
summary: "What a unit of this type says when the player orders it to harvest."
see_also: [VoiceMove]
when_omitted:
  kind: value
  value: none
---

When the player orders a unit of this type to harvest, it says this instead of its [`VoiceMove`](/keys/voicemove/).

```ini title="rulesmd.ini"
[MYHARVESTER] ; example VehicleType
VoiceHarvest=MyHarvesterHarvest
```
