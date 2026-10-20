---
key: DisplayCreditsDelay
scope: global-rules
summary: "The minutes between the floating cash texts of one structure."
see_also: [ProduceCashDisplay]
when_omitted:
  kind: value
  value: "0.02, about 18 game frames"
---

A structure with [`ProduceCashDisplay=yes`](/keys/producecashdisplay/) shows a new floating text only when this many minutes have passed since its last one. The delay in frames is the value times the frames in a minute, rounded down. With `0`, every payment shows at once.

```ini title="rulesmd.ini"
[AudioVisual]
DisplayCreditsDelay=0.05
```
