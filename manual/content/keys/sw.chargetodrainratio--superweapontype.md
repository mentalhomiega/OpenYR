---
key: SW.ChargeToDrainRatio
scope: superweapontype
summary: "Sets how long a charge-draining weapon runs compared with its charge time."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] ChargeToDrainRatio"
---

The weapon runs for its charge time multiplied by this value. Use it on a weapon with [`UseChargeDrain=yes`](/keys/usechargedrain/) or `Type=Battery`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.ChargeToDrainRatio=2.0
```

See [Cost and timing](/systems/superweapons/#cost-and-timing).
