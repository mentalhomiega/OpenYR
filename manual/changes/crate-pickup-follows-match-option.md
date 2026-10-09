---
title: Replace a collected crate whenever crates are on for the match
category: fix
release: 0.2.0
credit: [MentalHomiega]
targets:
- type: key
  id: Crates
  effect: changed
- type: system
  id: crates
  effect: changed
---

We now replace a collected crate whenever crates are on for the match. Before, the rules' `Crates` key also had to be `yes`, so a match with `Crates=no` in the rules dropped the replacement even when the launch file or the lobby turned crates on. The decompiled game replaces a collected crate under the match option alone.
