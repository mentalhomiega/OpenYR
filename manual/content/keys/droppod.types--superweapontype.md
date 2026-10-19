---
key: DropPod.Types
scope: superweapontype
summary: "Lists the infantry a drop pod weapon lands."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "E1 and E2, equally likely"
---

Each pod carries one infantry type picked at random from the list, and a type listed twice is twice as likely. Names that are not infantry types are ignored.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
DropPod.Types=E1,E2
```

See [Drop pods](/systems/superweapons/#drop-pods).
