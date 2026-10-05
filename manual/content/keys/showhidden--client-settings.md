---
key: ShowHidden
scope: client-settings
label: 'Show hidden objects'
see_also: [Behind, CanHideThings]
when_omitted:
  kind: value
  value: "no"
---

`ShowHidden=yes` draws the [`Behind`](/keys/behind/#scope-global-rules) animation over objects hidden behind a [`CanHideThings=yes`](/keys/canhidethings/#scope-buildingtype) structure. With `no`, the animation is not drawn. The setting has no effect on the corner brackets drawn when no `Behind` animation is set; those always show.

The Hidden Units checkbox in the game controls dialog sets this key, and the game saves the choice when the dialog is accepted.

```ini title="RA2MD.INI"
[Options]
ShowHidden=yes
```
