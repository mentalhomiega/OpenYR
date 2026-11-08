---
key: DeployedCrushable
scope: infantrytype
label: 'Crushable while deployed'
see_also: [Deployer, Crushable, Crusher]
when_omitted:
  kind: value
  value: "yes"
---

A `DeployedCrushable=no` soldier cannot be crushed from the moment it finishes digging in until it finishes packing up. A vehicle does not crush it in that state, and treats its cell as it would any other occupant that it cannot crush. While the soldier is still digging in, it can be crushed, and it cannot be while it is packing up. The default `yes` leaves a deployed soldier crushable like any other.

```ini title="rulesmd.ini"
[GGI] ; Guardian GI
Deployer=yes
DeployedCrushable=no
```

The key only matters for a soldier whose type sets [`Deployer=yes`](/keys/deployer/#scope-infantrytype), since only such a soldier can be deployed.
