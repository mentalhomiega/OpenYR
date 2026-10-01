---
key: IsAlternateColor
summary: Draws an electric bolt in its second color.
see_also: ["IsElectricBolt"]
when_omitted:
  kind: value
  value: "no"
---

An [`IsElectricBolt=yes`](/keys/iselectricbolt/) weapon with `IsAlternateColor=yes` draws two of its bolt's three strands in color 5 of `PALETTE.PAL` rather than color 10. The third strand keeps color 15.

```ini title="rulesmd.ini"
[MyTankZap] ; example WeaponType
IsElectricBolt=yes
IsAlternateColor=yes
```

The key has no effect on a weapon that does not draw an electric bolt.
