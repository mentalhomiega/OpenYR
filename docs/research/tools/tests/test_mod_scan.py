import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import mod_scan  # noqa: E402


def make_data(folder):
    folder.mkdir()
    (folder / "engine.txt").write_text("# c\nStrength\nCost\n", encoding="utf-8")
    (folder / "ares.txt").write_text("Image.Foo\nPrefix.<x>.Suffix\n", encoding="utf-8")
    (folder / "phobos.txt").write_text("Phobos.Tag\n", encoding="utf-8")


class ModScanTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        make_data(self.root / "data")
        self.origins = mod_scan.load_origins(self.root / "data")

    def tearDown(self):
        self.tmp.cleanup()

    def test_classify(self):
        c = lambda k: mod_scan.classify(k, self.origins)
        self.assertEqual(c("strength"), "engine")
        self.assertEqual(c("Image.Foo"), "ares")
        self.assertEqual(c("Prefix.Mid.Suffix"), "ares")
        self.assertEqual(c("Phobos.Tag"), "phobos")
        self.assertEqual(c("Phobos.Tag2"), "phobos")
        self.assertEqual(c("Nothing"), "unknown")

    def test_engine_wins_over_extensions(self):
        (self.root / "data" / "phobos.txt").write_text("Cost\n", encoding="utf-8")
        origins = mod_scan.load_origins(self.root / "data")
        self.assertEqual(mod_scan.classify("Cost", origins), "engine")

    def test_scan_with_include(self):
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "RulesMD.ini").write_text(
            "[#include]\n1=extra.ini\n[Tank]\nStrength=300 ; hp\nPhobos.Tag=yes\n"
            "[InfantryTypes]\n0=E1\n", encoding="latin-1")
        (mod / "extra.ini").write_text("[Tank]\nMystery=1\nStrength=9\n", encoding="latin-1")
        entries, found = mod_scan.scan_folder(mod)
        self.assertEqual(found, ["rulesmd.ini"])
        keys, occ, ignored, _ = mod_scan.summarize(entries, self.origins)
        self.assertEqual(keys["engine"], 1)
        self.assertEqual(occ["engine"], 2)
        self.assertEqual(keys["phobos"], 1)
        self.assertEqual(keys["unknown"], 1)
        self.assertEqual(ignored, 1)
        known, total, pct = mod_scan.coverage(keys)
        self.assertEqual((known, total), (2, 3))
        self.assertAlmostEqual(pct, 66.666, places=2)

    def test_include_cycle_terminates(self):
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "rulesmd.ini").write_text("[#include]\n1=a.ini\n", encoding="latin-1")
        (mod / "a.ini").write_text("[#include]\n1=rulesmd.ini\n[S]\nCost=1\n", encoding="latin-1")
        entries, _ = mod_scan.scan_folder(mod)
        self.assertEqual(entries, [("S", "Cost")])

    def test_output_has_names_and_counts_only(self):
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "rulesmd.ini").write_text("[Tank]\nSecretKey=hunter2\nStrength=1\n", encoding="latin-1")
        out = io.StringIO()
        with redirect_stdout(out):
            mod_scan.main([str(mod), "--list-unknown"])
        text = out.getvalue()
        self.assertIn("unknown: secretkey", text)
        self.assertNotIn("hunter2", text)

    def test_shipped_phobos_patterns(self):
        origins = mod_scan.load_origins()
        for key in ("PrimaryFireFLH.Burst0", "VoiceWeapon3Attack", "Insignia.Weapon2.Elite",
                    "TiberiumEater.Cell4", "AircraftDockingDir1"):
            self.assertEqual(mod_scan.classify(key, origins), "phobos", key)

    def test_missing_files(self):
        self.assertEqual(mod_scan.main([str(self.root)]), 2)


if __name__ == "__main__":
    unittest.main()
