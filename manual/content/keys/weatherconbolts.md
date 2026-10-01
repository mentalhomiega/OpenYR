---
key: WeatherConBolts
summary: "The bolt animations of a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each bolt of a lightning storm is picked at random from this list and plays on the cell it strikes. The height of the first entry's image also sets how high the clouds hang. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
WeatherConBolts=MYBOLT1,MYBOLT2 ; AnimTypes registered in [Animations]
```

With the list empty, the strikes still deal damage, with no bolt drawn.
