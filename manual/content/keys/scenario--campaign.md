---
key: Scenario
scope: campaign
label: Campaign opening mission
see_also: ["CD", "Description", "FinalMovie", "NextScenario"]
when_omitted:
  kind: value
  value: ""
  note: An empty name, which starts nothing.
---

The value is the map file of the campaign's first mission. Choosing the campaign from the mission list starts that file. Write the path with its extension, as the shipped campaigns do with `Maps/Missions/GDI1A.MAP`.

A value may start with `RA2->`, as in `RA2->ALL01T.MAP`. The prefix is removed before the file name is used, so the map is found by `ALL01T.MAP` alone, as it would be without the prefix. The prefix also makes the movie that a mission's trigger plays get its index from Red Alert 2's movie list rather than Yuri's Revenge's. Any other prefix stays part of the file name.

If the file cannot be read, the game shows an error message and returns to the menu. An empty value fails the same way.

Only the first mission is named here. [Choosing the next mission](/systems/campaign-progression/#choosing-the-next-mission) covers how each won mission leads to the next, through the stage's first choice or the mission's [`NextScenario`](/keys/nextscenario/). [Campaign progression](/systems/campaign-progression/) covers the whole sequence and the state carried between missions.

When the campaign's `CD` is above `1`, this value also decides its loading-screen artwork; see [`CD`](/keys/cd/).
