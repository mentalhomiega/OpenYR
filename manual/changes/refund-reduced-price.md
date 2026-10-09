---
title: Refund a structure's reduced price when sold
category: fix
release: 0.2.0
targets:
- type: key
  id: RefundPercent
  effect: changed
credit: [MentalHomiega]
---

Selling a structure with a [`FreeUnit=`](/keys/freeunit/) refunds its price less the unit it gives, so a refinery with a 1400-credit harvester refunds 600 credits' worth, not 2000. The difficulty's and the country's `Cost=` no longer enter the refund; the country's multiplier for the structure's category still does. A free unit that cannot be placed is refunded in full, without `RefundPercent`.
