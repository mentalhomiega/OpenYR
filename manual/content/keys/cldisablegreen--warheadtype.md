---
key: CLDisableGreen
scope: warheadtype
label: 'Flash leaves out green'
see_also: [CLDisableRed, CLDisableBlue, Bright]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[MyShellWH] ; example WarheadType
CLDisableGreen=yes
```

With `yes`, the flash of a shot from a weapon with [`Bright=yes`](/keys/bright/#scope-weapontype) that carries this warhead brightens the ground without green, which tints it. A flash with all three `CLDisable` keys set to `yes` brightens nothing. The keys do not affect flashes that [`Bright=yes` on the warhead](/keys/bright/#scope-warheadtype) causes.
