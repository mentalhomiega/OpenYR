---
key: PurifierBonus
summary: The share of each ore delivery's value that every ore purifier adds.
see_also: [OrePurifier, AIVirtualPurifiers, "system:tiberium"]
when_omitted:
  kind: value
  value: "0.25"
---

For every [`OrePurifier=yes`](/keys/orepurifier/) structure a house has, each unit of ore its harvesters unload pays this share of its value again, and adds this share of its score.

```ini title="rulesmd.ini"
[General]
PurifierBonus=0.25
```

With the default and one purifier, a unit worth 25 credits pays 25 plus 6, since the bonus of 6.25 is rounded down. A value of `0` turns purifiers off.
