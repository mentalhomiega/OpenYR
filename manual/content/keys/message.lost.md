---
key: Message.Lost
summary: The text shown to the player who loses a structure of this type that needs an engineer to a capture.
see_also: [NeedsEngineer, "Message.Capture", LostEvaEvent, "system:capture"]
when_omitted:
  kind: value
  value: "none"
---

`Message.Lost` is the label of a string table entry that appears in the message list for ten seconds when an enemy takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type from the local player. The text is shown in the loser's color. `none`, or no key, shows nothing.

The player also hears `EVA_TechBuildingLost`, or the line [`LostEvaEvent`](/keys/lostevaevent/) names, whether or not the type sets this key.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
NeedsEngineer=yes
Message.Lost=TXT_TechOil_Lost
```
