---
key: ForbiddenHouses
summary: Keeps the countries listed from building the type.
see_also: ["RequiredHouses", "Owner", "system:production"]
when_omitted:
  kind: value
  value: "no limit"
---

A house may not build the type if the country it acts as is in the list. The value is a comma-separated list of country names from `[Countries]`, like [`Owner=`](/keys/owner/). [What a house may build](/systems/production/#country-and-stolen-technology) covers where the test sits among the other gates.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
ForbiddenHouses=Russians
```

The test also applies to computer houses.
