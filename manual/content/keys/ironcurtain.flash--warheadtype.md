---
key: IronCurtain.Flash
scope: warheadtype
label: Flash when this warhead hits an Iron Curtained object
summary: "Whether this warhead's hits on Iron Curtained or force-shielded objects make a flash over them."
see_also: [IronCurtainFlash, "system:superweapons"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] IronCurtainFlash, which defaults to yes."
---

`IronCurtain.Flash` is an Ares key. A hit that an Iron Curtain or a Force Shield turns away makes a flash of light over the object, white under the Iron Curtain and blue under the Force Shield. `IronCurtain.Flash=no` stops this warhead's hits from flashing, and `yes` makes them flash even when [`IronCurtainFlash`](/keys/ironcurtainflash/#scope-global-rules) is `no`. The protection and the damage are unchanged.

```ini title="rulesmd.ini"
[MyWarhead] ; example WarheadType
IronCurtain.Flash=no
```
