---
key: MessageTextColor
summary: The color scheme of the text a map's text action prints for a player of the side.
see_also: ["system:sidebar"]
when_omitted:
  kind: value
  value: none
  note: The text is drawn in the first color scheme.
---

Name a scheme from `[Colors]`, such as `DarkRed`. When the local player's side sets the key, the line that a map's [Text trigger](/mapping/actions/11/) action prints is drawn in that scheme. A name that matches no scheme leaves the text in the first scheme. Chat lines and other messages keep their own colors.

```ini title="rules.ini"
[Nod]
MessageTextColor=DarkRed
```
