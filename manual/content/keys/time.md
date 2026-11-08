---
key: Time
summary: The hour a generated map is fought at, as a position from 0 through 3.
see_also: [Biome, UseTransitions]
when_omitted:
  kind: value
  value: "1"
  note: Afternoon. It places no floodlights unless UseTransitions is on.
---

The positions are `0` morning, `1` afternoon, `2` dusk and `3` night. [Map seed files](/formats/map-seed/) covers the section it is written in.

```ini title="map seed file"
[RandomMap]
Time=3
```

The hour picks the map's ambient light, tints and height level light from the lists in [RMGMD.INI](/formats/rmgmd-ini/). With the stock file, [`TemperateAmbientLight`](/keys/temperateambientlight/) lights a temperate map at three quarters of full daylight at morning and dusk, full at afternoon and 35 hundredths at night. A tundra or taiga map takes [`SnowAmbientLight`](/keys/snowambientlight/) instead, which differs only at night, at 55 hundredths.

The hour also sets how many `GALITE` floodlights ring each player's start point: none in the morning or the afternoon, two at dusk and four at night. [Map generation](/systems/map-generation/#floodlights) covers where a ring can be placed.

With [`UseTransitions=yes`](/keys/usetransitions/), the hour also selects the settings file loaded into the map, and every start point gets a ring of four lights whatever the hour.

When a map is [generated from a file](/systems/map-generation/#the-dialog-path-and-the-scenario-path), a value below `0` becomes `0` and one above `3` becomes `3`.
