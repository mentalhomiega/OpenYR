---
type: action
id: TACTION_IRON_CURTAIN_AT
title: "Iron Curtain At..."
summary: "Puts everything around a waypoint under the Iron Curtain."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The invoke animation `IronCurtainInvokeAnim` plays over the waypoint. Every object on the waypoint's cell and the eight cells around it is protected for `IronCurtainDuration`, whoever owns it."
  - "The Iron Curtain's usual kills apply. Infantry in the area die, and so do vehicles that are organic."
  - "The kills are credited to no house."
  - "The action fails when the waypoint does not exist. No super weapon, house or power is involved, so the house that owns the trigger does not matter."
related:
  - type: action
    id: TACTION_LIGHTNING_STORM_STRIKE
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Curtain=1,109,0,0,0,0,0,0,C ; the Iron Curtain covers the area around waypoint 2
```
