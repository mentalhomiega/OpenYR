---
title: Create computer teams before their members are buildable
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-team-production
  effect: changed
credit:
- MentalHomiega
---

A computer house now creates a team from an AI trigger when a powered factory of its own can produce every member's kind, even if it does not yet meet a member's prerequisites or tech level. Before, it also required every member to be buildable at that moment, so a trigger waited until the whole roster was unlocked. The house then builds the members it can and recruits the rest later, and the team does not start until it is full.
