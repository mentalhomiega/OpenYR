---
key: SW.InitialReady
scope: superweapontype
summary: "Makes the weapon fully charged the first time the house holds it."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the weapon starts ready the first time the house holds it. Later charges take the full [`RechargeTime=`](/keys/rechargetime/). A weapon that starts suspended for lack of power charges normally.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.InitialReady=yes
```

See [Availability rules](/systems/superweapons/#availability-rules).
