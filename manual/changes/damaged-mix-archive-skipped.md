---
title: Skip a damaged MIX archive instead of mounting it
category: fix
release: 0.2.0
targets:
- type: format
  id: mix
  effect: changed
credit:
- MentalHomiega
---

A MIX archive that is shorter than its header and index, or that states a negative member count or data size, is now skipped when the game mounts it. Before, such an archive could crash the game or join the search with meaningless entries. A member that reaches past the end of the file is now left out, so a request for it goes on to the next archive.
