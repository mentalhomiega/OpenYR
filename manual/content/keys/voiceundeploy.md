---
key: VoiceUndeploy
summary: "The voice a dug-in soldier answers a deploy order with."
see_also: [VoiceDeploy]
when_omitted:
  kind: value
  value: none
---

When the player clicks a soldier that is already dug in, which orders it to pack up, it answers with this voice. With no voice set, the order gets no answer.

```ini title="rulesmd.ini"
[MYGI] ; example InfantryType
VoiceUndeploy=MYGI_Packup
```
