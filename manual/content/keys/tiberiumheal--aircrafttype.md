---
key: TiberiumHeal
scope: aircrafttype
label: Heals on Tiberium
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "no"
---

The game reads this flag and counts it in the checksum, but nothing acts on it. An infantry, vehicle or aircraft of this type does not heal on Tiberium, and its destruction seeds no Tiberium. The `TIBERIUM_HEAL` ability from [`VeteranAbilities`](/keys/veteranabilities/) or [`EliteAbilities`](/keys/eliteabilities/) has no effect either.

For healing that works, see [Repair](/systems/repair/).
