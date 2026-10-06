---
key: DefaultToGuardArea
summary: Makes an idle infantryman or vehicle of this type take Area Guard instead of Guard.
see_also: [GuardArea, IQ, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

An idle infantryman or armed vehicle of a `DefaultToGuardArea=yes` type takes Area Guard instead of Guard, unless it belongs to a team, whose members always take Guard. Area Guard scans out to the larger area radius from the spot where the object was left; [Scan radius](/systems/target-selection/#scan-radius) gives both distances.

The key is a third way to reach Area Guard, beside the house [`GuardArea`](/keys/guardarea/) level and the `GUARD_AREA` [veterancy ability](/systems/veterancy/#abilities):

- **Infantry.** A human house's soldier takes Area Guard, armed or not. A computer house's infantry is not affected; its `IQ` decides.
- **Vehicles.** An armed vehicle takes Area Guard even when its house is below the `GuardArea` level, whoever owns it. An unarmed vehicle still takes Guard.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
DefaultToGuardArea=yes
```
