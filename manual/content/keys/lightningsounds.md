---
key: LightningSounds
summary: "The sounds a lightning storm's bolts make."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each bolt of a lightning storm plays a sound picked at random from this list where it strikes. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[AudioVisual]
LightningSounds=MyStrike1,MyStrike2 ; sound IDs registered in SOUNDMD.INI
```

The ion storm uses [`LightningSound`](/keys/lightningsound/) instead.
