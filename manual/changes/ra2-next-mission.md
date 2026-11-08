---
title: Continue a Red Alert 2 campaign with the next mission of the campaign list
category: fix
release: 0.2.0
targets:
- type: key
  id: Scenario
  scope: campaign
  effect: changed
credit:
- MentalHomiega
---

After the player wins a mission of a campaign whose `Scenario=` has the `RA2->` prefix, the game starts the mission of the next entry in the campaign list. Before, it read the `NextScenario=` of the map, which names a Tiberian Sun map that does not exist, and ended at an error box. The campaign ends, and the game returns to the menu, when the next entry is not a Red Alert 2 mission whose map exists, such as the header row of the other side.
