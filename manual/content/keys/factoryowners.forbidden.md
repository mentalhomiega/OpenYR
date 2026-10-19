---
key: FactoryOwners.Forbidden
summary: The countries whose factories alone cannot build objects of this type.
see_also: [FactoryOwners, RequiredHouses, ForbiddenHouses, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

`FactoryOwners.Forbidden` is a comma separated list of country names. A house that owns factories for the kind of object, all of them first owned by listed countries, cannot build the type. One factory first owned by another country allows it. A house with no such factory is left to the other build rules.

The computer ignores the key for structure types.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
FactoryOwners.Forbidden=Russians
```
