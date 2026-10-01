---
key: FillSilos
summary: Gives each house with storage about its money again in credits when the scenario opens.
see_also: ["system:tiberium", Storage]
when_omitted:
  kind: value
  value: "no"
---

```ini title="map file"
[Basic]
FillSilos=yes
```

As the scenario finishes loading, every house with free storage receives units of the first registered Tiberium type until they are worth at least its money minus one unit's [`Value`](/keys/value/). Each unit is paid out as a harvester unload would be, as credits scaled by the country's [`IncomeMult`](/keys/incomemult/), and adds five points to the house's score. Nothing is stored, so the house's free storage only decides whether it takes part.

:::caution[The house keeps its credits]
The house's money only sets how much it receives, and none of it is spent. A house with any free storage starts with nearly twice its money.
:::

Money carried over by [`CarryOverMoney`](/keys/carryovermoney/) arrives after this conversion and is not counted.

:::danger[Give the first Tiberium type a Value above 0]
With a `Value` of `0`, the scenario never finishes loading if any house has money and free storage.
:::
