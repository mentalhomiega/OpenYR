---
key: Unnatural
summary: "Keeps Natural objects from firing at objects of this type."
see_also: [Natural]
when_omitted:
  kind: value
  value: "no"
---

Objects whose type is [`Natural=yes`](/keys/natural/) never fire at an `Unnatural=yes` object.

```ini title="rulesmd.ini"
[MYPSYCHIC] ; example InfantryType
Unnatural=yes
```
