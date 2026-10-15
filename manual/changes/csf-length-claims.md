---
title: Read a string table whose lengths run past its end
category: fix
release: 0.2.0
targets:
- type: format
  id: csf
  effect: changed
credit:
- MentalHomiega
---

A string table whose label or string states a length longer than the rest of the file now ends the table at that record, like any other cut record. Before, a length of gigabytes made the game try to reserve that much memory and fail. A header with a negative label or string count is now refused instead of loading an empty table.
