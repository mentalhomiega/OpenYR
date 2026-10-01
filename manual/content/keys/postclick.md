---
key: PostClick
summary: Makes a superweapon the second click of a two-click weapon, fired without a charge of its own.
see_also: [PreClick, PreDependent, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A `PostClick=yes` weapon is the second half of a [two-click weapon](/systems/superweapons/#two-click-weapons). It needs no structure, charge or cameo: it fires whenever its house holds the weapon its [`PreDependent=`](/keys/predependent/) names, fully charged, and it acts on the cells that weapon last picked. Firing it spends the other weapon's charge, not its own.

```ini title="rulesmd.ini"
[ChronoWarpSpecial]
Type=ChronoWarp
Action=ChronoWarp
PostClick=yes
PreDependent=ChronoSphere
```

A `PostClick=yes` weapon whose house holds no weapon of the named `Type=` cannot fire.
