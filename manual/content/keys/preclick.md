---
key: PreClick
summary: Makes a superweapon's shot only pick cells for a second weapon to act on.
see_also: [PostClick, PreDependent, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A `PreClick=yes` weapon is the first half of a [two-click weapon](/systems/superweapons/#two-click-weapons). Its shot stays charged: the charge is spent only when the [`PostClick=yes`](/keys/postclick/) weapon whose [`PreDependent=`](/keys/predependent/) names its `Type=` fires, and a right click that cancels that second click keeps it ready.

Only `Type=ChronoSphere` uses the first click to pick cells and arm the second click.

:::caution[`PreClick=yes` on another behavior never spends its charge]
A `PreClick=yes` weapon of any other `Type=` delivers its effect and stays charged, so it can be fired again at once, as often as the player likes.
:::

```ini title="rulesmd.ini"
[ChronoSphereSpecial]
Type=ChronoSphere
Action=ChronoSphere
PreClick=yes
```
