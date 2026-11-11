---
title: Redraw and pulse a structure an airstrike designator marks
category: fix
release: 0.2.0
targets:
- type: key
  id: Airstrike
  effect: changed
credit:
- MentalHomiega
---

A structure that an airstrike designator marks is now tinted whole as soon as the planes are called, and loses the tint at once when the mark lifts. Before, the tint appeared only on the parts of the structure that something else redrew, such as a strip the view scrolled into sight, and those parts stayed tinted after the mark lifted. The marked structure also pulses between its normal and double brightness, as in Yuri's Revenge. Saves from earlier builds do not load.
