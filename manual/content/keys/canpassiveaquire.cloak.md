---
key: CanPassiveAquire.Cloak
summary: With no, a cloakable object of this type on the Guard mission never picks a target on its own.
see_also: [CanPassiveAquire, "CanPassiveAquire.Guard", "system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

`CanPassiveAquire.Cloak=no` makes an object that can cloak hold fire on the Guard mission, as [`CanPassiveAquire.Guard=no`](/keys/canpassiveaquire.guard/) does for every object of the type. An object counts as cloakable when its type has [`Cloakable=yes`](/keys/cloakable/) or when the Cloaking ability or a crate gives it a cloak; it need not be hidden at the moment. The Guard area mission still scans.

```ini title="rulesmd.ini"
[MYSPY] ; example InfantryType
Cloakable=yes
CanPassiveAquire.Cloak=no
```
