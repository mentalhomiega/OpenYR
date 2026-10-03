---
key: ResourceGatherer
summary: "Marks a type that collects ore."
see_also: [ResourceDestination, Airstrike]
when_omitted:
  kind: value
  value: "no"
---

A structure that sets both `ResourceGatherer=yes` and `ResourceDestination=yes`, such as a deployed Slave Miner, is never the target of an [`Airstrike`](/keys/airstrike/) weapon; a unit with one uses its first weapon on it instead. The key has no other effect.

```ini title="rulesmd.ini"
[SMIN] ; Slave Miner
ResourceGatherer=yes
```
