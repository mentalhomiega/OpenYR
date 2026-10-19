---
key: ParaDrop.Types
scope: superweapontype
summary: "Lists the infantry a paradrop weapon carries."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's paradrop lists"
---

Used with [`ParaDrop.Num=`](/keys/paradrop.num/), which must have one count for each type. Each plane carries every listed type. Names that are not infantry types are skipped. A country or side can set its own list with `ParaDrop.<name>.Types=`, `ParaDrop.<name>.Num=` and `ParaDrop.<name>.Aircraft=`, where `<name>` is the country or the side. The country's keys win, then the side's, then the plain keys. A weapon with none of these keys uses the side's lists.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ParaDrop.Types=E1,E2
```

See [Paradrops](/systems/superweapons/#paradrops).
