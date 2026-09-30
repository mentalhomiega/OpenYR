---
key: RequiredHouses
summary: Limits the type to the countries listed.
see_also: ["ForbiddenHouses", "Owner", "system:production"]
when_omitted:
  kind: value
  value: "no limit"
---

A house may build the type only if the country it acts as is in the list. The value is a comma-separated list of country names from `[Countries]`, like [`Owner=`](/keys/owner/). [What a house may build](/systems/production/#country-and-stolen-technology) covers where the test sits among the other gates.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
RequiredHouses=Americans,British
```

The test also applies to computer houses.
