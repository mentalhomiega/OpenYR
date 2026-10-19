---
key: FlashSidebarTabFrames
scope: superweapontype
label: Sidebar tab flash length
see_also: [IsPowered, RechargeTime, "system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

`FlashSidebarTabFrames=` sets how long the sidebar tab holding a superweapon flashes after the weapon is ready. With the default `-1` the tab flashes for as long as the weapon stays ready. With `0` it never flashes. With a positive number it flashes for that many frames counted from the frame the weapon's recharge finished.

```ini title="rules.ini"
[MyStormSpecial] ; a SuperWeaponType
FlashSidebarTabFrames=300 ; the tab flashes for 20 seconds after the weapon is ready
```

A weapon that is suspended, such as one that has lost its power, never flashes. A weapon with `UseChargeDrain=yes` flashes whenever it is not charging. The count starts only when a recharge completes: a weapon given fully charged, as a trigger can, was never recharged, so a positive count does not make its tab flash.

Only the modern sidebar panel flashes tabs, and it flashes a tab only while another tab is shown. The superweapons sit on the defenses tab, the second one.
