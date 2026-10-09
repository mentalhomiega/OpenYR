---
key: UndeployDelay
scope: infantrytype
label: 'Frames dug in'
see_also: [Deployer, DeployFire]
when_omitted:
  kind: value
  value: "-1"
---

A [`Deployer=yes`](/keys/deployer/#scope-infantrytype) soldier with `UndeployDelay` set to a number of frames stays dug in for that many frames after it deploys, then packs up on its own. A negative value, the default, leaves packing up to orders and to the computer player rules in [Deployer](/keys/deployer/#scope-infantrytype).

```ini title="rulesmd.ini"
[MYPSYCHIC] ; example InfantryType
Deployer=yes
DeployFire=yes
UndeployDelay=150 ; frames dug in before it packs up
```

While it is dug in, neither the Deploy order nor a click on the soldier packs it up before the delay ends. The Deploy order still digs in a soldier that is not dug in.
