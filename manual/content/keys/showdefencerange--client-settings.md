---
key: ShowDefenceRange
scope: client-settings
label: Show defence range when selected
see_also: [InterfaceStyle]
when_omitted:
  kind: computed
  note: '`yes` when [`InterfaceStyle`](/keys/interfacestyle/#scope-client-settings) is `Modern` (its default), `no` for the Classic interface.'
---

`ShowDefenceRange=yes` draws a ring at the range of a selected defence's primary weapon. The ring shows only for your own structures, and only while they have power. A structure whose type has its own radial indicator, such as the Gap Generator, keeps that indicator and draws no second ring.

The **Show defence range when selected** checkbox in the game controls dialog and on the classic options page sets the key.
