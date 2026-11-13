---
key: SelfHealing.RookieMax
scope: aircrafttype
summary: "The share of its strength up to which this type heals itself while rookie."
see_also: [SelfHealing.Max, SelfHealing.Amount, SelfHealingCap]
when_omitted:
  kind: inherited
  note: The SelfHealing.Max, else SelfHealingCap, else the SelfHealCap of [General], else full strength.
---

`SelfHealing.RookieMax` is an Ares key. It is [`SelfHealing.Max`](/keys/selfhealing.max/) for an object that is rookie. A set `SelfHealing.Max` takes its place.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
SelfHealing=yes
SelfHealing.RookieMax=50%
SelfHealing.VeteranMax=75%
SelfHealing.EliteMax=100%
```
