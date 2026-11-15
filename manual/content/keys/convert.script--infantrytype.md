---
key: Convert.Script
scope: infantrytype
label: Infantry conversion by script
see_also: [Locomotor]
when_omitted:
  kind: value
  value: "none, so the soldier keeps its type on the script line"
---

`Convert.Script` is an Ares key. When a team runs the [Convert type](/mapping/missions/tmission-convert-type/) script line, each soldier in it whose type sets the key becomes the named infantry type where it stands. It keeps what a converted vehicle keeps: owner, rank, mission, team, selection, place and the same share of health, with ammo above the new type's maximum cut. A type with another [`Locomotor`](/keys/locomotor/) is refused, and a type that names itself does nothing. A team's aircraft are not converted.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
Convert.Script=GGI
```
