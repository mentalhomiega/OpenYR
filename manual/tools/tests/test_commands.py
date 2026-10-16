from copy import deepcopy
from pathlib import Path
from tempfile import TemporaryDirectory
import sys
import unittest


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

import commands
import commands_engine
import schema_validation


class RegisteredCommandTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.catalog = commands_engine.build_catalog()
        cls.registered = cls.catalog["registered_commands"]

    def test_current_registration_set_reaches_both_builds(self):
        self.assertTrue(self.registered)
        self.assertTrue(all(
            record["availability"]["builds"] == ["release", "debug"]
            for record in self.registered))
        self.assertTrue(all(
            record["title"].strip() and record["description"].strip()
            and record["category"].strip()
            for record in self.registered))

    def test_numbered_team_families_expand_through_ten(self):
        ids = {record["id"] for record in self.registered}
        for prefix in ("TeamCreate", "TeamSelect", "TeamAddSelect", "TeamAddTo", "TeamCenter"):
            self.assertEqual(
                {identifier for identifier in ids if identifier.startswith(prefix + "_")},
                {f"{prefix}_{number}" for number in range(1, 11)},
            )

    def test_only_source_forced_bindings_are_published(self):
        forced = {
            record["id"]: record["forced_binding"]
            for record in self.registered
            if "forced_binding" in record
        }
        self.assertEqual(forced, {"DeleteWaypoint": "Delete", "Options": "Escape"})
        self.assertTrue(all("default_binding" not in record for record in self.registered))

    def test_ids_routes_and_resource_metadata_are_exact(self):
        # build_catalog refuses to return a catalog with a duplicate ID or route,
        # so uniqueness is established before setUpClass finishes.
        by_id = {record["id"]: record for record in self.registered}
        # The title token pads to two digits and the description token does not,
        # so the family is checked across the width change rather than at one member.
        for number in range(1, 11):
            record = by_id[f"TeamCreate_{number}"]
            self.assertEqual(record["title"], f"Create Team {number:2d}")
            self.assertEqual(
                record["description"],
                f"Creates Team {number} from currently selected units.")


class StringConstructorRegistrationTests(unittest.TestCase):
    INIT = (
        "class HeldModifierCommandClass : public CommandClass\n"
        "{\n"
        "\tpublic:\n"
        "\t\tHeldModifierCommandClass(char const * name, char const * display, char const * description) :\n"
        "\t\t\tName(name),\n"
        "\t\t\tDisplay(display),\n"
        "\t\t\tDescription(description)\n"
        "\t\t{\n"
        "\t\t}\n"
        "\t\tvirtual char const * Get_Unique_Name(void) const {\n"
        "\t\t\treturn(Name);\n"
        "\t\t}\n"
        "\t\tvirtual char const * Get_Display_Name(void) const {\n"
        "\t\t\treturn(Display);\n"
        "\t\t}\n"
        "\t\tvirtual char const * Get_Category(void) const {\n"
        "\t\t\treturn(Fetch_String(TXT_INTERFACE));\n"
        "\t\t}\n"
        "\t\tvirtual char const * Get_Description(void) const {\n"
        "\t\t\treturn(Description);\n"
        "\t\t}\n"
        "\tprivate:\n"
        "\t\tchar const * Name;\n"
        "\t\tchar const * Display;\n"
        "\t\tchar const * Description;\n"
        "};\n"
        "\n"
        "void Init_Commands(void)\n"
        "{\n"
        "\tAllCommands.Add(new HeldModifierCommandClass(\"ForceFire\", \"Force fire\", "
        "\"Hold to fire (on a unit or the ground), then release.\"));\n"
        # The forced bindings must be registered too, or registered_commands refuses the text.
        "\tAllCommands.Add(new HeldModifierCommandClass(\"DeleteWaypoint\", \"Delete waypoint\", "
        "\"Deletes the selected waypoint.\"));\n"
        "\tAllCommands.Add(new HeldModifierCommandClass(\"Options\", \"Options\", "
        "\"Opens the options menu.\"));\n"
        "}\n"
    )
    LANGUAGE = 'TXT_INTERFACE "Interface"\n'

    def registration_text(self, arguments):
        return self.INIT.replace(
            '"ForceFire", "Force fire", "Hold to fire (on a unit or the ground), then release."',
            arguments)

    def test_string_arguments_fill_the_members_the_methods_return(self):
        records = {
            record["id"]: record
            for record in commands_engine.registered_commands(self.INIT, self.LANGUAGE)
        }
        record = records["ForceFire"]
        self.assertEqual(record["title"], "Force fire")
        self.assertEqual(
            record["description"], "Hold to fire (on a unit or the ground), then release.")
        self.assertEqual(record["category"], "Interface")
        self.assertEqual(record["route_id"], "forcefire")

    def test_wrong_string_count_fails_against_the_constructor(self):
        text = self.registration_text('"ForceFire", "Force fire"')
        with self.assertRaisesRegex(ValueError, "constructor takes 3 strings, registration gives 2"):
            commands_engine.registered_commands(text, self.LANGUAGE)

    def test_non_literal_string_argument_is_rejected(self):
        text = self.registration_text('"ForceFire", name, "Hold to fire."')
        with self.assertRaisesRegex(ValueError, "unsupported constructor argument 'name'"):
            commands_engine.registered_commands(text, self.LANGUAGE)

    def test_unstored_parameter_is_not_returned_as_metadata(self):
        text = self.INIT.replace(
            "\t\t\treturn(Display);", "\t\t\treturn(Name2);")
        with self.assertRaisesRegex(ValueError, "Get_Display_Name"):
            commands_engine.registered_commands(text, self.LANGUAGE)

    def test_shipped_modifier_registrations_publish_their_source_strings(self):
        records = {record["id"]: record for record in commands_engine.registered_commands()}
        expected = {
            "ForceFire": "Force fire",
            "ForceMove": "Force move",
            "SelectModifier": "Add to selection",
            "QueueMove": "Queue move",
            "GridBuildModifier": "Grid build modifier",
        }
        for identifier, title in expected.items():
            self.assertEqual(records[identifier]["title"], title)
        self.assertEqual(
            records["GridBuildModifier"]["description"],
            "Hold, then press Q W E R, A S D F or Z X C V to start the sidebar cameos in that order.",
        )


class CommandAdapterTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.manifest = commands_engine.load_manifest()
        cls.catalog = commands_engine.build_catalog(cls.manifest)

    def test_current_direct_controls_and_launch_branches_are_fully_classified(self):
        # Adjudication is fail-closed: a discovered site with no classification, a
        # claim with no site, and a doubly claimed site each raise. Returning at all
        # is the classification, so the counts belong to the manifest rather than here.
        fixed, launch = commands_engine.adapted_commands(self.manifest)
        self.assertTrue(fixed)
        self.assertTrue(launch)
        self.assertTrue(all(record["id"].startswith("fixed:") for record in fixed))
        self.assertTrue(all(record["id"].startswith("launch:") for record in launch))

    def test_current_inventory_is_tree_wide(self):
        sites = commands_engine.discover_fixed_sites()
        owners = {(site.file, site.function) for site in sites}
        self.assertTrue(sites)
        self.assertTrue({
            ("code/conquer.cpp", "Map_Edit_Loop"),
            ("code/dropship.cpp", "Dropship_Screen"),
            ("code/mpscore.cpp", "MultiScore::User_Input"),
        }.issubset(owners))

    def test_new_file_is_discovered_and_removed_site_makes_manifest_stale(self):
        with TemporaryDirectory() as directory:
            code = Path(directory) / "code"
            source = code / "future" / "input_handler.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "void Future_Handler(int input)\n"
                "{\n"
                "    // case KN_F8: ignored commented control\n"
                "    if (\n"
                "        input & KN_BUTTON\n"
                "    ) {}\n"
                "    switch (input) {\n"
                "        case KN_F9: break;\n"
                "    }\n"
                "}\n",
                encoding="latin-1",
            )
            sites = commands_engine.discover_fixed_sites(code)
            self.assertEqual(
                [(site.file, site.function, site.expression) for site in sites],
                [
                    ("code/future/input_handler.cpp", "Future_Handler", "KN_BUTTON"),
                    ("code/future/input_handler.cpp", "Future_Handler", "KN_F9"),
                ],
            )
            with self.assertRaisesRegex(ValueError, "has no fixed-control classification"):
                commands_engine._adjudicate(sites, [], [], "fixed-control")

            exclusion = {
                "site": {
                    "file": "code/future/input_handler.cpp",
                    "function": "Future_Handler",
                    "expression": "KN_F9",
                },
                "reason": "Synthetic test classification.",
            }
            source.write_text("void Future_Handler(int input) {}\n", encoding="latin-1")
            self.assertEqual(commands_engine.discover_fixed_sites(code), [])
            with self.assertRaisesRegex(ValueError, "stale classification"):
                commands_engine._adjudicate([], [], [exclusion], "fixed-control")


    def test_manifest_sites_are_semantic_identities_not_line_fingerprints(self):
        for group in ("fixed_controls", "launch_options"):
            for record in self.manifest[group]:
                for site in record["sites"]:
                    self.assertNotIn("line", site)
        for group in ("fixed_exclusions", "launch_exclusions"):
            for record in self.manifest[group]:
                for site in commands_engine._exclusion_sites(record):
                    self.assertNotIn("line", site)
                self.assertTrue(record["reason"].strip())

    def test_new_stale_and_overlapping_sites_fail_contextually(self):
        site = commands_engine.Site(
            "code/example.cpp", "Handle_Input", "KN_F9", None, 14, "case KN_F9:")
        with self.assertRaisesRegex(ValueError, "has no fixed-control classification"):
            commands_engine._adjudicate([site], [], [], "fixed-control")

        adapter = {
            "id": "fixed:example",
            "sites": [{
                "file": site.file,
                "function": site.function,
                "expression": site.expression,
            }],
        }
        with self.assertRaisesRegex(ValueError, "stale classification"):
            commands_engine._adjudicate([], [adapter], [], "fixed-control")
        exclusion = {"site": adapter["sites"][0], "reason": "Synthetic exclusion."}
        with self.assertRaisesRegex(ValueError, "overlapping classifications"):
            commands_engine._adjudicate([site], [adapter], [exclusion], "fixed-control")

    def test_debug_only_launch_availability_stays_distinct(self):
        launch = {record["id"]: record for record in self.catalog["launch_options"]}
        self.assertEqual(launch["launch:seed"]["availability"]["builds"], ["debug"])
        self.assertEqual(
            launch["launch:tournament-time"]["availability"]["builds"],
            ["release", "debug"],
        )


class CommandContractTests(unittest.TestCase):
    def test_generated_contract_rejects_unknown_fields_and_bad_routes(self):
        catalog = commands_engine.build_catalog()
        invalid = deepcopy(catalog)
        invalid["registered_commands"][0]["route_id"] = "Bad Route"
        invalid["registered_commands"][0]["default_binding"] = "F"
        errors = schema_validation.errors_for(
            invalid, "generated-commands.schema.json", "commands")
        self.assertTrue(any("route_id" in error for error in errors))
        self.assertTrue(any("default_binding" in error for error in errors))

    def test_command_generator_serializes_before_atomic_replace(self):
        source = Path(commands.__file__).read_text(encoding="utf-8")
        serialization = source.index("safe_dump")
        replacement = source.index("atomic_write_text", serialization)
        self.assertLess(serialization, replacement)


if __name__ == "__main__":
    unittest.main()
