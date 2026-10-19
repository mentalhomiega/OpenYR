---
key: SW.AffectsHouse
scope: superweapontype
summary: "Chooses whose objects an iron curtain or sonar pulse affects."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "All houses; enemies for a sonar pulse"
---

A list of `owner`, `allies`, `team`, `enemies`, `all` and `none`, relative to the firing house. A `Type=IronCurtain` weapon protects only the objects of those houses. A `Type=SonarPulse` weapon decloaks only those houses' objects. A `Type=ForceShield` weapon shields only structures of those houses, in place of the owner and its allies. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AffectsHouse=team
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
