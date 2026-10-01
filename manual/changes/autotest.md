---
title: Run unattended tests with -AUTOTEST
category: internal
release: 0.2.0
targets:
- type: command
  id: launch:autotest
  effect: added
credit: [Lucas]
---

`-AUTOTEST=<script>` plays a test script inside the engine with the window hidden and the real mouse and keyboard ignored, so a test can run while someone uses the computer. docs/AUTOTEST.md describes the script.
