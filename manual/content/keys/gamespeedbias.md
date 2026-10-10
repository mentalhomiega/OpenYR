---
key: GameSpeedBias
summary: The multiplier on every house's build time.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "1"
---

The value multiplies every house's build time. A value above 1 makes production slower: at `1.1`, builds take about 10 percent longer.

The value also multiplies the [`Groundspeed=`](/keys/groundspeed/) and [`Airspeed=`](/keys/airspeed/) figures, but nothing reads them, so it changes no speed.

A house combines this value with its difficulty section's [`BuildTime=`](/keys/buildtime/#scope-difficulty-settings) [when it is given its difficulty](/systems/difficulty/#how-the-figures-are-combined). Outside a campaign, the country's [`BuildTime=`](/keys/buildtime/#scope-housetype) is combined in as well. Campaign games use this value too.
