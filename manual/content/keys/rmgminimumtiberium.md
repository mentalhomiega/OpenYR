---
key: RMGMinimumTiberium
summary: How much tiberium each player adds to a generated map's fields at the lowest tiberium setting.
see_also: [RMGMaximumTiberium, "system:map-generation"]
when_omitted:
  kind: value
  value: "2500"
---

`RMGMinimumTiberium` and [`RMGMaximumTiberium`](/keys/rmgmaximumtiberium/) set how much tiberium each player adds to a generated map's layout fields. A map seed's [`Tiberium`](/keys/tiberium/#scope-random-map-generation) setting, from 0 to 100, picks a point between them: the minimum plus that percentage of the gap to the maximum, with any fraction dropped. Each unit covers one bare cell or thickens a covered cell by one stage.

```ini title="RMGMD.INI"
[General]
RMGMinimumTiberium=900
RMGMaximumTiberium=1050
```

With these values, `Tiberium=50` gives each player 975 units. The layout fields share that amount times [`NumPlayers`](/keys/numplayers/#scope-random-map-generation), divided evenly among them. A maximum below the minimum makes the amount fall as `Tiberium` rises. The start-point fields use neither key. [Random map generation](/systems/map-generation/#tiberium) explains how the fields grow.
