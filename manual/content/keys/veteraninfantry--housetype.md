---
key: VeteranInfantry
scope: housetype
label: Country veteran infantry types
summary: The infantry types that a house of this country makes veterans when it creates them.
see_also: [VeteranUnits, VeteranAircraft, "system:veterancy"]
when_omitted:
  kind: value
  value: ""
  note: No infantry type is listed, so new infantry start as rookies unless a spy or another rule promotes them.
---

`VeteranInfantry=` lists the infantry types that a house of this country makes veterans when it creates them. Separate the type IDs with commas, as in `VeteranInfantry=E1,E3`.

We read the list when the rules load. A house whose country lists a type makes each new infantry of that type a veteran, whether a factory, a trigger, a reinforcement or a team creates it. The check does not look at [`Trainable=`](/keys/trainable/), so a listed type that is not trainable is promoted too. Objects that already exist keep their rank.

A key that is absent or empty leaves the list as it was, so a country with no key has no listed infantry. Only the first 127 characters of the value are read, so a type named after that point is ignored. A name that matches no infantry type adds nothing, because no infantry is built from it.

The list comes from the country's own section. A country based on another with [`ParentCountry=`](/keys/parentcountry/#scope-housetype) does not inherit the parent's list.

[Promotion without kills](/systems/veterancy/#promotion-without-kills) lists the other ways a new object can start with a rank.
