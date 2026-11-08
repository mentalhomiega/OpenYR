---
title: Read VoiceSelectDeactivated for a shut-down object
category: feature
release: 0.2.0
targets:
- type: key
  id: VoiceSelectDeactivated
  effect: added
credit: [MentalHomiega]
---

An object that is shut down, such as a Robot Tank with no Robot Control Center, now speaks its `VoiceSelectDeactivated` list when it is selected, as in Yuri's Revenge. Before, it spoke `VoiceSelect` and the key was not read. A type with no `VoiceSelectDeactivated` list gives no select line while it is shut down.
