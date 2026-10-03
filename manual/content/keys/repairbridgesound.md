---
key: RepairBridgeSound
summary: "The sound played when an engineer repairs a bridge."
see_also: [BridgeRepairHut]
when_omitted:
  kind: value
  value: none
---

When an engineer enters a [`BridgeRepairHut=yes`](/keys/bridgerepairhut/) structure and repairs its bridge, this sound plays at the hut, whichever house owns the engineer.

```ini title="rulesmd.ini"
[AudioVisual]
RepairBridgeSound=BridgeRepaired
```
