---
key: DeployedCrushable
scope: infantrytype
label: 'Crushable while deployed'
see_also: [Deployer, Crushable, Crusher]
when_omitted:
  kind: value
  value: "yes"
---

A `DeployedCrushable=no` soldier cannot be crushed while it is deployed. A vehicle does not crush it in that state, and treats its cell as it would any other occupant that it cannot crush. When the soldier packs up, it can be crushed again. The default `yes` leaves a deployed soldier crushable like any other.

```ini title="rulesmd.ini"
[GGI] ; Guardian GI
Deployer=yes
DeployedCrushable=no
```

The key only matters for a soldier whose type sets [`Deployer=yes`](/keys/deployer/#scope-infantrytype), since only such a soldier can be deployed.
