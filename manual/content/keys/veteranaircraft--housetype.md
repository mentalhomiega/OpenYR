---
key: VeteranAircraft
scope: housetype
label: Country veteran aircraft types
summary: The aircraft types that a house of this country makes veterans when it creates them.
see_also: [VeteranInfantry, VeteranUnits, "system:veterancy"]
when_omitted:
  kind: value
  value: ""
  note: No aircraft type is listed, so new aircraft start as rookies.
---

`VeteranAircraft=` lists the aircraft types that a house of this country makes veterans when it creates them. Separate the type IDs with commas, as in `VeteranAircraft=MIG,YAK`.

We read the list when the rules load. A house whose country lists a type makes each new aircraft of that type a veteran, whether an airfield, a trigger or a team creates it. The check does not look at [`Trainable=`](/keys/trainable/). Objects that already exist keep their rank.

A key that is absent or empty leaves the list as it was, so a country with no key has no listed aircraft. Only the first 127 characters of the value are read, so a type named after that point is ignored. A name that matches no aircraft type adds nothing, because no aircraft is built from it.

The list comes from the country's own section. A country based on another with [`ParentCountry=`](/keys/parentcountry/#scope-housetype) does not inherit the parent's list.

[Promotion without kills](/systems/veterancy/#promotion-without-kills) lists the other ways a new object can start with a rank.
