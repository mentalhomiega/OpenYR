---
key: IsSelectableCombatant
summary: Lets the Select View command select a structure of this type along with the player's units.
see_also: [Selectable, UndeploysInto, "system:band-selection"]
when_omitted:
  kind: value
  value: "no"
---

The Select View command selects every selectable object of the player's whose drawing position is inside the tactical view. A structure joins that selection only if it can [undeploy](/keys/undeploysinto/) and is not a construction yard, or its type sets `IsSelectableCombatant=yes`. A structure of an `IsSelectableCombatant=yes` type is taken whether or not it undeploys, and a construction yard of that type is taken too. The structure must still be [`Selectable=yes`](/keys/selectable/) and owned by the player.

A box dragged across the view takes such a structure too; [band selection](/systems/band-selection/#what-the-box-takes) lists what a box takes.

The key is read for every kind of object but changes only structures; a soldier, vehicle or aircraft that sets it behaves as before.

```ini title="rulesmd.ini"
[MYTURRET] ; example BuildingType
Selectable=yes
IsSelectableCombatant=yes
```
