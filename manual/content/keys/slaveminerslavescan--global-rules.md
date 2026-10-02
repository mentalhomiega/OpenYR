---
key: SlaveMinerSlaveScan
scope: global-rules
label: 'Slave ore search radius'
see_also: [Enslaves, "system:slave-miners"]
when_omitted:
  kind: value
  value: "16"
---

How far, in cells, a slave looks for ore from where it stands; it finds only ore closer than this. A fraction of a cell is dropped. A slave that finds none closer than this goes back to its miner.
