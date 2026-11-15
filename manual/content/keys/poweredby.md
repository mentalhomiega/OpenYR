---
key: PoweredBy
summary: "Shuts a vehicle down while its owner has no working structure of the listed types."
see_also: [PoweredUnit, PowersUnit, Operator]
when_omitted:
  kind: value
  value: "empty, so the vehicle needs no structure"
---

`PoweredBy` is an Ares key. It takes a comma-separated list of building types. A vehicle that names any works only while its owner has at least one structure of a listed type that is working: powered, finished and not being sold. Without one the vehicle shuts down: it drops its target and its orders, stops moving and plays its deactivate sound. It starts again, with its activate sound, as soon as such a structure works. A vehicle that is out of the world, such as one inside a transport, is not shut down.

Only vehicles read the list. Infantry, aircraft and structures that name it are never shut down by it. The vehicle does not need to stand near the structure, and one structure is enough however many vehicles depend on it.

It works alongside the stock [`PoweredUnit`](/keys/poweredunit/) rule, which keys on a structure's [`PowersUnit`](/keys/powersunit/) instead and lets a vehicle standing in a structure's cell go on working. A vehicle that sets both is shut down if either rule says so. `PoweredBy` has no such exception for a vehicle standing in a structure.

```ini title="rulesmd.ini"
[MTNK]
PoweredBy=GATECH,NAPSIS
```
