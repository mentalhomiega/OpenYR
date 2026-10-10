---
key: BuildTime
scope: difficulty-settings
label: Difficulty build-time multiplier
no_effect: true
when_omitted:
  kind: value
  value: "1"
  note: The difficulty block is re-read from fixed defaults whenever its section is present, so a later file that contains the section without this key restores 1 rather than keeping the earlier value.
---

`[Easy]`, `[Normal]` and `[Difficult]` each set their own multiplier. A house combines the one for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot) with [`GameSpeedBias`](/keys/gamespeedbias/) and, outside a campaign game, with [its country's `BuildTime=`](/keys/buildtime/#scope-housetype).

Nothing in the game reads the combined figure, so no value here changes a build time. [How long it takes](/systems/production/#how-long-it-takes) lists the factors that do.
