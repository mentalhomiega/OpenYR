---
key: SlaveMinerLongScan
scope: global-rules
label: 'Miner relocation search radius'
see_also: [SlaveMinerShortScan, SlaveMinerScanCorrection, "system:slave-miners"]
when_omitted:
  kind: value
  value: "80"
---

How many cells a slave miner looks for ore when it chooses a place to deploy, either as a vehicle setting out or as a structure whose nearby ore has run out. A fraction of a cell is dropped. A miner that finds no ore closer than this stays where it is.
