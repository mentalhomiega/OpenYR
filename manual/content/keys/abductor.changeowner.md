---
key: Abductor.ChangeOwner
summary: "With yes, an object an Abductor weapon takes aboard changes to the firer's house."
see_also: [Abductor, ImmuneToPsionics]
when_omitted:
  kind: value
  value: "no"
---

With `Abductor.ChangeOwner=yes`, an object that an [`Abductor=yes`](/keys/abductor/) weapon takes aboard becomes the firer's. It keeps its new owner when it leaves the hold. An object that is [`ImmuneToPsionics=yes`](/keys/immunetopsionics/) or holds the `PSIONICSIMMUNE` ability is taken aboard but keeps its owner.

```ini title="rulesmd.ini"
[MyAbductGun] ; example WeaponType
Abductor=yes
Abductor.ChangeOwner=yes
```
