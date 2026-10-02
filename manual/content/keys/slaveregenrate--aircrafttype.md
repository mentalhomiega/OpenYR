---
key: SlaveRegenRate
scope: aircrafttype
label: 'Lost slave replacement delay'
see_also: [SlaveReloadRate, "system:slave-miners"]
when_omitted:
  kind: value
  value: "0"
---

The number of frames after one of its slaves is killed before the object has a new one. The new slave waits inside until its owner is a structure.
