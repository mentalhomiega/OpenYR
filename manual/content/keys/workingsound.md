---
key: WorkingSound
summary: "The sound played at a structure when it comes into service."
see_also: [NotWorkingSound, "system:power"]
when_omitted:
  kind: value
  value: none
---

When a structure of this type becomes [operational](/systems/power/#defenses) and is neither being built nor sold, after it was not, this sound plays at it. Placing a structure that has power counts, so the sound also plays once when it is finished.

```ini title="rulesmd.ini"
[MYDEFENSE] ; example BuildingType
WorkingSound=PowerOn
```
