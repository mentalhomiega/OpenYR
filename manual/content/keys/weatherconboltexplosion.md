---
key: WeatherConBoltExplosion
summary: "The explosion animation of a lightning storm's bolt."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays where a lightning storm's bolt strikes, and wherever else [`LightningWarhead`](/keys/lightningwarhead/) explodes. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
WeatherConBoltExplosion=MYBOLTEXP ; an AnimType registered in [Animations]
```

With the key unset, explosions through that warhead show no animation.
