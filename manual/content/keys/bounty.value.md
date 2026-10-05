---
key: Bounty.Value
summary: The bounty this object pays when it is destroyed, set for the rookie, veteran and elite ranks at once.
see_also: ["Bounty.RookieValue", "Bounty.VeteranValue", "Bounty.EliteValue", Bounty, "system:bounty"]
when_omitted:
  kind: value
  value: "0"
  note: The rank keys still apply.
---

`Bounty.Value` is the amount the owner of a destroyer with [`Bounty=yes`](/keys/bounty/) receives when this object is destroyed, whatever its rank. A negative value takes that amount from the destroyer's owner instead.

[`Bounty.RookieValue`](/keys/bounty.rookievalue/), [`Bounty.VeteranValue`](/keys/bounty.veteranvalue/) and [`Bounty.EliteValue`](/keys/bounty.elitevalue/) override it for their own rank. A rank whose key is absent keeps the `Bounty.Value` amount.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
Bounty.Value=300
Bounty.EliteValue=500 ; an elite one pays 500; the other ranks pay 300
```
