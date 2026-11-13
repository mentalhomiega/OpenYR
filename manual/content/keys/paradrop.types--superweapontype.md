---
key: ParaDrop.Types
scope: superweapontype
summary: "Lists the infantry a paradrop weapon carries."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's paradrop lists"
---

Used with [`ParaDrop.Num=`](/keys/paradrop.num/), which must have one count for each type. Each plane carries every listed type. Names that are not infantry types are skipped. A country or side can set its own list with `ParaDrop.<name>.Types=`, `ParaDrop.<name>.Num=` and `ParaDrop.<name>.Aircraft=`, where `<name>` is the country or the side. A later plane of a weapon with [`ParaDrop.Count=`](/keys/paradrop.count/) above 1 can have its own: `ParaDrop.<name>.Plane2.Types=`, `ParaDrop.Plane2.Types=` and so on, from plane 2 up. Plane 1 has no `Plane` part. A plane takes the first place that sets its list, in this order: the country's plane keys, the side's plane keys, the plain plane keys, then the same three for plane 1, and last the side's lists. The types and counts come from one place, so a place that sets one of them has to set both. The aircraft is looked up the same way on its own. A weapon with none of these keys uses the side's lists.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ParaDrop.Types=E1,E2
```

See [Paradrops](/systems/superweapons/#paradrops).
