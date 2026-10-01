---
key: DetectDisguiseRange
summary: "How far a DetectDisguise structure shows its owner disguised objects as they are, in cells."
see_also: [DetectDisguise, "system:disguises"]
when_omitted:
  kind: value
  value: "0"
---

A [`DetectDisguise=yes`](/keys/detectdisguise/) structure [shows its owner](/systems/disguises/#structures-that-see-through-disguises) the disguised objects whose cell is less than this many cells from its center as they are. With `0`, it shows none.

```ini title="rulesmd.ini"
[MYSENSOR] ; example BuildingType
DetectDisguise=yes
DetectDisguiseRange=8
```
