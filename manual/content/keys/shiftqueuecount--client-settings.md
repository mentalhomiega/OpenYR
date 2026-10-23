---
key: ShiftQueueCount
scope: client-settings
label: Shift-click queue count
see_also: [InterfaceStyle, MaximumQueuedObjects]
when_omitted:
  kind: value
  value: "5"
  note: Five units per Shift-click, as the modern build queues them. Set it to 1 for the one unit a click queues in Yuri's Revenge.
---

`ShiftQueueCount=` sets how many units one Shift-click on a unit or infantry cameo queues. It is written in `[Options]` of `RA2MD.INI`. A value below 1 acts as 1.

The count applies only when [`InterfaceStyle`](/keys/interfacestyle/) is `Modern`. The classic sidebar queues one unit per click, and buildings build one at a time whatever the value.

The count stops at the places the factory has left. A factory holds one unit in production and up to [`MaximumQueuedObjects`](/keys/maximumqueuedobjects/) behind it. A Shift-click asks for fewer units when fewer places are left, and one click that finds the queue full is refused with the scold sound, as a plain click is.

Saved games do not store the value. It is read when the game starts.
