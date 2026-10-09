---
key: TreeFire
summary: The two flames a terrain object would show once it catches fire. gamemd never sets a terrain object alight, so this has no effect.
see_also: [Sparky, Wood, Immune, TreeFlammability, OnFire]
no_effect: true
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[AudioVisual]
TreeFire=MYTREEFIRE1,MYTREEFIRE2 ; AnimTypes registered in [Animations]
```

No terrain object catches fire in gamemd. A [`Sparky=yes`](/keys/sparky/) warhead does not light a tree, and no fire spreads from one terrain object to another. The engine never shows these flames, so the list can be left empty.
