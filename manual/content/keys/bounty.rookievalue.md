---
key: Bounty.RookieValue
summary: The bounty this object pays when it is destroyed as a rookie, overriding Bounty.Value.
see_also: ["Bounty.Value", Bounty, "system:bounty"]
when_omitted:
  kind: inherited
  note: Bounty.Value from the same section, or 0 when that is also absent.
---

`Bounty.RookieValue` is the amount the owner of a destroyer with [`Bounty=yes`](/keys/bounty/) receives when this object is destroyed while it is neither a veteran nor an elite. A negative value takes that amount from the destroyer's owner instead.

The rank is the one the object holds when it dies. [Veterancy and promotion](/systems/veterancy/) explains how objects gain rank.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
Bounty.Value=300
Bounty.RookieValue=200 ; only a rookie one pays 200
```
