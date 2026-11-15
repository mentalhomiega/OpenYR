---
key: EngineerAlwaysCaptureTech
summary: "With yes, a player's engineer shows the enter cursor over a tech structure whatever its strength."
see_also: [EngineerCaptureLevel, MultiplayPassive, "system:capture"]
when_omitted:
  kind: value
  value: "yes"
---

`EngineerAlwaysCaptureTech` is an Ares key. A tech structure is one whose first owner was a country with [`MultiplayPassive=yes`](/keys/multiplaypassive/), such as an oil derrick placed for the neutral house; it stays a tech structure after it is captured.

With `yes`, a player's engineer over a non-allied tech structure shows the enter cursor even when the structure's strength is above [`EngineerCaptureLevel`](/keys/engineercapturelevel/). With `no`, a tech structure follows `EngineerCaptureLevel` like any other structure. Like that key, this one changes only the cursor: the click gives the same capture order either way.

```ini title="rulesmd.ini"
[General]
EngineerCaptureLevel=.5
EngineerAlwaysCaptureTech=no
```
