---
title: Run Yuri's Revenge actions 123 and 124, Mind Control Base and Restore Mind Controlled Base
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_MIND_CONTROL_BASE
  effect: changed
- type: action
  id: TACTION_RESTORE_MIND_CONTROLLED_BASE
  effect: changed
credit:
- MentalHomiega
---

Action 123 gives the trigger's house every structure of another house, and action 124 gives them back. Neither announces a capture or springs an "entered by" trigger. Both did nothing before.

A structure taken this way now remembers its former owner and the house that holds it, so the save revision went up.
