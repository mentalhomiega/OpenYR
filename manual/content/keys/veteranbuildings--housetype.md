---
key: VeteranBuildings
scope: housetype
label: Country veteran structure types
summary: The structure types that a house of this country makes veterans when it creates them.
see_also: [VeteranInfantry, VeteranUnits, VeteranAircraft, "system:veterancy"]
when_omitted:
  kind: value
  value: ""
  note: No structure type is listed, so new structures start as rookies unless a spy or another rule promotes them.
---

`VeteranBuildings=` is an Ares key. It lists the structure types that a house of this country makes veterans when it creates them, as in `VeteranBuildings=GAPILL,NALASR`.

A house whose country lists a type makes each new structure of that type a veteran, whether it is built or placed by the map. The check does not look at [`Trainable=`](/keys/trainable/). Structures that already exist keep their rank, and a structure that changes owner keeps the rank it has.

For the local player, a listed type also shows its alternate cameo ([`AltCameo`](/keys/altcameo/) or [`AltCameoPCX`](/keys/altcameopcx/)) in the sidebar, when its art has one.
