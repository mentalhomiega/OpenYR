---
title: Stop Tiberium from hurting or healing anything
category: fix
release: 0.2.0
targets:
- type: system
  id: tiberium
  effect: changed
- type: key
  id: TiberiumProof
  effect: changed
- type: key
  id: TiberiumHeal
  effect: changed
credit:
- MentalHomiega
---

Infantry no longer take damage from standing in Tiberium or gems, and `TiberiumHeal=yes` types no longer heal there or seed Tiberium when destroyed, as in Yuri's Revenge. [`TiberiumProof`](/keys/tiberiumproof/) and [`TiberiumHeal`](/keys/tiberiumheal/) are still read but change nothing.
