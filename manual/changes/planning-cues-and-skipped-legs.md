---
title: Play planning-mode cues and skip legs whose target is gone
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
credit: [MentalHomiega]
---

Turning planning mode on plays the `StartPlanningModeSound` sound and shows a message once per scenario. Turning it off plays `EndPlanningModeSound`, shows another message once per scenario and starts the queued plans. Both sounds are `[AudioVisual]` rules keys and default to no sound. Loading a save plays neither.

When a plan reaches a leg whose object target or object destination is gone (dead, inactive or in limbo), that leg is dropped and the next leg runs. Before, the whole rest of the plan was dropped. A unit that is itself dead, inactive or in limbo still drops its whole plan.
