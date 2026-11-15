---
key: Lightning.Clouds
scope: superweapontype
summary: "Lists the cloud animations a lightning storm can use."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] WeatherConClouds"
---

Each cloud is one animation from the list, chosen at random. An empty list, written `none`, gathers no clouds, so the storm strikes nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Clouds=WCCLOUD1,WCCLOUD2
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
