---
key: AIRestrictReplaceTime
scope: global-rules
label: Rebuild hold-back after an attack
summary: Frames after one of a computer house's structures takes damage, during which the house rebuilds only walls, base defenses and power plants.
see_also: [AIExtraRefineries, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "0"
---

A whole number of game frames, at 15 frames to the second. Each time one of the house's structures takes damage from an attacker, a window opens that lasts this many frames from that hit. While it is open, a computer house in a skirmish or multiplayer game does not rebuild a structure it lost from its plan, unless the structure is armed or a power plant. A wall is rebuilt only beside one of the house's structures. A plan node that has never held a structure is filled at once. [Rebuilding](/systems/ai-base-building/#rebuilding) lists the order.

```ini title="rulesmd.ini"
[General]
AIRestrictReplaceTime=400
```

A campaign game ignores the key. `0`, the value when omitted, closes the window at once, so the key has no effect.
