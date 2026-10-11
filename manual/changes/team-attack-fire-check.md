---
title: Have the team Attack line pick another target when the leader cannot fire at it
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_ATTACK
  effect: changed
credit:
- MentalHomiega
---

A team on an Attack line now drops its target on the turn after the leader finds, on one of its checks every eighth frame, that it can never fire at it, and the line picks another target. A Droppod team's members that are still above the map count as able to attack, so the line does not end while they land. Before this the team stayed on a target its leader could never hit until the target died, and a drop pod team's line could end while its members were still landing.

The retarget flag is kept in a save, so the save revision went up.
