---
key: ActsLike
summary: The country a campaign house builds and plans as.
see_also: [Owner, HunterSeeker, MultiplayPassive, "system:production", "system:ai-base-building"]
when_omitted:
  kind: computed
  note: The house's own country, or no country at all for a [`MultiplayPassive=yes`](/keys/multiplaypassive/) country such as the stock `Neutral` and `Special`.
---

`ActsLike` makes a house build and plan as another country. Name the country by its section name, by its `Name=`, or by its position in the rules country list. `<none>` makes the house act for no country. A value that names no country leaves the default in place.

Only a campaign mission reads its house records, so this is a campaign setting. Outside a campaign, every house keeps the default.

```ini title="scenario map file"
[Special] ; a house record in the scenario's own house list
ActsLike=Nod ; ActsLike=1 names the same country in the stock rules
```

The country chosen here affects four things:

- **Computer base planning.** The house plans and builds from the first entry that this country may own in each of these lists: [power plants](/keys/buildpower/), [barracks](/keys/buildbarracks/), [factories](/keys/buildweapons/), [radars](/keys/buildradar/), [tech centers](/keys/buildtech/), [refineries](/keys/buildrefinery/), [walls](/keys/concretewalls/) and gates ([east-west](/keys/ewgates/) and [north-south](/keys/nsgates/)). A replacement [harvester](/keys/harvesterunit/) is the first entry this country may own, or the list's first entry when the country may own none. A [base plan](/systems/ai-base-building/) only includes structures whose [`Owner`](/keys/owner/) list names this country. The house also follows the base-building settings of this country's side. A house that acts for no country plans no base.
- **Unit crates.** When one of the house's objects collects a unit crate, a randomly chosen reward vehicle is drawn only from types this country may own.
- **Hunter-seekers.** A hunter-seeker launched by the house is the [`HunterSeeker`](/keys/hunterseeker/#scope-side) of this country's side.
- **Side-restricted AI triggers.** An AI trigger restricted to one side runs for the house only when this country belongs to that side.

It does not change which country a [`RequiredHouses`](/keys/requiredhouses/) or [`ForbiddenHouses`](/keys/forbiddenhouses/) list tests. Those lists test the house's own country: the one its record names with [`Country=`](/keys/country--house-per-scenario/), or, when the record names none, the country its own name gives.
