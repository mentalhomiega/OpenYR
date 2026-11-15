---
key: SW.VirtualCharge
scope: superweapontype
summary: "Keeps a weapon's charge while its house does not hold it."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a weapon whose granting structures are gone keeps counting down, and stays charged once the countdown ends. The cameo leaves the sidebar and the weapon cannot be fired. When the house holds the weapon again, it returns with the charge it had instead of starting its countdown over.

The key has no effect on a one-time weapon or on a weapon with [`UseChargeDrain=yes`](/keys/usechargedrain/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.VirtualCharge=yes
```

See [Keeping the charge](/systems/superweapons/#keeping-the-charge).
