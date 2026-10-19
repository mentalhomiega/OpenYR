---
key: MenuText.Status
summary: The text shown for a country in the lobby's country list.
see_also: ["system:skirmish-setup", "Multiplay"]
when_omitted:
  kind: value
  value: none
---

The value is a string table label, such as `STT:PlayerSideAmerican`. The skirmish and network lobbies show its text in a tip while the pointer rests on the country box of a player whose country is the one chosen. A label that no string resolves shows no tip. Ares shows the text in the status bar while the pointer is over the country in the open list; the lobby here has no status bar and no per-entry hover.

```ini title="rules.ini"
[UnitedStates]
MenuText.Status=STT:PlayerSideAmerican
```
