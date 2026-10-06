---
title: Crash airport bound aircraft that have no airfield
category: feature
release: 0.2.0
targets:
- type: key
  id: AirportBound
  effect: added
- type: system
  id: aircraft-operations
  effect: changed
- type: key
  id: Landable
  effect: changed
credit:
- MentalHomiega
---

`AirportBound=yes` on an aircraft type stops its aircraft from landing in the open, as the Harrier and Black Eagle do in Yuri's Revenge. An airborne aircraft with no structure to dock at crashes. A move order now sends such an aircraft to the cell and then to a docking structure, where it previously landed on the spot. Any aircraft with `Dock=` buildings that finishes a move in the air now looks for a bay too. Saves made by earlier builds no longer load, because aircraft types now save the key.
