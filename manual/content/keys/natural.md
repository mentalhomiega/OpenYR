---
key: Natural
summary: "Keeps objects of this type from firing at Unnatural ones."
see_also: [Unnatural]
when_omitted:
  kind: value
  value: "no"
---

A `Natural=yes` object never fires at an object whose type is [`Unnatural=yes`](/keys/unnatural/), such as an attack dog facing Yuri Prime.

```ini title="rulesmd.ini"
[MYDOG] ; example InfantryType
Natural=yes
```
