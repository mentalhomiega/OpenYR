---
key: PrismSupportDelay
summary: "Frames a prism tower rests after beaming support."
see_also: ["system:prism-towers"]
when_omitted:
  kind: value
  value: "100"
---

After a tower [beams support](/systems/prism-towers/#recruiting-support) to another, it cannot fire or support again for this many frames.

```ini title="rulesmd.ini"
[General]
PrismSupportDelay=45
```
