---
title: Scale kill value by the victim's rank
category: fix
release: 0.2.0
targets:
- type: system
  id: veterancy
  effect: changed
credit: [MentalHomiega]
---

We now count the victim's cost doubled when the victim was a veteran and tripled when it was elite, as Yuri's Revenge does. The killer's experience and the killer's house score both use that value, so a kill of an elite object scores three times its cost. We no longer give a kill of an allied object any score, where it previously added the victim's cost.
