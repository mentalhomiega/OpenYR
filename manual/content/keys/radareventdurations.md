---
key: RadarEventDurations
summary: Read and ignored. The frames each radar event kind stays in play are built in.
see_also: ["system:map-visibility", RadarEventVisibilityDurations, RadarEventSuppressionDistances]
when_omitted:
  kind: value
  value: ""
  note: Nothing changes. The built-in values apply.
---

OpenYR reads this list and does not use it. The routines that create, update, draw and remove [radar events](/reference/enums/radar-event/) read the built-in table on that page instead, so a rules file that sets this list has no effect. The built-in duration of a kind is the number of frames it stays in play after it settles: 400 for combat, 600 for base attacked, 5 for superweapon.
