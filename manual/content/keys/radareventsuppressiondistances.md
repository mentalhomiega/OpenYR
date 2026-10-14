---
key: RadarEventSuppressionDistances
summary: Read and ignored. The cells within which a radar event blocks a later one of its kind are built in.
see_also: ["system:map-visibility", RadarEventDurations]
when_omitted:
  kind: value
  value: ""
  note: Nothing changes. The built-in values apply.
---

OpenYR reads this list and does not use it. The routine that decides whether a new [radar event](/reference/enums/radar-event/) is refused reads the built-in table on that page, so a rules file that sets this list has no effect.

A new event of a suppressible kind is refused when an event of the same kind, still in play, lies closer than that kind's built-in distance, in cells measured in a straight line. An event stays in play, and goes on suppressing, until its built-in duration runs out, even after it has stopped being drawn. Each EVA line that goes with an event plays only when that event is created, so a refused event also silences its line.
