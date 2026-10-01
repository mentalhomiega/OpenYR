---
key: IncomeMult
summary: Scales the credits a country earns from the ore its harvesters deliver.
see_also: [Value, "system:tiberium"]
when_omitted:
  kind: value
  value: "1.0"
---

Each unit of ore a harvester unloads pays its Tiberium type's [`Value`](/keys/value/) multiplied by this value, rounded down to whole credits for each delivery. [Credits and storage](/systems/tiberium/#credits-and-storage) covers the payment.

```ini title="rulesmd.ini"
[MyCountry] ; example country
IncomeMult=1.25 ; a quarter more from every load
```
