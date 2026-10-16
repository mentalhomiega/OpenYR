---
key: HangSeconds
summary: How long the main thread may go without handling messages before the game reports a hang. Zero turns the check off.
when_omitted:
  kind: value
  value: "5"
---

`HangSeconds=` in `[Debug]` sets how long the main thread may go without handling messages before the game reports a hang. A value of `0` or below turns the check off. A value above `3600` is treated as `3600`.

The game reads the value once, after its one-time initialization, so a change takes effect at the next start.

```ini title="RA2MD.INI"
[Debug]
HangSeconds=10
```

The report, and the waits the check does not time, are on the [crash reports](/using/crash-reports/) page.
