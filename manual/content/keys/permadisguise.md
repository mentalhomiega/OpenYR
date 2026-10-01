---
key: PermaDisguise
summary: "Keeps a CanDisguise object disguised all the time."
see_also: [CanDisguise, AlliedDisguise, "system:disguises"]
when_omitted:
  kind: value
  value: "no"
---

A [`CanDisguise=yes`](/keys/candisguise/) object of this type is [always disguised](/systems/disguises/): as its side's default soldier when it has copied no other, and damage does not strip the disguise.

```ini title="rulesmd.ini"
[MYSPY] ; example InfantryType
CanDisguise=yes
PermaDisguise=yes
```
