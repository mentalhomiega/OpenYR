---
key: SpyPowerBlackout
summary: "How many frames a spy cuts off the power of the house whose power plant it enters."
see_also: [SpyMoneyStealPercent, Spyable, "system:capture"]
when_omitted:
  kind: value
  value: "0"
---

When a spy walks into another house's structure with positive [`Power`](/keys/power/#scope-buildingtype), that house's structures make no power for this many frames. Its structures then behave as for any shortage of power. [Infiltrating it](/systems/capture/#infiltrating-it) lists the other spy effects.

```ini title="rulesmd.ini"
[General]
SpyPowerBlackout=1000
```

At `0` or below, spying on a power plant does nothing. A second spy restarts the blackout at its full length.
