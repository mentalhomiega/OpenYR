---
title: Choose the EVA lines and messages for promotion, loss and capture
category: feature
release: 0.2.0
targets:
- type: key
  id: EVA.VeteranPromoted
  effect: added
- type: key
  id: EVA.ElitePromoted
  effect: added
- type: key
  id: EVA.Lost
  effect: added
- type: key
  id: LostEvaEvent
  effect: added
- type: key
  id: Message.Capture
  effect: added
- type: key
  id: Message.Lost
  effect: added
credit:
- MentalHomiega
---

An object type can name the EVA line announced when its object is promoted to veteran or elite (`EVA.VeteranPromoted`, `EVA.ElitePromoted`) or lost (`EVA.Lost`). A structure that needs an engineer can show text to the player who captures it (`Message.Capture`) or loses it (`Message.Lost`), and speak a line to the loser (`LostEvaEvent`). The keys follow the Ares documentation. A type that sets none of them behaves as before.
