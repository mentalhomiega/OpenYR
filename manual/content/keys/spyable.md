---
key: Spyable
summary: "Lets spies be sent into this structure."
see_also: [Agent, Infiltrate, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

A spy, an [`Agent=yes`](/keys/agent/) soldier, gets the enter cursor over another house's `Spyable=yes` structure and can be sent inside. What the spy does there depends on the structure; [Infiltrating it](/systems/capture/#infiltrating-it) lists the effects.

```ini title="rulesmd.ini"
[MYPLANT] ; example BuildingType
Spyable=yes
```

Engineers and other soldiers that can walk into structures go by [`Capturable`](/keys/capturable/) instead.
