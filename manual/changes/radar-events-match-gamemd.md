---
title: Radar events use Yuri's Revenge's seventeen kinds and suppression rules
category: fix
release: 0.2.0
targets:
- type: enum
  id: RadarEventType
  effect: changed
- type: action
  id: TACTION_RADAR_EVENT
  effect: changed
- type: key
  id: RadarEventDurations
  effect: changed
- type: key
  id: RadarEventVisibilityDurations
  effect: changed
- type: key
  id: RadarEventSuppressionDistances
  effect: changed
credit:
- MentalHomiega
---

Radar events now use Yuri's Revenge's seventeen kinds and their built-in durations, distances, colours and suppression flags. A new event is refused when one of the same kind, from any house, lies closer than its kind's distance, and the EVA line that goes with it then stays silent. The beacon event is kind 11 and is yellow, not non-combat. The space-bar jump returns to the newest of the last eight event cells, and a second press within 1.6 seconds steps back through them.

The `RadarEventDurations`, `RadarEventVisibilityDurations` and `RadarEventSuppressionDistances` lists in `[General]` are read but no longer change the game. Yuri's Revenge's event code uses built-in values for these, so base-attacked events last 600 frames, where the reference rules list says 400.
