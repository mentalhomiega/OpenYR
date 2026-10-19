---
key: SW.Deferment
scope: superweapontype
summary: "Delays the weapon's effect after the shot."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "0, or 20 for UnitDelivery"
---

The number of frames between the launch and the effect. The cost, the charge restart and [`Message.Launch=`](/keys/message.launch/) happen at the launch; the effect, [`Message.Activate=`](/keys/message.activate/) and the target sound and animation wait. A `Type=LightningStorm` weapon uses the value in place of `LightningStormDeferment`. Chronosphere, chrono warp, firestorm, battery and force shield weapons ignore it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Deferment=30
```

See [Cost and timing](/systems/superweapons/#cost-and-timing).
