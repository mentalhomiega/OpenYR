---
key: UIName
summary: Names the string table entry that supplies the type's displayed name.
see_also: [Name]
when_omitted:
  kind: value
  value: "none; the name comes from Name="
---

`UIName` holds the label of an entry in the [string table](/formats/csf/), such as `Name:APOC`, and the game shows that entry's text as the type's name. The label is matched without regard to letter case. If the table has no such label, or the entry's text is empty, the name comes from [`Name=`](/keys/name/).

A value that starts with `NOSTR:` is not a label: the game shows the text after the prefix as the name, without looking in the string table. The prefix follows the Ares documentation and is matched without regard to letter case. `NOSTR:` with nothing after it gives the name from `Name=`. The game reads up to 63 characters of `UIName`, prefix included, and cuts a longer value short. Ares allows 31, so keep the value to 31 characters if the rules must also work with Ares.

The name appears in these places:

- the text under a sidebar cameo, when the player has cameo text turned on;
- the tooltip over a cameo, which shows the name and the cost, or only the cost when cameo text is on;
- the label of a superweapon's countdown on the tactical view;
- the name of an object on the map, such as in the tooltip over it, including the name a disguised spy shows.

A country is also found by its `UIName` text wherever the game looks a country up by name.

```ini title="rulesmd.ini"
[APOC]
UIName=Name:APOC ; a label in RA2MD.CSF
Name=Apocalypse Tank

[SONAR] ; example SuperWeaponType
UIName=NOSTR:Sonar Pulse ; shown as Sonar Pulse
```
