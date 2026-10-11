---
title: End the team Attack line on a target the leader can never fire at
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_ATTACK
  effect: changed
credit:
- MentalHomiega
---

A team on an Attack line now moves on every eighth frame the leader cannot aim its weapon at the team's target at all, and a Droppod team's members that are still above the map count as able to attack. Before this the team stayed on a target its leader could never hit until the target died, and a drop pod team's line could end while its members were still landing.
