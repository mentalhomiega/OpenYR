---
title: Check that an AI trigger's teams can reach the enemy
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-team-production
  effect: changed
credit: [MentalHomiega]
---

Before, a computer house raised an AI trigger's teams whatever lay between the two bases. Now each TeamType the trigger names must reach the enemy: a land team needs both bases in one zone of its movement zone, and a team of naval transports needs both bases in one `Amphibious` zone but in different zones of its own. Naval warship teams and teams marked `IsBaseDefense=yes` are still raised without the check.
