---
key: ParaDrop.Num
scope: side
label: Side paradrop counts
summary: "Gives the count of each paradrop type."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's `[General]` counts: `AllyParaDropNum`, `SovParaDropNum` or `YuriParaDropNum`."
---

A list of counts, one for each type in [`ParaDrop.Types=`](/keys/paradrop.types/). This is an Ares key.

```ini title="rulesmd.ini"
[GDI] ; example side
ParaDrop.Types=E1
ParaDrop.Num=6
```
