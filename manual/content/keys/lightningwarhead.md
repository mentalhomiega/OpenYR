---
key: LightningWarhead
summary: "The warhead of a lightning storm's bolts."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each bolt of a lightning storm deals [`LightningDamage`](/keys/lightningdamage/) through this warhead. The warhead's [`CellSpread`](/keys/cellspread/) sets how far each strike reaches. Any explosion through this warhead plays [`WeatherConBoltExplosion`](/keys/weatherconboltexplosion/) instead of the warhead's own animations. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningWarhead=MyBoltWH ; a WarheadType registered in [Warheads]
```

With no warhead, the bolts do no damage.
