---
key: BountyDisplay
summary: Whether bounty payments are shown over the destroyed object, for destroyers that do not set Bounty.Display.
see_also: ["Bounty.Display", Bounty, "system:bounty"]
when_omitted:
  kind: value
  value: "no"
---

`BountyDisplay=yes` shows the [bounty](/systems/bounty/#display) of every destroyer whose type does not set [`Bounty.Display`](/keys/bounty.display/). A type that sets `Bounty.Display` follows its own value.

```ini title="rulesmd.ini"
[AudioVisual]
BountyDisplay=yes
```
