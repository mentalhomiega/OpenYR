---
title: A primary factory stays primary when clicked again
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

Clicking a factory that is already its house's primary no longer clears its primary status. Before, one click on the primary factory made it non-primary, and a later click set it primary again. Making another factory primary works as before.
