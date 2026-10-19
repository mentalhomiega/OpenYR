---
key: Lightning.ScatterDelay
scope: superweapontype
summary: "Sets how often clouds gather around the center of a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningScatterDelay"
---

While a `Type=LightningStorm` weapon's storm rages, [`Lightning.ScatterCount=`](/keys/lightning.scattercount/) clouds gather over random cells near the target every this many frames. A value of 0 or less gathers none. The cells lie within half of [`SW.Range=`](/keys/sw.range/) cells of the target along each axis, or within half of `LightningCellSpread` when the weapon sets no range.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.ScatterDelay=10
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
