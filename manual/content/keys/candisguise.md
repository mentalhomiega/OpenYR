---
key: CanDisguise
summary: "Lets a disguise warhead fired by this object disguise it."
see_also: [MakesDisguise, "system:disguises"]
when_omitted:
  kind: value
  value: "no"
---

An object of this type that hits a soldier with a [`MakesDisguise=yes`](/keys/makesdisguise/) warhead [takes on that soldier's look](/systems/disguises/).

```ini title="rulesmd.ini"
[MYSPY] ; example InfantryType
CanDisguise=yes
```
