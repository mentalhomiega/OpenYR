---
key: AirRangeBonus
scope: aircrafttype
label: Extra range against aircraft
see_also: [Range, AA]
when_omitted:
  kind: value
  value: "0"
---

`AirRangeBonus` adds to the reach of every weapon of this object when the target is in the air, such as a flying aircraft or a jumpjet in flight. The value is in cells, and a fraction is accepted. It applies to soldiers, vehicles, aircraft and structures alike.

```ini title="rulesmd.ini"
[FV] ; the stock IFV
AirRangeBonus=4 ; reaches 4 cells farther against aircraft
```
