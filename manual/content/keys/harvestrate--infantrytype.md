---
key: HarvestRate
scope: infantrytype
label: 'Shovel delay'
see_also: [Slaved, "system:slave-miners"]
when_omitted:
  kind: value
  value: "0"
---

The number of frames between the bails of ore a slave picks up from its cell. It takes the first bail as soon as it stands on ore, then one every `HarvestRate` frames until it carries its `Storage` or the cell runs out.
