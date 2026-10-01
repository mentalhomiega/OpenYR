---
key: LightningPrintText
summary: "Whether a lightning storm announces itself in on-screen text."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

Yuri's Revenge prints a warning while a lightning storm waits to break and a message when it breaks. The game reads this key, but prints no storm text yet.

```ini title="rulesmd.ini"
[General]
LightningPrintText=no
```

The storm itself is unaffected.
