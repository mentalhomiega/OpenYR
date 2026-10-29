---
title: Let the first selected unit answer a group order
category: fix
release: 0.2.0
targets:
- type: key
  id: VoiceMove
  effect: changed
- type: key
  id: VoiceAttack
  effect: changed
credit: [MentalHomiega]
---

When a group is ordered to move, the unit that answers is now the first one that was selected, as in Yuri's Revenge. Before, a group move was answered by whichever unit stood nearest the middle of the group, so a mixed group could answer with the wrong unit's voice. The `MoveFlash` marker still appears once, on the clicked cell.
