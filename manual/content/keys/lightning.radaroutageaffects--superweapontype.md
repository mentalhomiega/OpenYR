---
key: Lightning.RadarOutageAffects
scope: superweapontype
summary: "Chooses which houses a lightning storm jams radar for."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "enemies"
---

A list of `owner`, `allies`, `team`, `enemies`, `all` and `none`, relative to the house that fired the storm. A storm that no house fired counts every house as an enemy. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.RadarOutageAffects=enemies,allies
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
