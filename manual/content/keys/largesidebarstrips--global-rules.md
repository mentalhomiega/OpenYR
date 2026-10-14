---
key: LargeSidebarStrips
scope: global-rules
label: Large sidebar strips
summary: Lets each sidebar strip hold 225 cameos instead of 75.
see_also: [RecheckPrerequisites, "system:sidebar"]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[General]
LargeSidebarStrips=yes
```

`LargeSidebarStrips=yes` lets each [sidebar strip](/systems/sidebar/#what-the-strips-list) hold 225 cameos. With `no`, a strip holds 75 and refuses a further cameo while it is full, so a type offered to a full strip gets no cameo.

The figure of 75 is the room Yuri's Revenge allots to each tab. Its test admits one cameo more than that room, so the engine writes one entry past the end of the array. OpenTS refuses that entry.

This key is an OpenTS extension, and Yuri's Revenge has no equivalent. A rules set that offers more than 75 buildable types on one tab sets it in its own `rules.ini`. A saved game restores the value it was saved with, whatever the current rules say.
