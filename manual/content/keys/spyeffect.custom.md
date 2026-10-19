---
key: SpyEffect.Custom
summary: With yes, a spy that infiltrates this structure does what the other SpyEffect keys say, instead of the stock effect.
see_also: ["SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyAmount", "SpyEffect.StolenTechIndex", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

`SpyEffect.Custom=yes` replaces what the structure does when a soldier with [`Agent=yes`](/keys/agent/) infiltrates it. The stock effect, chosen by the kind of structure, is skipped. The structure instead applies each [`SpyEffect.`](/keys/spyeffect.resetradar/) key it sets, and every other `SpyEffect.` key does nothing without this one.

A player on either side hears the line of each effect that fires, and `EVA_BuildingInfiltrated` when none has a line of its own. An effect that the structure does not set does nothing.

The `SpyEffect.` keys that reveal the owner's radar or production, grant a super weapon or demolish the structure after a delay are not read.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
SpyEffect.Custom=yes
SpyEffect.PowerOutageDuration=1800
SpyEffect.StolenMoneyAmount=1000
```
