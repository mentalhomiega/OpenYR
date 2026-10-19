---
key: LowSelectionPriority
scope: aircrafttype
summary: "With yes, a drag selection leaves this object out when it caught others."
see_also: [PrioritySelectionFiltering, Selectable]
when_omitted:
  kind: value
  value: "no"
---

A selection box that catches at least one object without `LowSelectionPriority=yes` selects those and leaves the flagged ones out. A box that catches only flagged objects selects them all. Clicking a flagged object, and selecting by type or by group, are not changed. The [`PrioritySelectionFiltering`](/keys/priorityselectionfiltering/) setting turns the rule off for a player.

```ini title="rulesmd.ini"
[ASW] ; example AircraftType
LowSelectionPriority=yes
```
