---
key: LightningPrintText
summary: "Whether a lightning storm announces itself in on-screen text."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

While a lightning storm waits to break, we play the EVA warning and print a message each time the frames left before the break are a multiple of 225. When the storm breaks, we print a message and play [`StormSound`](/keys/stormsound/). A shot refused because a storm is already active prints its message whatever this key says.

```ini title="rulesmd.ini"
[General]
LightningPrintText=no
```

The storm itself is unaffected.
