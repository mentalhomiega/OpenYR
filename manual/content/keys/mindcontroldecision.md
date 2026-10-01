---
key: MindControlDecision
summary: "What a unit this team's members take over does, overriding the computer's roll."
see_also: [AICaptureNormal, "system:mind-control"]
when_omitted:
  kind: value
  value: "0"
---

When a member of this team takes a unit over for a computer house, this value [replaces the computer's choice](/systems/mind-control/#what-a-computer-does-with-a-unit): `1` joins the team, `2` goes to a grinder, `3` goes to a bio reactor, `4` hunts and `5` does nothing. With `0`, the computer's roll decides.

```ini title="map.ini"
[MyTeam] ; example TeamType
MindControlDecision=2
```
