---
key: RadarEventVisibilityDurations
summary: Read and ignored. The frames a settled radar event stays drawn are built in.
see_also: ["system:map-visibility", RadarEventDurations]
when_omitted:
  kind: value
  value: ""
  note: Nothing changes. The built-in values apply.
---

OpenYR reads this list and does not use it. The routines that create, update and draw [radar events](/reference/enums/radar-event/) read the built-in table on that page, so a rules file that sets this list has no effect. The built-in marker frames are 200 for kinds 0 to 5, 11, 12 and 16, and 0 for the rest.
