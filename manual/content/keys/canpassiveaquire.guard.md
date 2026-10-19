---
key: CanPassiveAquire.Guard
summary: With no, an object of this type on the Guard mission never picks a target on its own.
see_also: [CanPassiveAquire, "CanPassiveAquire.Cloak", "system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

A `CanPassiveAquire.Guard=no` object holds fire on the Guard mission until it is ordered to attack. It still scans on the Guard area mission and, with [`OpportunityFire=yes`](/keys/opportunityfire/), on a move order, so a player can make it take targets by sending it to guard an area. The key has no effect where [`CanPassiveAquire`](/keys/canpassiveaquire/) is already `no`, and the key's spelling is as the game reads it.

```ini title="rulesmd.ini"
[MYSNIPER] ; example InfantryType
CanPassiveAquire.Guard=no
```
