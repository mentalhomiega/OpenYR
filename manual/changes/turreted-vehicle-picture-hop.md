---
title: Stop turreted vehicles shaking by a pixel while they turn
category: fix
release: 0.2.0
targets: []
credit: [MentalHomiega]
---

A vehicle with a turret could be drawn one pixel too low or too far right, depending on whether its picture was an odd number of pixels tall or wide on that facing, so it shook while turning. Its picture is now placed the same way whatever its size.
