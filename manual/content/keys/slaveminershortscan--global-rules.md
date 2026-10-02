---
key: SlaveMinerShortScan
scope: global-rules
label: 'Deployed miner ore radius'
see_also: [SlaveMinerLongScan, SlaveMinerScanCorrection, "system:slave-miners"]
when_omitted:
  kind: value
  value: "5"
---

A deployed slave miner stays where it is while ore lies closer than this many cells. The distance is measured from the middle of the structure's right edge, and a fraction of a cell is dropped. A player's idle mobile miner also sets out by itself once ore lies closer than this to it; see [`SlaveMinerKickFrameDelay`](/keys/slaveminerkickframedelay/#scope-global-rules).
