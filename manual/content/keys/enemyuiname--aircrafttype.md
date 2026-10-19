---
key: EnemyUIName
scope: aircrafttype
summary: "The name a player who is not an ally sees over an object of this type."
see_also: [UIName, Nominal]
when_omitted:
  kind: value
  value: "none, so such a player sees the generic enemy text"
---

The hover text over an object whose owner does not share its view with the player reads the string that `EnemyUIName` names, in place of the generic text for an enemy soldier, vehicle or structure. A spectator, and a player whose spy has infiltrated the structure, see the type's real name. The value is a string label, or text after `NOSTR:`, as for [`UIName`](/keys/uiname/).

```ini title="rulesmd.ini"
[YACXST] ; example BuildingType
EnemyUIName=NOSTR:Construction Yard
```
