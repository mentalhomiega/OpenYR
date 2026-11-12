"""Declared settings reach the catalog like typed accessor reads."""

from pathlib import Path
import sys
import tempfile
import unittest


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

import extract_engine


SAMPLE = r'''
constexpr NameEntry Mode_Names[] = {
	{ "None", MODE_NONE },
	{ "Map", MODE_MAP },
};

constexpr Setting<OptionsClass> Settings[] = {
	{
		.Section = "Video", .Key = "Switch",
		.Flag = &OptionsClass::Switch, .Default = true,
	},
	{
		// A comment naming .Section = "Bogus", .Key = "Ignored" stays out.
		.Section = "Video", .Key = "Level",
		.Number = &OptionsClass::Level, .Default = 3, .Min = 1, .Max = 16,
	},
	{
		.Section = "Video", .Key = "Mode",
		.Number = &OptionsClass::Mode, .Default = MODE_NONE, .Names = Mode_Names,
	},
};
'''


class DeclaredSettingTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.path = Path(directory.name) / "optionsettings.cpp"
        self.path.write_text(SAMPLE, encoding="latin-1")

    def test_table_entries_become_records(self):
        records, defaults = extract_engine.extract_declared_settings(
            str(self.path), "OptionsClass", {"group": "client settings"})
        by_key = {record["key"]: record for record in records}
        self.assertEqual(set(by_key), {"Switch", "Level", "Mode"})
        self.assertEqual(by_key["Switch"]["value_type"], "boolean")
        self.assertEqual(by_key["Switch"]["member"], "Switch")
        self.assertEqual(by_key["Level"]["value_type"], "integer")
        self.assertEqual(by_key["Mode"]["value_type"], "string")
        self.assertIsNone(by_key["Mode"]["member"])
        self.assertEqual(by_key["Mode"]["default_expr"], '"None"')
        self.assertEqual(defaults, {"Switch": "true", "Level": "3"})

    def test_entry_needs_one_member_and_a_default(self):
        self.path.write_text('.Section = "Video", .Key = "Bad",\n', encoding="latin-1")
        with self.assertRaises(ValueError):
            extract_engine.extract_declared_settings(
                str(self.path), "OptionsClass", {})


if __name__ == "__main__":
    unittest.main()
