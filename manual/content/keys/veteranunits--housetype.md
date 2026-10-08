---
key: VeteranUnits
scope: housetype
label: Country veteran vehicle types
summary: The vehicle types that a house of this country makes veterans when it creates them.
see_also: [VeteranInfantry, VeteranAircraft, "system:veterancy"]
when_omitted:
  kind: value
  value: ""
  note: No vehicle type is listed, so new vehicles start as rookies unless a spy or another rule promotes them.
---

`VeteranUnits=` lists the vehicle types that a house of this country makes veterans when it creates them. Separate the type IDs with commas, as in `VeteranUnits=HTNK,MTNK`.

We read the list when the rules load. A house whose country lists a type makes each new vehicle of that type a veteran, whether a factory, a trigger, a reinforcement or a team creates it. The check does not look at [`Trainable=`](/keys/trainable/) or at whether the vehicle is naval, so a listed type is promoted whatever those keys say. Objects that already exist keep their rank.

A key that is absent or empty leaves the list as it was, so a country with no key has no listed vehicles. Only the first 127 characters of the value are read, so a type named after that point is ignored. A name that matches no vehicle type adds nothing, because no vehicle is built from it.

The list comes from the country's own section. A country based on another with [`ParentCountry=`](/keys/parentcountry/#scope-housetype) does not inherit the parent's list.

[Promotion without kills](/systems/veterancy/#promotion-without-kills) lists the other ways a new object can start with a rank.
