---
key: ForbiddenHouses
summary: Keeps the countries listed from building the type.
see_also: ["RequiredHouses", "Owner", "system:production"]
when_omitted:
  kind: value
  value: "no limit"
---

A house may not build the type if its own country is in the list. A house's own country is the country its house record names with [`Country=`](/keys/country/), or, when the record names none, the country its own name gives. [`ActsLike=`](/keys/actslike/) does not change the test, and a country based on another with [`ParentCountry=`](/keys/parentcountry/) counts as itself. The value is a comma-separated list of country names from `[Countries]`, like [`Owner=`](/keys/owner/). [What a house may build](/systems/production/#country-and-stolen-technology) covers where the test sits among the other gates.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
ForbiddenHouses=Russians
```

The test also applies to computer houses.
