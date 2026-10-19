---
key: ProtectedDriver
summary: "With yes, KillDriver warheads leave this vehicle's driver alone."
see_also: [ProtectedDriver.MinHealth, KillDriver, Organic, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

A `ProtectedDriver=yes` vehicle keeps its driver against a [`KillDriver`](/keys/killdriver/) warhead, which then damages it as it does any other target. The key sets the default of [`ProtectedDriver.MinHealth`](/keys/protecteddriver.minhealth/) to `0`, so a vehicle that also sets that key is protected only above that health.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
ProtectedDriver=yes
```

A type with [`Organic=yes`](/keys/organic/) or `Natural=yes` keeps its driver whatever this key says, as does an object with the `PROTECTED_DRIVER` [veteran or elite ability](/systems/veterancy/#abilities).
