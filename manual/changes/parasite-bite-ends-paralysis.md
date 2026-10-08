---
title: End paralysis when a parasite bite has no Paralyzes
category: fix
release: 0.2.0
targets:
- type: key
  id: Paralyzes
  effect: changed
credit: [MentalHomiega]
---

We now end the victim's paralysis when a parasite bites with a warhead whose `Paralyzes` is `0`, as Yuri's Revenge does. Before, such a bite left the victim's paralysis in place.
