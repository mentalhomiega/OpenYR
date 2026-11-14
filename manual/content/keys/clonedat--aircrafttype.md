---
key: ClonedAt
scope: aircrafttype
label: Cloned at these structures
see_also: [Cloneable, Cloning, CloningFacility, CloningFacilityIgnoresFactories]
when_omitted:
  kind: value
  value: "none"
---

`ClonedAt` is an Ares key that lists the structures that copy this infantryman or vehicle. Each time a factory of the owner releases one it has finished building, every working structure of that owner whose type is in the list releases one more of the same type, free of charge. Two listed structures release two copies. A copy that cannot get out of its structure is lost, with no refund.

A type with a list is no longer copied by [`Cloning=yes`](/keys/cloning/) or [`CloningFacility=yes`](/keys/cloningfacility/) structures unless they are in the list too. A listed structure is skipped when it is the factory that built the object, is out of the world or is dead. A listed structure that makes objects itself is also skipped while [`CloningFacilityIgnoresFactories`](/keys/cloningfacilityignoresfactories/) is `yes`, the default. [`Cloneable=no`](/keys/cloneable/) stops every copy, listed structures included.

Only infantry released by a barracks and vehicles released by a vehicle factory are copied. An aircraft type reads the key but is never copied.

```ini title="rulesmd.ini"
[MYSOLDIER] ; example InfantryType
ClonedAt=MYCLONEBAY,MYCLONEBAY2
```
