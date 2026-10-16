from pathlib import Path
import sys
import tempfile
import textwrap
import unittest

TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

import openyr_keys


CATALOG = """\
Alpha:
  key: Alpha
  scopes:
  - applies_to: [global rules]
    file: rules.ini
    section: {kind: literal, name: AudioVisual}
    value_type: integer
    status: generated
Beta:
  key: Beta
  scopes:
  - applies_to: [BulletType]
    file: art.ini
    section: {kind: identifier, source: object-type}
    value_type: boolean
    status: generated
  - applies_to: [WarheadType]
    file: rules.ini
    section: {kind: identifier, source: object-type}
    value_type: boolean
    status: generated
Gamma:
  key: Gamma
  scopes:
  - applies_to: [client settings]
    file: ra2md.ini
    section: {kind: literal, name: Video}
    value_type: boolean
    status: generated
"""

FILES = {
    "manual/data/ini-keys.yaml": CATALOG,
    "manual/content/keys/alpha.md": """\
---
key: Alpha
summary: Sets how bright the thing is.
when_omitted:
  kind: value
  value: "3"
---
Body.
""",
    "manual/content/keys/beta.md": """\
---
key: Beta
---
`Beta=yes` turns the thing on. A second sentence follows.
""",
    "manual/content/keys/beta--bullettype.md": """\
---
key: Beta
scope: bullettype
when_omitted:
  kind: computed
  note: Follows the projectile.
---
""",
    "manual/content/keys/beta--warheadtype.md": """\
---
key: Beta
scope: warheadtype
when_omitted:
  kind: value
  value: "no"
---
""",
    "manual/content/keys/gamma.md": """\
---
key: Gamma
summary: Draws the thing.
when_omitted:
  kind: value
  value: "no"
---
""",
    "manual/changes/added-alpha.md": """\
---
title: Add Alpha
category: feature
release: 0.2.0
targets:
- {type: key, id: Alpha, effect: added}
credit: [MentalHomiega]
---
Text.
""",
    "manual/changes/added-beta.md": """\
---
title: Add Beta
category: feature
release: 0.2.0
targets:
- {type: key, id: Beta, scope: warheadtype, effect: added}
credit: [MentalHomiega]
---
Text.
""",
    "manual/changes/yr-gamma.md": """\
---
title: Port Gamma
category: feature
release: 0.2.0
targets:
- {type: key, id: Gamma, effect: added}
credit: [MentalHomiega]
---
Text.
""",
    "manual/content/formats/spawn-ini.md": "## What the file asks for\n",
    "manual/content/formats/opents-ini.md": "## The files it reads\n",
    "code/spawnerconfig.cpp": 'x = ini.Get_Int(SETTINGS, "Credits", 1);\n',
    "code/deploymentconfig.h": 'std::string RulesFile = "RULESMD.INI";\n',
    "code/deploymentconfig.cpp":
        'RulesFile = ini.Get_String("Files", "Rules", RulesFile.c_str());\n',
    "docs/LAUNCHER.md": "| `[Update] Check=` | `yes` | Turns update checks off. |\n",
}

DATA = """\
version: 1
records:
  added-alpha: OpenYR
  added-beta: Ares
gamemd_records: []
undecided_records: []
examples:
  rules: |
    [AudioVisual]
    Alpha=3
  spawn: |
    [Settings]
    Credits=5
  opents: |
    [Files]
    Rules=X.INI
spawn_ini: |
  Settings | Credits | 10000 | what-the-file-asks-for | The credits.
opents_ini:
  Files/Rules:
    effect: "The rules file."
    anchor: the-files-it-reads
"""


def make_tree(root, data=DATA, files=None):
    for name, text in {**FILES, **(files or {})}.items():
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
    data_file = root / "openyr_keys.yaml"
    data_file.write_text(data, encoding="utf-8")
    return data_file


class OpenYrKeysTests(unittest.TestCase):
    def build(self, **options):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            data_file = make_tree(root, **options)
            return openyr_keys.build(root, data_file)

    def test_lists_classified_keys_and_leaves_out_gamemd_ones(self):
        text, problems = self.build()
        self.assertEqual(list(problems), [])
        self.assertIn("[`Alpha`](../manual/content/keys/alpha.md) | `3` | Sets how bright the thing is. | OpenYR", text)
        self.assertNotIn("Gamma", text.split("## SPAWN.INI")[0].split("## Contents")[1])

    def test_a_scoped_target_lists_only_that_scope(self):
        text, _ = self.build()
        self.assertIn("beta--warheadtype.md", text)
        self.assertNotIn("beta--bullettype.md", text)
        self.assertIn("`Beta=yes` turns the thing on.", text)
        self.assertNotIn("A second sentence", text)

    def test_reports_a_record_with_no_decision(self):
        _, problems = self.build(data=DATA.replace("  added-beta: Ares\n", ""))
        self.assertIn("unclassified: added-beta", problems)

    def test_refuses_an_example_that_sets_an_unlisted_key(self):
        _, problems = self.build(data=DATA.replace("Alpha=3", "Zeta=3"))
        self.assertTrue(any("Zeta" in problem for problem in problems))

    def test_checks_spawn_keys_against_the_code(self):
        _, problems = self.build(data=DATA.replace("| Credits |", "| Missing |"))
        self.assertTrue(any("Missing" in problem for problem in problems))

    def test_checks_spawn_anchors_against_the_page(self):
        _, problems = self.build(data=DATA.replace("what-the-file-asks-for", "no-such-heading"))
        self.assertTrue(any("no-such-heading" in problem for problem in problems))

    def test_reads_opents_defaults_from_the_header(self):
        text, problems = self.build()
        self.assertEqual(list(problems), [])
        self.assertIn("`RULESMD.INI` | The rules file.", text)

    def test_reads_launcher_update_keys(self):
        text, _ = self.build()
        self.assertIn("`[Update] Check`", text)

    def test_build_is_deterministic(self):
        first, _ = self.build()
        second, _ = self.build()
        self.assertEqual(first, second)

    def test_heading_anchor_matches_github(self):
        self.assertEqual(openyr_keys.heading_anchor("OPENTS.INI and the launcher"), "opentsini-and-the-launcher")


if __name__ == "__main__":
    unittest.main()
