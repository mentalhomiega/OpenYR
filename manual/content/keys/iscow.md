---
key: IsCow
summary: "Gives the soldier the cow's idle behaviour, wandering about often, even under human control."
see_also: [IsDesolator, Fraidycat]
when_omitted:
  kind: value
  value: "yes for the type named COW, otherwise no"
---

`IsCow` is an Ares key. A soldier with `IsCow=yes` idles like the stock cow. When it picks an idle action it rolls a second time, and a low roll turns the action into a wander. When it wanders it moves off even if a human player owns it, where other soldiers wander only if they are computer-controlled and have [`Fraidycat=yes`](/keys/fraidycat/).

The default is `yes` for a type whose name is `COW` and `no` for every other, so stock rules behave as before. Setting the key on another type gives it the same behaviour, and `IsCow=no` on `COW` removes it.

```ini title="rulesmd.ini"
[MOO]
IsCow=yes
```
