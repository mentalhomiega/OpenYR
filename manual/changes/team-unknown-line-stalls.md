---
title: Keep a team on a script line it cannot run, and after Change script
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_SCRIPT
  effect: changed
- type: system
  id: ai-team-execution
  effect: changed
credit:
- MentalHomiega
---

A team on a script line with a number the engine does not know, or whose script has just been replaced by Change script, now stays where it is, as it does in gamemd. Before this the team went on to the next line at once, so a Change script line started the new script from its first line.
