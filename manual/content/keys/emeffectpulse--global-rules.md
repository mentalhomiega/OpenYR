---
key: EMEffectPulse
scope: global-rules
label: Stock pulse for EMEffect warheads
see_also: [EMEffect, EMP.Duration]
when_omitted:
  kind: computed
  note: no when the rules declare any armor type in an Ares `[ArmorTypes]` section, otherwise yes.
---

Whether a warhead with `EMEffect=yes` and no `EMP.Duration=` makes the stock electromagnetic pulse where it hits.

- With `yes`, the hit makes a pulse over the warhead's `CellSpread` that stuns vehicles and powers down buildings for as many frames as the weapon's damage, as Yuri's Revenge does.
- With `no`, `EMEffect=yes` has no effect and the warhead only does its damage. A warhead with `EMP.Duration=` uses the Ares EMP counter either way.

```ini title="rulesmd.ini"
[General]
EMEffectPulse=no
```

This is our own key. Ares does not read `EMEffect`, and Ares-based mods such as Mental Omega leave it on warheads that should not stun, such as flamethrowers, so rules that declare Ares armor types get no pulse unless the key says otherwise. Stock rules declare none and keep the pulse.
