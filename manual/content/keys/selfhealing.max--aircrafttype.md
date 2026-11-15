---
key: SelfHealing.Max
scope: aircrafttype
summary: "The share of its strength up to which this type heals itself, whatever its rank."
see_also: [SelfHealing.Amount, SelfHealing.RookieMax, SelfHealing.VeteranMax, SelfHealing.EliteMax, SelfHealingCap]
when_omitted:
  kind: inherited
  note: The rank form for the object's rank, else SelfHealingCap, else the SelfHealCap of [General], else full strength.
---

`SelfHealing.Max` is an Ares key. Self-healing stops when the object holds this share of its [`Strength`](/keys/strength/), written as `50%` or `0.5`. A step never takes the object past it, and an object already above it does not lose strength. Stock Yuri's Revenge heals up to full strength. The Tiberian Dawn Mammoth Tank, which mended to half, is the Ares manual's example.

When it is set it wins over [`SelfHealing.RookieMax`](/keys/selfhealing.rookiemax/), [`SelfHealing.VeteranMax`](/keys/selfhealing.veteranmax/) and [`SelfHealing.EliteMax`](/keys/selfhealing.elitemax/).

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
SelfHealing=yes
SelfHealing.Max=50%
```
