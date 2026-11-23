---
title: Jump Camera Home and CenterBase go to the base center
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_JUMP_CAMERA_HOME
  effect: changed
- type: command
  id: CenterBase
  effect: changed
credit:
- MentalHomiega
---

Action 145 and the `CenterBase` key now choose the view as Yuri's Revenge does. With several construction yards and none primary they take the earliest listed, not the latest. A yard is a structure with `ConstructionYard=yes`. With no yard the view goes to the base center, including one a Center Base Cell action set, instead of the first other structure.
