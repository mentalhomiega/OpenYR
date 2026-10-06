---
key: BountyEnablers
summary: The structures a house must own before it collects any bounty.
see_also: [Bounty, GivesBounty, "system:bounty"]
when_omitted:
  kind: value
  value: none
  note: With no list, every house collects bounty.
---

`BountyEnablers=` lists BuildingType IDs. When the list has at least one entry, a house collects [bounty](/systems/bounty/#who-collects) only while it owns at least one structure of a listed type. When the list is empty or absent, ownership does not matter.

A structure stops counting when it is destroyed, sold or captured. A structure whose type sets [`Insignificant=yes`](/keys/insignificant/) never counts. A house with no listed structure neither collects a positive bounty nor loses a negative one.

```ini title="rulesmd.ini"
[General]
BountyEnablers=GACNST,NACNST ; a house collects bounty only while it owns one of these
```
