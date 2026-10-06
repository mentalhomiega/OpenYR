---
key: Suffix
scope: housetype
label: Country filename extension
summary: Parsed country filename extension that the engine never uses.
no_effect: true
see_also: [Prefix, Name]
when_omitted:
  kind: value
  value: ""
  note: No suffix is stored.
---

The value is stored on the country in full, up to 31 characters, so `Allied` and `Soviet` are kept whole. Nothing in the game reads it.

```ini title="rules.ini"
[GDI]
Suffix=GDI
```
