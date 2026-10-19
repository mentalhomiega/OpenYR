---
title: Convert a vehicle when it deploys
category: feature
release: 0.2.0
targets:
- type: key
  id: Convert.Deploy
  effect: added
credit:
- MentalHomiega
---

A simple deployer with `Convert.Deploy=` becomes the named vehicle type when it finishes deploying, with the same share of its health, and can convert back with another deploy order. The key follows the Ares documentation, and a vehicle that sets no `Convert.Deploy` behaves as before.
