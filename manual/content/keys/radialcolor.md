---
key: RadialColor
summary: Read by Yuri's Revenge but unused; a structure's radius ring takes its owner's color.
see_also: [HasRadialIndicator, "system:cloaking"]
---

Three comma-separated numbers from `0` to `255`, for red, green and blue.

:::note[A partial triplet reads as the default]
`RadialColor=255` names one channel where three are needed, so the ring keeps its default color and the debug log records the line. [INI syntax](/formats/ini-syntax/#malformed-values) has the rule.
:::
