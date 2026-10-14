---
title: Let a wreck keep its passengers aboard
category: feature
release: 0.2.0
targets:
- type: key
  id: WreckReleasesPassengers
  effect: added
credit:
- MentalHomiega
---

`WreckReleasesPassengers` is a new vehicle-type key in `rules.ini`. It decides whether a vehicle with [`DeathFrames`](/keys/deathframes/) releases its passengers when it becomes a wreck. `no` keeps them aboard until the wreck explodes, which is what earlier builds did. Saves made by earlier builds no longer load, because unit types now save the setting.
