---
key: ParaDrop.Types
scope: housetype
label: Country paradrop types
summary: "Lists the types a paradrop drops."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The country's side lists."
---

A list of infantry and vehicle types that a paradrop weapon drops for a house of this country. Used with [`ParaDrop.Num=`](/keys/paradrop.num/), which needs one count for each type; a country that sets one without the other falls back for both. A plane takes this list when its weapon names none for it, before the side's list. Only infantry types are dropped by a plane at present. This is an Ares key.

The country that counts is the one the house plays.

```ini title="rulesmd.ini"
[Americans] ; example country
ParaDrop.Types=E1,GHOST
ParaDrop.Num=4,2
```
