---
key: VoiceDeploy
summary: "The voice an object answers a deploy order with."
see_also: [VoiceUndeploy, DeploySound]
when_omitted:
  kind: value
  value: none
---

When the player orders an object to deploy by clicking it, it answers with this voice instead of its move voice. A soldier that is already dug in answers with [`VoiceUndeploy`](/keys/voiceundeploy/) instead. A structure with [`UndeploysInto`](/keys/undeploysinto/) also plays this voice for its owner as it turns back into its vehicle. With no voice set, the order gets no answer.

```ini title="rulesmd.ini"
[SMIN] ; Slave Miner
VoiceDeploy=SlaveMinerDeployVoice
```
