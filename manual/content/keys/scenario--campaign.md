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

A value may start with `RA2->`: `RA2->ALL01T.MAP` starts the map `ALL01T.MAP`. The game finds the map by its file name in the archives it searches, so the prefix does not limit where the map is read from. A mission with the prefix reads its movie indexes from Red Alert 2's movie list, not Yuri's Revenge's.

If the file cannot be read, the game shows an error message and returns to the menu. An empty value fails the same way.

Only the first mission is named here. [Choosing the next mission](/systems/campaign-progression/#choosing-the-next-mission) covers how each won mission leads to the next, through the stage's first choice or the mission's [`NextScenario`](/keys/nextscenario/). [Campaign progression](/systems/campaign-progression/) covers the whole sequence and the state carried between missions.

When the campaign's `CD` is above `1`, this value also decides its loading-screen artwork; see [`CD`](/keys/cd/).
