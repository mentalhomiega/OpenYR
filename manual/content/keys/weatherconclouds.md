---
key: WeatherConClouds
summary: "The cloud animations of a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each cloud of a lightning storm is picked at random from this list. It hangs as high above its cell as half the height of the first [`WeatherConBolts`](/keys/weatherconbolts/) image, and its bolt strikes halfway through its animation. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
WeatherConClouds=MYCLOUD1,MYCLOUD2 ; AnimTypes registered in [Animations]
```

With the list empty, a storm gathers no clouds and strikes nothing.
