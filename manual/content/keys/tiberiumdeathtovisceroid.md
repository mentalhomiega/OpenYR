---
key: TiberiumDeathToVisceroid
summary: Parsed switch that no death reads, so no death leaves a visceroid.
no_effect: true
see_also: [SmallVisceroid, Visceroids, "system:tiberium"]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="map file"
[Basic]
TiberiumDeathToVisceroid=no
```

Nothing in the game reads this switch. Infantry killed by the [damage Tiberium deals](/systems/tiberium/#damage), and any object killed by the damage a gas particle ([`BehavesLike=Gas`](/keys/behaveslike/#scope-particletype)) deals, leave no visceroid whatever the value. The map's value is still read and saved.
