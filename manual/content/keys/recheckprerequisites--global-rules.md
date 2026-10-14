---
key: RecheckPrerequisites
scope: global-rules
label: Re-check what the sidebar offers
summary: Removes a type's sidebar cameo and cancels its production once the house no longer meets the type's prerequisites, tech level or ownership rules.
see_also: [Prerequisite, BuildLimit, "system:sidebar", "system:production"]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rules.ini"
[General]
RecheckPrerequisites=yes
```

By default, [the sidebar check](/systems/sidebar/#what-removes-a-cameo) also removes a cameo whose type fails the tech-level, prerequisite or ownership gate of [what a house may build](/systems/production/#what-a-house-may-build). Selling or losing a structure named in the type's [`Prerequisite=`](/keys/prerequisite/) therefore removes the cameo, and the cameo returns once the house can build the type again. With `RecheckPrerequisites=no`, the check ignores those gates, so the cameo stays and can still start production. Computer houses have no sidebar and are not affected.

While the key is `yes`, removing a cameo also cancels that type's production. The item under construction and every queued copy are canceled, the credits spent so far are refunded, and a finished structure waiting to be placed is taken off the cursor.

For a structure type, the factory search needs a construction yard that is switched on. With the default `yes`, switching off every construction yard therefore removes all structure cameos and cancels structure production. Only a construction yard whose type drains power or sets [`Powered=yes`](/keys/powered/) can be [switched off](/systems/power/#switching-a-structure-off). Low power does not switch a structure off.

Yuri's Revenge has no such key. Its sidebar always applies these gates, so `no` is not stock behavior.

Vinifera reads the same key with the same meaning.
