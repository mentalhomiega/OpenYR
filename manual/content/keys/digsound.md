---
key: DigSound
summary: Sound of a subterranean unit breaking the surface on its way down or up, and of a nuclear missile launch.
see_also: [Dig, TunnelSpeed, Locomotor]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
DigSound=DIGIN1 ; a sound ID registered in SOUND.INI
```

A vehicle with the tunnel [`Locomotor`](/keys/locomotor/) plays this sound at its position at three points of a trip underground, each time with the [`Dig`](/keys/dig/) animation:

1. when it has turned to face its destination and starts to dig in;
2. when the nose-down dig-in finishes and it starts to sink;
3. when, rising at its destination, it comes within 50 leptons of ground level.

[`Dig`](/keys/dig/) lists the points a cancelled move skips.

A nuclear missile superweapon plays this sound at its target cell when it is launched, from its silo or from the map edge. The stock rules set it to `NukeSiren`, the air-raid siren.
