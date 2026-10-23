---
key: PerfLog
scope: client-settings
label: Perf log
when_omitted:
  kind: value
  value: "no"
---

`PerfLog=yes` writes timing and count lines to the debug log once a second. Each second has a `Perf:` line for the game frames, draws and presents, then `Perf frame`, `Perf draw`, `Perf present` and `Perf count` lines. The phases and counts are described in `docs/PERFORMANCE.md` in the source tree.

The lines cost time while they are on, so compare runs made with the same setting. The setting does not change the game, the match or its session identity.

```ini title="RA2MD.INI"
[Options]
PerfLog=yes
```
