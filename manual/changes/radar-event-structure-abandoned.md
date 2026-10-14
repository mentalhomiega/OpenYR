---
title: Raise a radar event when a player's structure is left empty
category: fix
release: 0.2.0
targets:
- type: enum
  id: RadarEventType
  effect: changed
credit:
- MentalHomiega
---

When the last occupant leaves a garrisonable structure the player owns, the structure passes to the Civilian side and a radar event is raised at it. Only a structure with `TechLevel=-1` qualifies, so a structure the player can build is left alone. The event's box closes in on the structure on the radar in plain pixels, and the space-bar jump reaches the structure.

The structure-abandoned line plays only when that event is created. A second structure left empty within 8 cells of the first, while the first event is still in play, raises no event and its line stays silent. The building-abandoned sound still plays. Before, every such structure played the line and raised no event.
