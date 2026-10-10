---
type: action
id: TACTION_LIGHTNING_STORM_STRIKE
title: "Lightning Storm strike..."
summary: "Calls a lightning storm over a waypoint for the trigger's house."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The storm is the one the Lightning Storm super weapon calls. It breaks [`LightningDeferment`](/keys/lightningdeferment/) frames later and lasts [`LightningStormDuration`](/keys/lightningstormduration/) frames."
  - "The trigger's house is the storm's owner: houses that are not its allies lose their radar while the storm rages."
  - "The action fails while a storm is raging, or when the waypoint does not exist. A storm that has been called but has not yet broken does not stop another call."
  - "No super weapon is used, so the trigger's house needs no Lightning Storm, power or charge."
related:
  - type: action
    id: TACTION_IRON_CURTAIN_AT
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Storm=1,102,0,0,0,0,0,0,C ; a lightning storm over waypoint 2
```
