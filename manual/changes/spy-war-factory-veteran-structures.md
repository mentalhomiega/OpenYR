---
title: Make deployable structures veterans after a war factory spy
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

After a spy enters a war factory or a shipyard, we make each trainable, non-naval structure that has an `UndeploysInto` type a veteran when it is created, as in Yuri's Revenge. The Yuri refinery is one such structure. We used to give these structures no veterancy from the spy.
