---
key: KeepWarheadVerses
scope: global-rules
label: Keep warhead Verses on a later read
see_also: [Verses]
when_omitted:
  kind: computed
  note: yes when the rules declare any armor type in an Ares `[ArmorTypes]` section, otherwise no.
---

Whether a warhead section that is read again without its own `Verses=` keeps the values it already has. A map that redefines a warhead, for example to add one key, is such a later read.

- With `no`, a later read without `Verses=` sets the warhead to 100% against all eleven original armor types, as Yuri's Revenge does. Custom armor types that follow one of those armors follow it to 100% too.
- With `yes`, the warhead keeps the `Verses=` that the rules gave it, and only a section that sets `Verses=` again changes them.

```ini title="rulesmd.ini"
[General]
KeepWarheadVerses=yes
```

This is our own key. Ares-based mods such as Mental Omega redefine many warheads in their maps without repeating `Verses=`, and expect the rules values to stay, so rules that declare Ares armor types keep them unless the key says otherwise. Stock rules declare none and keep the Yuri's Revenge reset. A map's own `[General]` can set the key too, and it then applies to that map's warhead sections.
