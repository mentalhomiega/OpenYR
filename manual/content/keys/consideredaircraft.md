---
key: ConsideredAircraft
summary: Makes a vehicle count as an aircraft for the sidebar order, the Academy veterancy bonus, relative damage and team retaliation.
see_also: ["system:sidebar", Naval, UseCameoSortOrder, Organic, TeamRetaliate, "Academy.AircraftVeterancy", "RelativeDamage.Aircraft"]
when_omitted:
  kind: context-dependent
  note: "`yes` for an AircraftType, `no` for every other type."
---

`ConsideredAircraft=yes` on a vehicle or aircraft type changes four results:

- **Sidebar order.** On the vehicle strip, ground vehicles come first, then types with `ConsideredAircraft=yes`, then [`Naval=yes`](/keys/naval/) types. [The order of the strips](/systems/sidebar/#the-order-of-the-strips) gives the full comparison. The key has no effect with [`UseCameoSortOrder=yes`](/keys/usecameosortorder/) or [`SidebarSorting=no`](/keys/sidebarsorting/).
- **Academy bonus.** A new object of the type takes its veterancy from [`Academy.AircraftVeterancy`](/keys/academy.aircraftveterancy/) instead of the vehicle value.
- **Relative damage.** A warhead with `RelativeDamage=yes` hurts it by [`RelativeDamage.Aircraft`](/keys/relativedamage.aircraft/) instead of the vehicle share.
- **Team retaliation.** With [`TeamRetaliate=yes`](/keys/teamretaliate/), a computer team ignores an attacker of the type.

The sidebar order applies to vehicle and aircraft types only.

```ini title="rulesmd.ini"
[MyGunship] ; example VehicleType that flies
ConsideredAircraft=yes
```
