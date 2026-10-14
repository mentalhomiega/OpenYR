---
enum_id: RadarEventType
slug: radar-event
title: Radar event
summary: Radar-notification kinds used by trigger actions and by the engine's radar pings.
representation: integer
bindings:
  key_value_types: []
  scripting_parameter_types: [radar-event]
source_files: [code/revent.hh]
values:
  - { constant: RADAREVENT_COMBAT, value: 0, input: "0", meaning: "Combat event. Magenta. Trigger action only." }
  - { constant: RADAREVENT_NONCOMBAT, value: 1, input: "1", meaning: "Non-combat event. Yellow. Trigger action only." }
  - { constant: RADAREVENT_DROPZONE, value: 2, input: "2", meaning: "Drop-zone event. Yellow. Trigger action only." }
  - { constant: RADAREVENT_BASE_ATTACKED, value: 3, input: "3", meaning: "Base-under-attack event. Magenta." }
  - { constant: RADAREVENT_HARVESTER_ATTACKED, value: 4, input: "4", meaning: "Harvester-under-attack event. Magenta." }
  - { constant: RADAREVENT_ENEMY_SENSED, value: 5, input: "5", meaning: "Enemy-detected event. Cyan." }
  - { constant: RADAREVENT_FACTORY_OUTPUT, value: 6, input: "6", meaning: "Object completed at a factory. Not drawn." }
  - { constant: RADAREVENT_UNIT_LOST, value: 7, input: "7", meaning: "Own unit destroyed. Not drawn." }
  - { constant: RADAREVENT_REPAIR, value: 8, input: "8", meaning: "Repair event. Not drawn." }
  - { constant: RADAREVENT_INFILTRATED, value: 9, input: "9", meaning: "Building infiltrated. Not drawn." }
  - { constant: RADAREVENT_OWNER_CHANGED, value: 10, input: "10", meaning: "Building changed owner. Not drawn." }
  - { constant: RADAREVENT_BEACON, value: 11, input: "11", meaning: "Beacon event. Yellow." }
  - { constant: RADAREVENT_BUILDING_PLACED, value: 12, input: "12", meaning: "Building placed. Yellow." }
  - { constant: RADAREVENT_SUPERWEAPON, value: 13, input: "13", meaning: "Superweapon event. Not drawn." }
  - { constant: RADAREVENT_FACTORY_FOUND, value: 14, input: "14", meaning: "Factory found. Not drawn." }
  - { constant: RADAREVENT_MIND_CONTROL, value: 15, input: "15", meaning: "Structure-abandoned event. Raised when the last occupant leaves a garrisonable structure the player owns. Not drawn." }
  - { constant: RADAREVENT_ALLY_ATTACKED, value: 16, input: "16", meaning: "Ally-under-attack event. Not drawn." }
---

A radar event is a box on the radar map that closes in on one cell to draw the player's eye there. Kinds 0 to 16 match the values Yuri's Revenge uses, so a map written for that game keeps its meaning.

Each kind has a fixed suppression distance, visibility, duration and suppression flag. The engine uses these values and ignores the three `[General]` lists that older builds read for them:

| Kind | Suppressible | Suppresses within (cells) | Marker frames | Stays in play (frames) |
| --- | --- | --- | --- | --- |
| 0 Combat | yes | 8 | 200 | 400 |
| 1 Noncombat | no | 8 | 200 | 400 |
| 2 Drop zone | no | 8 | 200 | 400 |
| 3 Base attacked | yes | 8 | 200 | 600 |
| 4 Harvester attacked | yes | 8 | 200 | 400 |
| 5 Enemy sensed | yes | 6 | 200 | 400 |
| 6 Factory output | yes | 2 | 0 | 200 |
| 7 Unit lost | yes | 8 | 0 | 200 |
| 8 Repair | yes | 2 | 0 | 400 |
| 9 Infiltrated | no | 5 | 0 | 400 |
| 10 Owner changed | no | 8 | 0 | 100 |
| 11 Beacon | yes | 8 | 200 | 200 |
| 12 Building placed | no | 8 | 200 | 400 |
| 13 Superweapon | no | 8 | 0 | 5 |
| 14 Factory found | yes | 8 | 0 | 200 |
| 15 Structure abandoned | yes | 8 | 0 | 400 |
| 16 Ally attacked | yes | 8 | 200 | 600 |

A suppressible kind is refused when an event of the same kind, still in play, lies closer than its suppression distance in cells measured in a straight line. Events of any owner count. The refused call returns false, and the EVA line that depends on it does not play. A marker is the box's drawn outline after it settles, and it is drawn only for a kind with a colour. Events leave the list when their duration runs out, and the newest one sets the cell for the space-bar jump.

The kind sets the box's colors. Kinds 0, 3 and 4 pulse between magenta and purple, kinds 1, 2, 11 and 12 between yellow and olive, and kind 5 between cyan and teal. A kind with no colour is never drawn, but it still suppresses and still counts for the space-bar jump.

A map can raise any kind with the [Create Radar Event](/mapping/actions/taction-radar-event/) trigger action. Kinds 0, 1 and 2 come only from trigger actions. The engine raises kinds 3 to 16 at the points where Yuri's Revenge raises them.
