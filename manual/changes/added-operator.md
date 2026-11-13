---
title: Shut objects down without an operator aboard (Operator)
category: feature
release: 0.2.0
targets:
- type: key
  id: Operator
  effect: added
credit: [MentalHomiega]
---

A type can now name the passenger types it needs in `Operator`, or `_ANY_` for any passenger, as Ares does. The object shuts down while no accepted passenger is aboard. Types that leave the key out are unchanged.
