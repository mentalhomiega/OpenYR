---
key: DecloakToFire
scope: weapontype
label: 'Owner uncloaks to fire'
see_also: ["system:cloaking"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, a cloaked object fires this weapon without dropping its cloak. Submarines use this to torpedo ships while they stay submerged. With `yes`, a cloaked object uncloaks before it fires the weapon.

```ini title="rulesmd.ini"
[MyTorpedo] ; example Weapon
DecloakToFire=no
```
