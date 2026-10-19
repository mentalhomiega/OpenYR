---
key: Count
scope: ui-controls
label: Lobby color count
summary: The number of player colors the lobbies offer, set in the UI file's `[Colors]` section.
when_omitted:
  kind: value
  value: "8"
---

`Count=` sets how many colors the skirmish and network lobbies and the color picker list. A value below 8 counts as 8 and a value above 16 as 16. The keys of [`[Colors]`](/formats/ui-ini/#lobby-colors) describe each slot up to the count.

```ini title="UIMD.INI"
[Colors]
Count=14
```

Slots after the eighth default to the schemes `NeonBlue`, `LightBlue`, `Yellow`, `Green`, `Red` and `Grey`, so a count of 14 lists six more colors without further keys. At most eight players start a match, so a lobby with more colors lets players leave colors unused.
