---
key: ResourceDestination
summary: "Marks a type that takes in ore."
see_also: [ResourceGatherer, Airstrike]
when_omitted:
  kind: value
  value: "no"
---

A structure that sets both `ResourceGatherer=yes` and `ResourceDestination=yes`, such as a deployed Slave Miner, is never the target of an [`Airstrike`](/keys/airstrike/) weapon; a unit with one uses its first weapon on it instead. The key has no other effect.

```ini title="rulesmd.ini"
[YAREFN] ; Slave Miner, deployed
ResourceDestination=yes
```
