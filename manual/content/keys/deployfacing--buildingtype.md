---
key: DeployFacing
scope: buildingtype
label: Facing to deploy into this structure
summary: "The direction a vehicle turns to before it deploys into this structure, and the direction it faces when the structure packs up."
see_also: [DeploysInto, UndeploysInto]
when_omitted:
  kind: value
  value: "4"
---

`DeployFacing` takes one of eight compass steps: `0` is north and each step turns 45 degrees clockwise, so `2` is east, `4` south and `6` west. A vehicle whose [`DeploysInto`](/keys/deploysinto/) names this structure turns to that direction before it deploys, and a structure with [`UndeploysInto`](/keys/undeploysinto/) places its vehicle facing that direction. A value of `8` or more wraps round to the steps from `0`.

```ini title="rulesmd.ini"
[YAREFN] ; Slave Miner, deployed
DeployFacing=0
```

The stock Yuri's Revenge rules set `DeployFacing=0` on the deployed Slave Miner, so the Slave Miner turns north before it deploys.
