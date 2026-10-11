---
key: IonStormWarning
summary: Parsed warning time that no script uses.
no_effect: true
see_also: [LightningDeferment, "system:ion-storms"]
when_omitted:
  kind: value
  value: "31"
---

The [Ion Storm start...](/mapping/actions/taction-ion-storm-start/) trigger action and the [Ion storm start in...](/mapping/missions/tmission-ion-storm-start/) team mission call the lightning storm, which waits [`LightningDeferment`](/keys/lightningdeferment/) frames, not this many seconds. Nothing else orders the storm that would wait this long.

```ini title="rules.ini"
[General]
IonStormWarning=31
```

[The warning](/systems/ion-storms/#the-warning) describes the countdown the engine's ion storm code runs.
