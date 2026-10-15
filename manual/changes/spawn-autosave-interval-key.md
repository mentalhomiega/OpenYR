---
title: Read the autosave interval a CnCNet launch file writes
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- MentalHomiega
---

The CnCNet client writes the Auto Save box as `AutoSaveInterval` in the `[Settings]` section of the launch file: 7200 when the box is checked, 0 when it is not. The game read only `AutoSaveGame`, so a match started by the client never saved automatically. The game now reads `AutoSaveInterval` as well. A file that writes both keys uses `AutoSaveGame`.

This change raised the launch file's schema version, which feeds the session identity, so a machine on an earlier build does not share a match with this one.
