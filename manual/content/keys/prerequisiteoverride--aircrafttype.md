---
key: PrerequisiteOverride
scope: aircrafttype
label: Build without prerequisites
see_also: [Prerequisite, BuildLimit, "system:production"]
when_omitted:
  kind: value
  value: ""
---

A house that owns at least one structure on this list may build the type without its [`Prerequisite=`](/keys/prerequisite/) entries. The tech level, stolen-technology, [`RequiredHouses=`](/keys/requiredhouses/) and [`ForbiddenHouses=`](/keys/forbiddenhouses/) checks, and the `Owner=` check for a structure, do not apply either. A type with `TechLevel=-1` therefore becomes buildable too. The [build limit](/keys/buildlimit/) still applies.

Write BuildingType IDs, separated by commas; case does not matter. An entry that matches no BuildingType is dropped. Each owned structure counts as it does for a `Prerequisite=` entry. A group name, such as `FACTORY`, stays in the list but never counts toward this override.

The standard rules set this key on `GHOST`, `CLEG` and `YURI`.

```ini title="rulesmd.ini"
[MYTANK] ; example InfantryType
Prerequisite=MYRADAR
PrerequisiteOverride=MYLANDMARK
; MYLANDMARK: an example BuildingType; owning one lets MYTANK be built without MYRADAR
```

Computer houses use this key when they produce. Their base planning reads only the `Prerequisite=` list.
