---
title: Set a fixed sale refund with Soylent
category: feature
release: 0.2.0
targets:
- type: key
  id: Soylent
  effect: added
credit: [ZivDero, CCHyper, tomsons26]
---

[`Soylent=`](/keys/soylent/) on an object type sets the credits a sale of that type refunds, in place of the share of its price that [`RefundPercent`](/keys/refundpercent/) gives. The amount is scaled by the owner's country multiplier for the type's category, and the default `0` keeps the usual refund.
