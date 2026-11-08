---
key: MaximumCheerRate
scope: global-rules
label: Cheer command delay
summary: The fewest frames between two uses of the cheer command by one player.
when_omitted:
  kind: value
  value: "1"
---

```ini title="rulesmd.ini"
[General]
MaximumCheerRate=300
```

After a player gives the cheer command, the command does nothing until more than `MaximumCheerRate` frames have passed. The wait starts again at each new match. The stock value of 300 is about 20 seconds at normal game speed.

The limit applies to the command each player gives. It does not stop infantry cheering for other reasons, such as freed slaves or a win.
