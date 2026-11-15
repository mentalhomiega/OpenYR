---
key: NoSelfGuardArea
scope: infantrytype
label: No guard area cursor on itself
see_also: [Deployer]
when_omitted:
  kind: value
  value: "no"
---

`NoSelfGuardArea` is an Ares key. A player's soldier whose weapons heal (their average damage is below zero) normally shows the guard area cursor when the mouse is over the soldier itself. With `NoSelfGuardArea=yes` the soldier shows its ordinary cursor over itself instead: the deploy cursor when it has [`Deployer=yes`](/keys/deployer/#scope-infantrytype), and no action otherwise.

The key changes nothing over other objects; the soldier still offers to heal a damaged ally.

```ini title="rulesmd.ini"
[MYMEDIC] ; example InfantryType with a healing weapon that deploys
Deployer=yes
NoSelfGuardArea=yes
```
