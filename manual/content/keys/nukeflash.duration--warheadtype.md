---
key: NukeFlash.Duration
scope: warheadtype
label: Nuclear flash length
summary: "Frames the map-wide nuclear flash lasts when a projectile with this warhead explodes; 0 for no flash."
see_also: [NukeAmbientChangeRate, PreImpactAnim, "system:superweapons"]
when_omitted:
  kind: computed
  note: "30 for the warhead named NUKE, 0 for any other warhead"
---

`NukeFlash.Duration` is an Ares key. When a projectile with this warhead explodes, the whole map's light rises to the nuclear flash level and stays there for this many game frames, then returns to the scenario's lighting over about 15 more frames, at the pace [`NukeAmbientChangeRate`](/keys/nukeambientchangerate/) sets. `0` turns the flash off.

The warhead named `NUKE` flashes when its `NUKEBALL` or [`PreImpactAnim`](/keys/preimpactanim/) animation starts, as in Yuri's Revenge, so `NukeFlash.Duration=0` on `[NUKE]` removes the flash of the stock nuclear missile. Any other warhead flashes at the moment its projectile explodes, with the lighting of the rules, not of a superweapon's `Light.*` keys. A new flash replaces one that is still on screen.

```ini title="rulesmd.ini"
[MyWH] ; example WarheadType
NukeFlash.Duration=45
```
