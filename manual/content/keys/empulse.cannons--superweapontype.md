---
key: EMPulse.Cannons
scope: superweapontype
summary: "Lists the structures that fire this EM pulse."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "Every EMPulseCannon=yes structure"
---

Only powered structures of these types fire, up to [`SW.MaxCount=`](/keys/sw.maxcount/) of them, nearest the target first. The same list limits which cannons the targeting cursor tests for range.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EMPulse.Cannons=GAEMP
```

See [Em pulse](/systems/superweapons/#em-pulse).
