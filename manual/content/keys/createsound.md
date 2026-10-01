---
key: CreateSound
summary: "The sound played where a new object of this type appears when it is built."
see_also: [CreateUnitSound, CreateInfantrySound, CreateAircraftSound]
when_omitted:
  kind: value
  value: none
---

When a house finishes building an object of this type, this sound plays where the object appears, for every player who can hear that spot. It replaces the rules' [`CreateUnitSound`](/keys/createunitsound/), [`CreateInfantrySound`](/keys/createinfantrysound/) or [`CreateAircraftSound`](/keys/createaircraftsound/) for the type.

```ini title="rulesmd.ini"
[MYHERO] ; example InfantryType
CreateSound=MyHeroReporting
```
