---
title: Stop an object from cloaking after a warhead hit
category: feature
release: 0.2.0
targets:
- type: key
  id: Sonar.Duration
  effect: added
credit: [MentalHomiega]
---

A warhead with `Sonar.Duration=` decloaks the objects it hits and keeps them from cloaking for that many frames. A longer cloak delay is no longer shortened when an object finishes uncloaking, so a sonar pulse's `SonarPulse.Delay` also holds for its full length.
