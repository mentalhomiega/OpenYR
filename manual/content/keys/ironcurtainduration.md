---
key: IronCurtainDuration
summary: How many frames the Iron Curtain protects what it covers.
see_also: [IronCurtainInvokeAnim, Organic, "system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

A vehicle, aircraft or structure covered by a [`Type=IronCurtain`](/keys/type/#scope-superweapontype) superweapon takes no damage for this many frames. [Iron curtain](/systems/superweapons/#iron-curtain) lists what the protection blocks.

```ini title="rulesmd.ini"
[CombatDamage]
IronCurtainDuration=750 ; 50 seconds at normal game speed
```

A second shot restarts the protection at its full length. At `0` or below, the shot protects nothing, though it still kills the infantry it covers.
