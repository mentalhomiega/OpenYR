---
key: FactoryOwners
summary: The countries whose factories may build objects of this type.
see_also: ["FactoryOwners.Forbidden", RequiredHouses, ForbiddenHouses, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

`FactoryOwners` is a comma separated list of country names. When it is not empty, a house can build the type only if it owns a factory for that kind of object that was first owned by one of the listed countries. A factory counts for the country of the house that first owned it, so a captured factory keeps its builder's country. An empty or absent list allows every country.

The computer ignores the key for structure types. The key does not read the plans an Ares building can give a house.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
FactoryOwners=Americans,Alliance
```
