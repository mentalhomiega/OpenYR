---
key: BuildTime
scope: housetype
label: Country build-time multiplier
no_effect: true
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country combines this multiplier with [the difficulty section's `BuildTime=`](/keys/buildtime/#scope-difficulty-settings) and [`GameSpeedBias`](/keys/gamespeedbias/) [when it is given its difficulty slot](/systems/difficulty/#how-the-figures-are-combined). A campaign game leaves the country's value out and keeps the other two.

Nothing in the game reads the combined figure, so no value here changes a build time. [How long it takes](/systems/production/#how-long-it-takes) lists the factors that do.
