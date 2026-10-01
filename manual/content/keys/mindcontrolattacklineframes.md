---
key: MindControlAttackLineFrames
summary: "How many frames a mind control line shows after each capture."
see_also: [LeptonMindControlOffset, "system:mind-control"]
when_omitted:
  kind: value
  value: "0"
---

After a [mind control](/systems/mind-control/#taking-an-object-over) weapon takes an object, the line joining the firer to it shows for this many frames even when neither is selected.

```ini title="rulesmd.ini"
[CombatDamage]
MindControlAttackLineFrames=20
```
