---
key: AIAutoDeployFrameDelay
summary: "How many frames the computer waits before it digs in a guarding soldier, for each difficulty."
see_also: [Deployer, UndeployDelay]
when_omitted:
  kind: value
  value: "15,25,100"
---

Three numbers, for the computer's Easy, Normal and Hard difficulty in turn. A computer player's [`Deployer=yes`](/keys/deployer/#scope-infantrytype) soldier with [`DeployFire=yes`](/keys/deployfire/#scope-infantrytype) and a negative [`UndeployDelay`](/keys/undeploydelay/#scope-infantrytype) digs in once more than that many frames have passed since its guard mission began. It does not dig in while it has a destination or an archive target in another cell. A soldier immune to radiation never digs in this way.

```ini title="rulesmd.ini"
[General]
AIAutoDeployFrameDelay=15,25,100
```

A soldier that is still moving when the delay has passed stops, and digs in at its next guard check.
