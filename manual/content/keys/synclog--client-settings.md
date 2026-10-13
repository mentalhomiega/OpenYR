---
key: SyncLog
scope: client-settings
label: Sync log
when_omitted:
  kind: value
  value: "no"
---

`SyncLog=yes` writes the frame check value to the debug log on every game frame, as the `-SYNCLOG` command-line switch does. Each line reads `SYNC frame <number> crc <value>`. The value is the one the network compares, so two machines' logs agree on every frame for as long as they stay in sync.

The debug log is in the `Debug` folder beside the game executable. The game reads the setting at startup, and the options screens leave it as the file has it.

```ini title="RA2MD.INI"
[Options]
SyncLog=yes
```

A launch file can turn the log on as well, with [`SyncLog`](/formats/spawn-ini/#what-the-file-asks-for) in `[Settings]`.
