---
key: SlaveMinerScanCorrection
scope: global-rules
label: 'Minimum miner relocation distance'
see_also: [SlaveMinerShortScan, SlaveMinerLongScan, "system:slave-miners"]
when_omitted:
  kind: value
  value: "3"
---

A deployed slave miner whose nearby ore has run out packs up only when its new place to deploy is more than this many cells from the middle of its right edge. A fraction of a cell is dropped.
