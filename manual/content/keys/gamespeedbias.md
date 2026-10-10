---
key: GameSpeedBias
summary: A multiplier on house figures that nothing reads, so it changes no build time or speed.
no_effect: true
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "1"
---

A house multiplies this value into its [`BuildTime=`](/keys/buildtime/#scope-difficulty-settings), [`Groundspeed=`](/keys/groundspeed/#scope-difficulty-settings) and [`Airspeed=`](/keys/airspeed/#scope-difficulty-settings) figures [when it is given its difficulty](/systems/difficulty/#how-the-figures-are-combined). Nothing reads those figures, so no value here changes how long anything takes to build or how fast anything moves. Campaign games read the value too, to the same end.
