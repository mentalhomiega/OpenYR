---
key: Underwater
summary: "Marks the object as a submerged target for naval targeting."
see_also: [NavalTargeting]
when_omitted:
  kind: value
  value: "no"
---

An `Underwater=yes` object counts as a submerged target for the attacker's [`NavalTargeting`](/keys/navaltargeting/): some values pick a different weapon against it, or refuse to fire at it while it is cloaked.

```ini title="rulesmd.ini"
[SUB] ; Typhoon attack submarine
Underwater=yes
```
