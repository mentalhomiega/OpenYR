---
title: Play the Construction sound for a structure with no BuildupSound
category: fix
release: 0.2.0
targets:
- type: key
  id: Construction
  effect: changed
credit:
- MentalHomiega
---

Before, the rules key `Construction` was read and never played. A structure that has no `BuildupSound` now plays it as its construction begins, as it does in Yuri's Revenge. The stock rules name a sound there that does not exist, so the stock game stays silent. A mod that names a real sound hears it for every structure without its own `BuildupSound`.
