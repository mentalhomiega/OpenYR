---
key: ListIndex
summary: Where the country stands in the lists of countries that the lobbies offer.
see_also: ["system:skirmish-setup", Multiplay]
when_omitted:
  kind: value
  value: "100"
---

The skirmish and network lobbies list the countries that set [`Multiplay=yes`](/keys/multiplay/) in ascending order of this value. Countries with the same value stay in the order of `[Countries]`. A negative value removes the country from the lists without removing it from `[Countries]`.

```ini title="rules.ini"
[Guild1]
ListIndex=9
```

The key does not change which country a player or computer house gets when Random is chosen.
