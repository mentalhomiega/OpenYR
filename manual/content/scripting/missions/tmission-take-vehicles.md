---
type: mission
id: TMISSION_TAKE_VEHICLES
title: Take vehicles
summary: Holds the line number Ares gives Take vehicles; the team does nothing on it.
caveats:
  - This is Ares line 67. We keep the number and the editor name so that a script written for Ares keeps its numbering, and we do not run the line.
  - A team that reaches the line stays on it and takes no later line, as it does for any number the game does not run. A script written for us does not use it.
related:
  - type: system
    id: ai-team-execution
  - type: action
    id: TACTION_KILL_DRIVERS_OF
---
