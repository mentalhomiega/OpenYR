---
key: Lightning.Bolts
scope: superweapontype
summary: "Lists the bolt animations a lightning storm can use."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] WeatherConBolts"
---

Each bolt is one animation from the list, chosen at random. An empty list, written `none`, still deals the damage but shows no bolt.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Bolts=WCLBOLT1,WCLBOLT2
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
