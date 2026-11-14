---
key: ParaDrop.Types
scope: side
label: Side paradrop types
summary: "Lists the types a paradrop drops."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's `[General]` list: `AllyParaDropInf`, `SovParaDropInf` or `YuriParaDropInf`."
---

A list of infantry and vehicle types that a paradrop weapon drops for a house of this side whose country sets none. Used with [`ParaDrop.Num=`](/keys/paradrop.num/). Only infantry types are dropped by a plane at present. This is an Ares key.

```ini title="rulesmd.ini"
[GDI] ; example side
ParaDrop.Types=E1
ParaDrop.Num=6
```
