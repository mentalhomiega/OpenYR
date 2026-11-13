---
key: DistributedWeaponFire
scope: weapontype
label: 'Read and saved, but nothing acts on it'
when_omitted:
  kind: value
  value: "no"
---

`DistributedWeaponFire` is a Yuri's Revenge key. We read it as a yes or no value and keep it with the weapon, and it has no effect: in Yuri's Revenge nothing in firing, targeting or damage looks at it either. The only other place the original game uses it is the weapon checksum that multiplayer sync checks compare. Mods that set it get the same shots as mods that leave it out.

```ini title="rulesmd.ini"
[MyWeapon] ; example WeaponType
DistributedWeaponFire=yes
```
