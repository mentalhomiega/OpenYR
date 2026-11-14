---
key: ParaDrop.Num
scope: housetype
label: Country paradrop counts
summary: "Gives the count of each paradrop type."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The country's side counts."
---

A list of counts, one for each type in [`ParaDrop.Types=`](/keys/paradrop.types/). A plane takes this list together with the types, before the side's list. This is an Ares key.

```ini title="rulesmd.ini"
[Americans] ; example country
ParaDrop.Types=E1,GHOST
ParaDrop.Num=4,2
```
