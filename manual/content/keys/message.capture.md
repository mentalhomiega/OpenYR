---
key: Message.Capture
summary: The text shown to the player who captures a structure of this type that needs an engineer.
see_also: [NeedsEngineer, "Message.Lost", CaptureEvaEvent, "system:capture"]
when_omitted:
  kind: value
  value: "none"
---

`Message.Capture` is the label of a string table entry that appears in the message list for ten seconds when an engineer of the local player takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type. The text is shown in the capturer's color. `none`, or no key, shows nothing.

A label the string table lacks shows as the label itself, prefixed with `MISSING:`.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
NeedsEngineer=yes
Message.Capture=TXT_TechOil_Captured
```
