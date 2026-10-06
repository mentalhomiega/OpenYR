---
title: Read extra string tables and literal NOSTR names
category: feature
release: 0.2.0
targets:
- type: format
  id: csf
  effect: changed
- type: key
  id: UIName
  effect: changed
credit:
- MentalHomiega
---

The game reads `STRINGTABLE00.CSF` through `STRINGTABLE99.CSF` after `RA2MD.CSF`, and each adds labels or replaces loaded ones. A table in another language than `RA2MD.CSF` is skipped unless it is language-neutral. A `UIName=` value starting with `NOSTR:` is shown as written instead of being looked up. Both follow the Ares documentation's string table enhancements.
