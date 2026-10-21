---
key: FirestormGroundAnim
summary: The animation a raised firestorm wall creates when what it catches is at or below 100 leptons above the ground.
see_also: ["system:laser-fences"]
when_omitted:
  kind: value
  value: none
---

`FirestormGroundAnim` plays when a raised firestorm wall catches something at or below 100 [leptons](/glossary/#lepton) above the ground. It appears at the wall section, not at the caught object. Anything higher gets [`FirestormAirAnim`](/keys/firestormairanim/) instead, which lists what the wall catches.

The animation plays its type's [`LoopCount`](/keys/loopcount/), once if the type sets none.

The [approach sweep](/systems/laser-fences/#what-a-raised-section-destroys) plays neither animation. It destroys objects moving into a raised section's cell, and they die short of the section.

:::note[A missing key shows nothing]
If the key is missing or empty, the wall catches things at or below 100 leptons without an animation.
:::
