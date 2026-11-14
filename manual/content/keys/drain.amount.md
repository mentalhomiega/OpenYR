---
key: Drain.Amount
summary: "Limits how much power a local drain takes from a power plant."
see_also: [Drain.Local]
when_omitted:
  kind: value
  value: "0"
---

`Drain.Amount` is an Ares key that applies only when [`Drain.Local=yes`](/keys/drain.local/) is set on the draining object or the drained power plant. The drain starts from the plant's power output, then the drainer's value limits it, then the plant's. A value above zero is a most and a value below zero is a least, so `-600` drains at least 600 power, even from a plant that makes less. Zero leaves the amount as it is.

For a plant making 500 power, a drainer with `Drain.Amount=200` takes 200, one with `0` takes 500 and one with `-1000` takes 1000.

```ini title="rulesmd.ini"
[MYPOWERPLANT] ; example BuildingType
Drain.Amount=300  ; a local drain never takes more than 300
```
