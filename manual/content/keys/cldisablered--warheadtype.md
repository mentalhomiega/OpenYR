---
key: CLDisableRed
scope: warheadtype
label: 'Flash leaves out red'
see_also: [CLDisableGreen, CLDisableBlue, Bright]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[MyShellWH] ; example WarheadType
CLDisableRed=yes
```

With `yes`, the flash of a shot from a weapon with [`Bright=yes`](/keys/bright/#scope-weapontype) that carries this warhead brightens the ground without red, which tints it. A flash with all three `CLDisable` keys set to `yes` brightens nothing. The keys do not affect flashes that [`Bright=yes` on the warhead](/keys/bright/#scope-warheadtype) causes.
