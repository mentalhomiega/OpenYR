---
key: SlaveMinerKickFrameDelay
scope: global-rules
label: 'Idle mobile miner delay'
see_also: [SlaveMinerShortScan, "system:slave-miners"]
when_omitted:
  kind: value
  value: none
---

The number of frames a mobile slave miner on its guard or harvest mission waits before it drives to ore and deploys by itself. The count starts when the miner is created, when it becomes a vehicle again, and when it last failed to find a place to deploy. When the key is omitted, miners never set out by themselves.

A computer player's miner sets out once the delay has passed. A human player's miner sets out only if it is also standing on ore or has ore closer than [`SlaveMinerShortScan`](/keys/slaveminershortscan/#scope-global-rules) cells.
