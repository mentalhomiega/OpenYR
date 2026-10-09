---
key: GameSpeed
scope: global-rules
label: Game speed
when_omitted:
  kind: value
  value: "6"
  note: The stock rules set `1`.
---

`GameSpeed` sets the starting game speed on the skirmish setup screen, on the scale that [`GameSpeed`](/keys/gamespeed/) gives a game: `0` is the fastest and `6` the slowest. The screen shows 6 minus this value, so the stock `1` appears as `5`. The screen shows this value until a match starts from it.
