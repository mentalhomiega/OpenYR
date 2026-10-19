---
key: PipWrap
scope: aircrafttype
label: Ammunition pip columns
see_also: [PipScale, MaxPips, Ammo, ReloadIncrement]
when_omitted:
  kind: value
  value: "0"
---

`PipWrap` sets how many columns of ammunition pips an object with `PipScale=Ammo` draws. With the default of `0` the object shows one pip for each shot it holds, in a single row. With `PipWrap=3` and `Ammo=9`, the pips form three columns of three rows. A column shows the highest row it reaches: row `r` of column `c`, counting from 0, stands for shot number `r * PipWrap + c + 1`, so a column fills from its bottom row up. A column with no shot in any row shows the empty pip. A pip count that `PipWrap` does not divide leaves its last shots without a pip.

```ini title="rules.ini"
[MYARTILLERY] ; a UnitType registered in [VehicleTypes]
Ammo=9
PipScale=Ammo
PipWrap=3 ; three columns, each holding up to three shots
```

With `PipWrap` set, the pip count is the whole `Ammo` value, so [`MaxPips`](/keys/maxpips/) and its default cap of five do not apply. A vehicle also divides its shots by `PipWrap` to work out the [`ReloadIncrement`](/keys/reloadincrement/) it adds to its reload delay, and that use needs no `PipScale`.
