---
key: PrioritySelectionFiltering
scope: client-settings
label: 'Drag selection skips low priority objects'
summary: "Whether a drag selection leaves out LowSelectionPriority objects when it caught others."
see_also: [LowSelectionPriority]
when_omitted:
  kind: value
  value: "yes"
---

With `PrioritySelectionFiltering=no`, a selection box selects every selectable object it catches, flagged or not. The setting has no checkbox; write it in the `[Options]` section of the game's settings file.

```ini title="RA2MD.INI"
[Options]
PrioritySelectionFiltering=no
```
