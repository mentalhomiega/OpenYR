---
key: SuperWeaponsAllowed
scope: global-rules
label: Superweapons
when_omitted:
  kind: value
  value: "yes"
  note: The stock rules leave the key out, so the built-in default applies.
---

`SuperWeaponsAllowed` sets the starting state of the Superweapons option on the skirmish setup screen. The screen shows this value until a match starts from it. With the option off, a building whose superweapon can be disabled from the shell cannot be built and does not grant its superweapon, unless the rules list it in [`BuildTech`](/keys/buildtech/). [Superweapons](/systems/superweapons/) covers the rest.
