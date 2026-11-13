---
type: action
id: TACTION_KILL_DRIVERS_OF
title: Kill Drivers Of
summary: Takes the drivers of the vehicles attached to the trigger and hands the vehicles to another house.
valid_values:
  - The first number is the house that receives the vehicles, as the map's house list numbers it. `-1` means the house named Special.
caveats:
  - This is an Ares action, number 147, and the trigger must be attached to the vehicles.
  - A vehicle keeps its driver when its type is Organic or Natural, has `ProtectedDriver=yes` and is healthier than its `ProtectedDriver.MinHealth`, or has the `PROTECTED_DRIVER` ability. A vehicle under the Iron Curtain or a force shield keeps it too.
  - The passengers climb out, and the vehicle changes house without the announcement of a capture. Its rank stays.
  - Infantry, aircraft and structures carrying the trigger are not touched.
related:
  - type: key
    id: KillDriver
  - type: key
    id: ProtectedDriver
  - type: action
    id: TACTION_CHANGE_HOUSE
---

```ini title="map file"
[Actions]
Strip=1,147,0,-1,0,0,0,0,A ; the attached vehicles lose their drivers and go to Special
```
