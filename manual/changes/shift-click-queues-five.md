---
title: Queue five units with a Shift-click on a cameo
category: feature
release: 0.2.0
targets:
- type: key
  id: ShiftQueueCount
  effect: added
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

In the modern interface, a Shift-click on a unit or infantry cameo queues five of that unit. [`ShiftQueueCount=`](/keys/shiftqueuecount/) sets the number, and `1` restores the single unit a click queues in Yuri's Revenge. Buildings still build one at a time, and the classic sidebar queues one unit per click. A factory's queue limit caps the count, so a Shift-click that finds fewer places left queues only those.

Shift-right-click on a cameo now abandons every unit of that type, queued and in production, in one step, as Yuri's Revenge's abandon-all does. Before, it abandoned one. On a unit that is building, the first Shift-right-click still suspends it, as a plain right-click does; a second press abandons the lot.
