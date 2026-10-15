"""Rebuild docs/OPENYR_KEYS.md, the index of INI keys OpenYR reads and gamemd does not.

    python manual/tools/openyr_keys.py            write docs/OPENYR_KEYS.md
    python manual/tools/openyr_keys.py --check    exit 1 when the file is stale
    python manual/tools/openyr_keys.py --report   list the change records that need a decision

The rows come from the change records in manual/changes and the key catalog
data/ini-keys.yaml. A record that adds a key does not say whether gamemd reads
the key, so openyr_keys.yaml classifies each record. SPAWN.INI, OPENTS.INI and
the launcher's [Update] keys are not in the catalog; their rows come from
openyr_keys.yaml, the code that reads them, and docs/LAUNCHER.md.

The script needs PyYAML and nothing else.
"""

import argparse
import difflib
import fnmatch
import re
import sys
from pathlib import Path

import yaml

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parents[1]
MANUAL = ROOT / "manual"
OUTPUT = ROOT / "docs" / "OPENYR_KEYS.md"
DATA_FILE = TOOLS / "openyr_keys.yaml"

OBJECT_TYPES = {"AircraftType", "BuildingType", "InfantryType", "UnitType"}

# The catalog names each file by the short name the manual uses.
GROUPS = [
    ("rules", "Rules", ("rules.ini", "ai.ini or map file")),
    ("art", "Art", ("art.ini",)),
    ("client", "Client settings", ("ra2md.ini",)),
    ("ui", "UI.INI", ("ui.ini",)),
    ("other", "Other game files", None),
]
OTHER_FILES = {
    "sound01.ini": "Sound registry (`SOUNDMD.INI`)",
    "theme01.ini": "Music registry (`THEMEMD.INI`)",
    "battle.ini": "Campaign list (`BATTLEMD.INI`)",
    "theater control file": "Theater control files",
    "map file": "Map and scenario files",
    "map file (.yrm, .mpr)": "Loose multiplayer maps (`.yrm`, `.mpr`)",
    "scenario packet (.pkt)": "Map packets (`.pkt`)",
    "map seed file": "Map seed files",
}
OMISSION_LABELS = {
    "computed": "Computed; see the page",
    "inherited": "Inherited; see the page",
    "context-dependent": "Depends; see the page",
    "unchanged": "Unchanged; see the page",
}


def read_frontmatter(path):
    source = path.read_text(encoding="utf-8")
    match = re.match(r"^---\r?\n([\s\S]*?)\r?\n---\r?\n?([\s\S]*)", source)
    if not match:
        raise ValueError(f"{path}: missing YAML frontmatter")
    return yaml.safe_load(match.group(1)) or {}, match.group(2)


def slugify(text):
    return re.sub(r"^-|-+$", "", re.sub(r"[^a-z0-9]+", "-", str(text).lower()))


def scope_ids(entry):
    """Name each scope of a catalog key as manual/tools/versioning.py does."""
    result = []
    for scope in entry.get("scopes", []):
        base = slugify((scope.get("applies_to") or [scope.get("section", "global")])[0])
        candidate, suffix = base, 2
        while candidate in result:
            candidate = f"{base}-{suffix}"
            suffix += 1
        result.append(candidate)
    return result


def heading_anchor(heading):
    text = re.sub(r"[^\w\s-]", "", heading.strip().lower())
    return re.sub(r"\s", "-", text)


def plain(text):
    """Turn a manual sentence into one a table cell and a repository page can hold."""
    text = re.sub(r"\[([^\]]+)\]\([^)]*\)", r"\1", text)
    text = re.sub(r"\s+", " ", text).strip()
    return text.replace("|", "\\|")


def first_sentence(body, limit=180):
    paragraphs = re.split(r"\n\s*\n", re.sub(r"```[\s\S]*?```", "", body).strip())
    text = plain(paragraphs[0]) if paragraphs and paragraphs[0] else ""
    match = re.search(r"(?<=[.!?])\s+(?=[A-Z`])", text)
    if match:
        text = text[:match.start()]
    if len(text) > limit:
        text = text[:limit].rsplit(" ", 1)[0].rstrip(",;:") + " ..."
    return text


class Problems(list):
    def add(self, text):
        self.append(text)


def load_catalog(root):
    path = root / "manual" / "data" / "ini-keys.yaml"
    loader = getattr(yaml, "CSafeLoader", yaml.SafeLoader)
    with path.open(encoding="utf-8") as stream:
        return yaml.load(stream, Loader=loader)


def load_pages(root):
    pages = {}
    for path in sorted((root / "manual" / "content" / "keys").glob("*.md")):
        data, body = read_frontmatter(path)
        pages[path.stem] = (data, body)
    return pages


def load_changes(root):
    changes = {}
    for path in sorted((root / "manual" / "changes").glob("*.md")):
        data, _ = read_frontmatter(path)
        changes[path.stem] = data
    return changes


def key_route(name, entry):
    route = name.lower()
    if entry.get("case_collides_with"):
        scopes = entry["scopes"][0].get("applies_to") or ["global"]
        route += "-" + slugify(scopes[0])
    return route


def classify(slug, data):
    """Return the source shown for a record, "gamemd", "undecided" or None."""
    if slug.startswith("yr-") or slug in data.get("gamemd_records", []):
        return "gamemd"
    if slug in data.get("undecided_records", []):
        return "undecided"
    return data.get("records", {}).get(slug)


def describe_default(omitted):
    if not omitted:
        return "see page"
    if omitted.get("kind") == "value":
        value = plain(str(omitted.get("value", "")))
        return show_default(value) if value else "(none)"
    note = plain(omitted.get("note", "")).rstrip(".")
    if note and len(note) <= 90:
        return note
    return OMISSION_LABELS.get(omitted.get("kind"), "see page")


def show_default(text):
    """Quote a one-word default; leave a sentence or a parenthesized note as written."""
    return text if text.startswith("(") or " " in text else f"`{text}`"


def describe_where(scope):
    section = scope.get("section") or {}
    if section.get("kind") == "literal":
        return f"[{section['name']}]"
    applies = scope.get("applies_to") or []
    if set(applies) == OBJECT_TYPES:
        return "Object types"
    return ", ".join(applies) or "Other"


def group_for(scope):
    filename = scope.get("file", "")
    for group_id, _, files in GROUPS:
        if files and filename in files:
            return group_id
    return "other"


def collect_rows(root, data, problems):
    catalog = load_catalog(root)
    pages = load_pages(root)
    changes = load_changes(root)

    removed = set()
    for change in changes.values():
        for target in change.get("targets", []):
            if target.get("type") == "key" and target.get("effect") == "removed":
                removed.add((target["id"], target.get("scope")))

    rows = {}
    for slug, change in changes.items():
        targets = [t for t in change.get("targets", [])
                   if t.get("type") == "key" and t.get("effect") == "added"]
        if not targets:
            continue
        origin = classify(slug, data)
        if origin is None:
            problems.add(f"unclassified: {slug}")
            continue
        if origin in ("gamemd", "undecided"):
            continue
        for target in targets:
            name = target["id"]
            entry = catalog.get(name)
            if entry is None:
                if (name, target.get("scope")) not in removed and (name, None) not in removed:
                    problems.add(f"{slug}: key {name} is not in data/ini-keys.yaml; run manage.py update")
                continue
            ids = scope_ids(entry)
            wanted = [target["scope"]] if target.get("scope") else ids
            for scope_id in wanted:
                if scope_id not in ids:
                    problems.add(f"{slug}: key {name} has no scope {scope_id}")
                    continue
                scope = entry["scopes"][ids.index(scope_id)]
                route = key_route(name, entry)
                overlay = pages.get(f"{route}--{scope_id}")
                base = pages.get(route)
                page_name = f"{route}--{scope_id}" if overlay else route
                if not overlay and not base:
                    problems.add(f"{slug}: key {name} has no page")
                    continue
                meta = {}
                for source in (base, overlay):
                    if source:
                        meta.update({k: v for k, v in source[0].items() if v is not None})
                summary = meta.get("summary")
                if not summary:
                    body = (overlay or base)[1]
                    summary = first_sentence(body) or first_sentence((base or overlay)[1])
                if (name, scope_id) in rows:
                    continue
                rows[(name, scope_id)] = {
                    "key": name,
                    "group": group_for(scope),
                    "file": scope.get("file", ""),
                    "where": describe_where(scope),
                    "default": describe_default(meta.get("when_omitted")),
                    "effect": plain(summary),
                    "link": f"../manual/content/keys/{page_name}.md",
                    "source": origin,
                }
    return list(rows.values())


def spawn_rows(root, data, problems):
    page = root / "manual" / "content" / "formats" / "spawn-ini.md"
    anchors = {heading_anchor(h) for h in re.findall(r"^#+\s+(.+)$", page.read_text(encoding="utf-8"), re.M)}
    code = "".join(
        (root / "code" / name).read_text(encoding="utf-8", errors="replace")
        for name in ("spawnerconfig.cpp", "spawner.cpp", "mpmode.cpp", "mpmodeparse.cpp")
        if (root / "code" / name).exists())
    rows = []
    for line in data.get("spawn_ini", "").splitlines():
        if not line.strip():
            continue
        section, key, default, anchor, effect = (part.strip() for part in line.split("|", 4))
        row = {"section": section, "key": key, "default": default, "anchor": anchor, "effect": effect}
        # Numbered keys such as Multi1 or GlobalFlag0 are built from a prefix in code.
        probe = re.sub(r"\d+$", "", key)
        if f'"{key}"' not in code and f'"{probe}' not in code:
            problems.add(f"spawn_ini: {key} is not read by the launch file code")
        if row["anchor"] not in anchors:
            problems.add(f"spawn_ini: {key}: the launch file page has no heading #{row['anchor']}")
        rows.append(row)
    return rows


def opents_rows(root, data, problems):
    source = (root / "code" / "deploymentconfig.cpp").read_text(encoding="utf-8")
    header = (root / "code" / "deploymentconfig.h").read_text(encoding="utf-8")
    defaults = dict(re.findall(r'std::string (\w+) = "([^"]*)";', header))
    defaults.update({m[0]: m[1] for m in re.findall(r"bool (\w+) = (true|false);", header)})
    page = root / "manual" / "content" / "formats" / "opents-ini.md"
    anchors = {heading_anchor(h) for h in re.findall(r"^#+\s+(.+)$", page.read_text(encoding="utf-8"), re.M)}
    described = data.get("opents_ini", {})
    rows = []
    pattern = re.compile(r'(\w+) = ini\.Get_(?:String|Bool)\("(\w+)", "(\w+)"')
    for member, section, key in pattern.findall(source):
        label = f"{section}/{key}"
        entry = described.get(label)
        if entry is None:
            for name, candidate in described.items():
                if "*" in name and fnmatch.fnmatchcase(label, name) and name != "Files/*":
                    entry = candidate
                    break
        if entry is None:
            entry = described.get(f"{section}/*")
        if entry is None:
            problems.add(f"opents_ini: no description for {label}")
            continue
        if entry["anchor"] and entry["anchor"] not in anchors:
            problems.add(f"opents_ini: {label}: the OPENTS.INI page has no heading #{entry['anchor']}")
        raw = defaults.get(member, "")
        if raw in ("true", "false"):
            raw = "yes" if raw == "true" else "no"
        rows.append({
            "section": section, "key": key, "default": entry.get("default") or raw or "(none)",
            "effect": entry["effect"], "anchor": entry["anchor"],
        })
    if not rows:
        problems.add("opents_ini: found no keys in code/deploymentconfig.cpp")
    return rows


def launcher_rows(root, problems):
    path = root / "docs" / "LAUNCHER.md"
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        match = re.match(r"^\|\s*`\[(Update)\]\s+(\w+)=`\s*\|\s*(.+?)\s*\|\s*(.+?)\s*\|$", line)
        if match:
            rows.append({
                "section": match.group(1), "key": match.group(2),
                "default": match.group(3).replace("`", "").strip(),
                "effect": plain(match.group(4)),
            })
    if not rows:
        problems.add("launcher: found no [Update] keys in docs/LAUNCHER.md")
    return rows


def table(header, lines):
    out = ["| " + " | ".join(header) + " |", "| " + " | ".join("---" for _ in header) + " |"]
    out.extend("| " + " | ".join(line) + " |" for line in lines)
    return out


def example_block(name, body, keys, problems):
    for line in body.splitlines():
        match = re.match(r"^\s*([A-Za-z][\w.]*)\s*=", line)
        if match and match.group(1) not in keys:
            problems.add(f"example {name}: {match.group(1)} is not a key the group lists")
    return ["```ini", *body.rstrip().splitlines(), "```"]


OPENTS_TITLE = "OPENTS.INI and the launcher"

INTRO = """\
# Keys OpenYR reads and gamemd does not

OpenYR reads the INI keys listed here, and Yuri's Revenge (`gamemd.exe`) does
not. A mod or map that sets one of them behaves differently in OpenYR, and the
original game ignores it.

Each key links to its page in the manual, which owns the details. The default
and the effect come from that page. In the tables, `rules.ini`, `art.ini` and
`ra2md.ini` stand for `rulesmd.ini`, `artmd.ini` and `RA2MD.INI`. A key that
appears under two headings, such as `AttachEffect.Duration`, is read in both
places.

We build this page with `python manual/tools/openyr_keys.py` from the manual's
key catalog and change records. Do not edit it by hand.

## How we pick the keys

A key is listed when a change record in `manual/changes/` adds it and
`manual/tools/openyr_keys.yaml` says gamemd does not read it. That file holds a
decision for each record. A record whose name starts with `yr-` ports gamemd
behavior, so its keys stay out. A record whose keys we could not place is also
left out until someone who knows the gamemd rules decides it;
`python manual/tools/openyr_keys.py --report` lists those records.

Each decision rests on what the change record says, not on a check of gamemd.
If a listed key turns out to be one gamemd reads, move its record to
`gamemd_records` in `openyr_keys.yaml`.

The From column says where a key comes from:

| From | Meaning |
| --- | --- |
| OpenYR | We added the key. |
| OpenTS | OpenTS added the key before or alongside our fork. |
| Ares, Phobos | We read the key as the Ares or Phobos documentation describes it. |
"""

REFRESH = """\
## Keeping this page current

Run `python manual/tools/openyr_keys.py` after `python manual/tools/manage.py
update` and commit the result. `--check` exits 1 when the page is stale or a
change record that adds a key has no decision in `openyr_keys.yaml`.
"""


def build(root=ROOT, data_file=DATA_FILE):
    problems = Problems()
    with Path(data_file).open(encoding="utf-8") as stream:
        data = yaml.safe_load(stream)

    rows = collect_rows(root, data, problems)
    by_group = {}
    for row in rows:
        by_group.setdefault(row["group"], []).append(row)

    spawn = spawn_rows(root, data, problems)
    opents = opents_rows(root, data, problems)
    launcher = launcher_rows(root, problems)
    examples = data.get("examples", {})

    counts = {gid: len(by_group.get(gid, [])) for gid, _, _ in GROUPS}
    out = [INTRO.rstrip(), "", "## Contents", ""]
    for group_id, title, _ in GROUPS:
        if counts[group_id]:
            out.append(f"- [{title}](#{heading_anchor(title)}): {counts[group_id]} entries")
    out.append(f"- [SPAWN.INI](#{heading_anchor('SPAWN.INI')}): {len(spawn)} entries")
    out.append(
        f"- [OPENTS.INI and the launcher](#{heading_anchor(OPENTS_TITLE)}): "
        f"{len(opents) + len(launcher)} entries")
    out.append("")

    for group_id, title, _ in GROUPS:
        group = by_group.get(group_id, [])
        if not group:
            continue
        out.extend([f"## {title}", ""])
        keys = {row["key"] for row in group}
        if group_id in examples:
            out.extend(["Example:", ""])
            out.extend(example_block(group_id, examples[group_id], keys, problems))
            out.append("")
        sections = {}
        for row in group:
            heading = row["where"]
            if group_id == "other":
                heading = OTHER_FILES.get(row["file"], row["file"])
                if row["where"].startswith("["):
                    heading += f": {row['where']}"
            sections.setdefault(heading, []).append(row)
        for heading in sorted(sections, key=lambda text: (text.startswith("["), text.lower())):
            out.extend([f"### {heading}", ""])
            lines = [
                [f"[`{row['key']}`]({row['link']})", row["default"], row["effect"], row["source"]]
                for row in sorted(sections[heading], key=lambda r: (r["key"].lower(), r["key"]))
            ]
            out.extend(table(["Key", "Default", "Effect", "From"], lines))
            out.append("")

    out.extend(["## SPAWN.INI", ""])
    out.append(
        "Unpatched Yuri's Revenge reads no launch file. A client such as CnCNet writes "
        "`SPAWN.INI` beside the game and starts it with `-SPAWN`, and we read the "
        "keys below from it. The [launch file page](../manual/content/formats/spawn-ini.md) "
        "owns the details, including what makes a launch fail.")
    out.append("")
    out.extend(["Example:", ""])
    out.extend(example_block("spawn", examples.get("spawn", ""), {r["key"] for r in spawn}, problems))
    out.append("")
    lines = []
    for row in spawn:
        link = f"../manual/content/formats/spawn-ini.md#{row['anchor']}"
        lines.append([f"[`[{row['section']}] {row['key']}`]({link})", show_default(row["default"]), plain(row["effect"])])
    out.extend(table(["Key", "Default", "Effect"], lines))
    out.append("")

    out.extend([f"## {OPENTS_TITLE}", ""])
    out.append(
        "`OPENTS.INI` describes a deployment: where its files are and which mods and "
        "files it starts with. Yuri's Revenge has no such file. The "
        "[OPENTS.INI page](../manual/content/formats/opents-ini.md) owns the details, and "
        "[the launcher guide](LAUNCHER.md) owns the `[Update]` keys.")
    out.append("")
    out.extend(["Example:", ""])
    file_keys = {r["key"] for r in opents} | {r["key"] for r in launcher}
    out.extend(example_block("opents", examples.get("opents", ""), file_keys, problems))
    out.append("")
    lines = []
    for row in opents:
        link = "../manual/content/formats/opents-ini.md" + (f"#{row['anchor']}" if row["anchor"] else "")
        lines.append([f"[`[{row['section']}] {row['key']}`]({link})", show_default(row["default"]), row["effect"]])
    for row in launcher:
        link = f"LAUNCHER.md#{heading_anchor('Settings in OPENTS.INI')}"
        lines.append([f"[`[{row['section']}] {row['key']}`]({link})", row["default"], row["effect"]])
    out.extend(table(["Key", "Default", "Effect"], lines))
    out.append("")
    out.append(REFRESH.rstrip())
    out.append("")
    return "\n".join(out), problems


def report(root=ROOT, data_file=DATA_FILE):
    with Path(data_file).open(encoding="utf-8") as stream:
        data = yaml.safe_load(stream)
    changes = load_changes(root)
    unclassified = [
        slug for slug, change in changes.items()
        if any(t.get("type") == "key" and t.get("effect") == "added" for t in change.get("targets", []))
        and classify(slug, data) is None
    ]
    return unclassified, list(data.get("undecided_records", []))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--check", action="store_true", help="exit 1 when the page is stale")
    parser.add_argument("--report", action="store_true", help="list records that need a decision")
    arguments = parser.parse_args(argv)

    if arguments.report:
        unclassified, undecided = report()
        print("Unclassified records (add each to openyr_keys.yaml):")
        for slug in unclassified:
            print(f"  {slug}")
        print("Undecided records (left out of the index):")
        for slug in undecided:
            print(f"  {slug}")
        return 0

    text, problems = build()
    if problems:
        print("ACTION REQUIRED", file=sys.stderr)
        for problem in problems:
            print(f"  - {problem}", file=sys.stderr)
        return 1

    if arguments.check:
        current = OUTPUT.read_text(encoding="utf-8") if OUTPUT.exists() else ""
        if current != text:
            sys.stderr.writelines(difflib.unified_diff(
                current.splitlines(True), text.splitlines(True),
                "docs/OPENYR_KEYS.md", "rebuilt", n=1))
            print("ACTION REQUIRED\n  - Run: python manual/tools/openyr_keys.py", file=sys.stderr)
            return 1
        print("VERIFIED docs/OPENYR_KEYS.md is current")
        return 0

    OUTPUT.write_text(text, encoding="utf-8", newline="\n")
    print("WROTE docs/OPENYR_KEYS.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
