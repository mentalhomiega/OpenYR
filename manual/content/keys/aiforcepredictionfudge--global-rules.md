---
key: AIForcePredictionFudge
scope: global-rules
summary: "A list of whole numbers that Yuri's Revenge reads from the [AI] section and then never uses."
when_omitted:
  kind: value
  value: ""
---

`AIForcePredictionFudge` is a Yuri's Revenge key, read from `[AI]` as a comma-separated list of whole numbers. The game keeps the list and saves it with the game state, but nothing reads its entries. Changing it changes no behaviour in Yuri's Revenge, and none here.

```ini title="rulesmd.ini"
[AI]
AIForcePredictionFudge=5,25,80
```
