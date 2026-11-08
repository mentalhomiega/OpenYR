---
title: Run unattended tests without the frame limit
category: internal
release: 0.2.0
targets:
- type: command
  id: launch:autotest-uncapped
  effect: added
- type: command
  id: launch:autotest-stop-after-steps
  effect: added
credit: [MentalHomiega]
---

`-AUTOTEST-UNCAPPED` removes the frame limit from a test run, and `-AUTOTEST-STOP-AFTER-STEPS` ends it after the script's last step. docs/AUTOTEST.md lists what differs from a run at normal speed.
