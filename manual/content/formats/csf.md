---
format_id: csf
title: CSF string tables
summary: Stores the game's text as UTF-16 strings looked up by label.
kind: binary
source_files:
  - code/csf.h
  - code/csf.cpp
  - code/init.cpp
extensions:
  - .CSF
role: text
---

The game reads its text from the string table `RA2MD.CSF`. Rules, the interface and scenarios name a string by its label, such as `Name:APOC` or `GUI:LoadingEx`, and the game shows the string the table holds for it. The table is read once at startup, right after the language archives are mounted, so it is normally a member of `LANGMD.MIX`. A loose `RA2MD.CSF` or one in an archive searched earlier replaces it; [MIX archives](/formats/mix/#mounting-and-search-order) gives the order.

If the table is missing or is not a string table, the game stops during startup.

## Looking up a label

- Labels are matched without regard to case.
- If a label appears more than once, the first one in the file is used.
- A label with several strings shows its first string.
- A label the table does not hold shows as `MISSING:'<label>'`, and the label is written to the debug log.

A string can also carry a short ASCII value, which names a speech file played with the string.

## How strings are cleaned up

When the table is read, spaces are removed from every string in these places:

- at the start of the string, and after a line break or a tab;
- where two or more spaces follow each other, all but the first;
- before a line break or a tab, and at the end of the string.

A string written only of spaces therefore reads as empty.

## File layout

Every number is a 32-bit little-endian integer. A tag is four bytes.

| Part | Contents |
| --- | --- |
| Header | The tag ` FSC`, the format version, the number of labels, the number of strings, an unused integer, and the language. The language is read only from version 2 onward; an older table counts as language 0. |
| Labels | One record per label: the tag ` LBL`, the number of strings the label has, the length of the label name in bytes, and the name in ASCII. The label's strings follow it directly. |
| Strings | The tag ` RTS`, or `WRTS` for a string with an extra value, then the length in UTF-16 characters and the characters with every bit inverted. A `WRTS` string ends with the length of its extra value in bytes and the value in ASCII. |

A table that declares no labels or no strings is refused. A record that is cut short or carries an unknown tag ends the table: the labels before it are kept, and the rest are missing.
