---
title: Send only the end of report logs over 8 MB, and warn about chat text
category: fix
release: 0.2.0
targets:
- type: system
  id: out-of-sync-recovery
  effect: changed
credit:
- MentalHomiega
---

A log over 8 MB is sent as its last 4 MB, starting at a whole line. The dialog shows the size sent and the size of the whole file. Any other file over 8 MB, such as a minidump, is left out without being read.

The Ask dialog also says that crash dumps and logs can contain chat text and folder names.
