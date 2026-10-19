---
key: SW.PostDependent
scope: superweapontype
summary: "Names the weapon whose second click completes a chronosphere shot."
see_also: [PreClick, PostClick, PreDependent, "system:superweapons"]
when_omitted:
  kind: computed
  note: "the first PostClick=yes weapon whose PreDependent= names ChronoSphere"
---

A `Type=ChronoSphere` weapon with `SW.PostDependent=` arms its follow-up click for the section the key names, so two chronospheres can each pair with their own warp. The named section still sets [`PostClick=yes`](/keys/postclick/) and [`PreDependent=ChronoSphere`](/keys/predependent/). A name that is not a declared weapon leaves the choice to the default.

When the follow-up fires, it acts on the cells picked by the chronosphere that names it. A chronosphere that names a different follow-up is passed over, and one that names none is used otherwise. The follow-up's own [`SW.Designators=`](/keys/sw.designators/), [`SW.Inhibitors=`](/keys/sw.inhibitors/) and the other aiming keys are tested on the second click.

```ini title="rulesmd.ini"
[ChronoliftSpecial] ; example SuperWeaponType
Type=ChronoSphere
PreClick=yes
SW.PostDependent=PostliftSpecial

[PostliftSpecial]
Type=ChronoWarp
PostClick=yes
PreDependent=ChronoSphere
```

See [Two-click weapons](/systems/superweapons/#two-click-weapons).
