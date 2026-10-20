---
key: ConsideredAircraft
scope: aircrafttype
label: Sorted with the aircraft on the sidebar
summary: Places a vehicle or aircraft cameo with the aircraft, after the ground vehicles, in the sidebar order.
see_also: ["system:sidebar", Naval, UseCameoSortOrder]
when_omitted:
  kind: context-dependent
  note: "`yes` for an AircraftType, `no` for every other type."
---

On the vehicle strip, ground vehicles come first, then types with `ConsideredAircraft=yes`, then [`Naval=yes`](/keys/naval/) types. [The order of the strips](/systems/sidebar/#the-order-of-the-strips) gives the full comparison.

```ini title="rulesmd.ini"
[MyGunship] ; example VehicleType that flies
ConsideredAircraft=yes
```

The key is read for every type, but only a VehicleType or an AircraftType uses it. It has no effect with [`UseCameoSortOrder=yes`](/keys/usecameosortorder/) or [`SidebarSorting=no`](/keys/sidebarsorting/).
