---
key: UseCameoSortOrder
scope: global-rules
label: Sort cameos by CameoSortOrder
summary: Orders the sidebar strips by CameoSortOrder and declaration order instead of Yuri's Revenge's order.
see_also: [CameoSortOrder, SortCameoAsBaseDefense, SidebarSorting, "system:sidebar"]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rulesmd.ini"
[General]
UseCameoSortOrder=yes
```

With `yes`, each strip is ordered by [`CameoSortOrder`](/keys/cameosortorder/), then by group, then by declaration order, as [the order of the strips](/systems/sidebar/#the-order-of-the-strips) describes. With `no`, the strips keep Yuri's Revenge's order and those keys have no effect.

It has no effect with [`SidebarSorting=no`](/keys/sidebarsorting/).
