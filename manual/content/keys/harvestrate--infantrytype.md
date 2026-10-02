---
key: HarvestRate
scope: infantrytype
label: 'Shovel delay'
see_also: [Slaved, "system:slave-miners"]
when_omitted:
  kind: value
  value: "0"
---

The number of frames a slave shovels before it picks up one bail of ore from its cell. It keeps shoveling until it carries its `Storage` or the cell runs out.
