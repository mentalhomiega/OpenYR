---
key: PrismSupportModifier
summary: "The extra damage each supporting prism tower adds to a shot, as a percent."
see_also: ["system:prism-towers"]
when_omitted:
  kind: value
  value: "100%"
---

A prism tower's shot deals this percent of its weapon's damage more [for each tower that supported it](/systems/prism-towers/#the-shot).

```ini title="rulesmd.ini"
[General]
PrismSupportModifier=150%
```
