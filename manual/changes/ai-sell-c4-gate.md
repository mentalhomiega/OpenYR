---
title: Refuse a computer sale only for a structure with a C4 charge
category: fix
release: 0.2.0
targets:
- type: system
  id: repair
  effect: changed
credit:
- MentalHomiega
---

A computer house no longer keeps a damaged structure because a delay-kill fuse is running on it. Only a structure with a C4 charge applied is refused. Before, the fuse and the charge both blocked the sale. The save revision moves to 31, so saves from earlier builds are not compatible.
