---
key: SelfHealing.Amount
scope: aircrafttype
summary: "The strength one self-healing step restores to this type, whatever its rank."
see_also: [SelfHealing, SelfHealing.Rate, SelfHealing.Max, SelfHealing.CombatDelay, SelfHealing.RookieAmount, SelfHealing.VeteranAmount, SelfHealing.EliteAmount, SelfHealingStep]
when_omitted:
  kind: inherited
  note: The rank form for the object's rank, else SelfHealingStep, else the SelfHealStep of [General], else 1.
---

`SelfHealing.Amount` is an Ares key. A type that heals itself, by `SelfHealing=yes` or by veteran or elite self-healing, restores this many strength points each time [`SelfHealing.Rate`](/keys/selfhealing.rate/) comes round. Stock Yuri's Revenge restores one. A value below zero counts as unset; `0` stops the healing.

When it is set it wins over [`SelfHealing.RookieAmount`](/keys/selfhealing.rookieamount/), [`SelfHealing.VeteranAmount`](/keys/selfhealing.veteranamount/) and [`SelfHealing.EliteAmount`](/keys/selfhealing.eliteamount/). Healing stops at [`SelfHealing.Max`](/keys/selfhealing.max/).

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
SelfHealing=yes
SelfHealing.Amount=5
```
