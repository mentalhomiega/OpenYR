import io
import struct
import sys
import tempfile
import unittest
import zlib
from contextlib import redirect_stdout
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import mod_scan  # noqa: E402


def make_data(folder):
    folder.mkdir()
    (folder / "engine.txt").write_text("# c\nStrength\nCost\n", encoding="utf-8")
    (folder / "ares.txt").write_text("Image.Foo\nPrefix.<x>.Suffix\n", encoding="utf-8")
    (folder / "phobos.txt").write_text("Phobos.Tag\n", encoding="utf-8")


def build_mix(files, flags=0, old_format=False):
    """Return the bytes of a MIX archive holding {name: text}."""
    body, entries = b"", []
    for name, text in files.items():
        data = text.encode("latin-1")
        entries.append((mod_scan.mix_file_id(name), len(body), len(data)))
        body += data
    index = b"".join(struct.pack("<Iii", *entry) for entry in sorted(entries))
    if old_format:
        return struct.pack("<HI", len(entries), len(body)) + index + body
    return struct.pack("<HHHI", 0, flags, len(entries), len(body)) + index + body


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

    def test_mix_file_id_padding(self):
        self.assertEqual(mod_scan.mix_file_id("a"), zlib.crc32(b"A\x01AA"))
        self.assertEqual(mod_scan.mix_file_id("ab"), zlib.crc32(b"AB\x02A"))
        self.assertEqual(mod_scan.mix_file_id("abc"), zlib.crc32(b"ABC\x03"))
        self.assertEqual(mod_scan.mix_file_id("abcd"), zlib.crc32(b"ABCD"))

    def test_reads_rules_from_mix_and_prefers_loose_file(self):
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "expandmo01.mix").write_bytes(build_mix({
            "rulesmo.ini": "[Tank]\nMixKey=1\n[#include]\n1=extra.ini\n",
            "extra.ini": "[Tank]\nIncludedKey=2\n",
            "rulesmd.ini": "[Tank]\nOldKey=3\n"}))
        (mod / "rulesmd.ini").write_text("[Tank]\nStrength=1\n", encoding="latin-1")
        entries, found = mod_scan.scan_folder(mod)
        self.assertEqual(found, ["rulesmd.ini", "rulesmo.ini"])
        self.assertEqual(sorted(key for _, key in entries),
                         ["IncludedKey", "MixKey", "Strength"])

    def test_old_format_mix_and_skipped_archives(self):
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "a.mix").write_bytes(build_mix({"artmd.ini": "[S]\nArtKey=1\n"}, old_format=True))
        (mod / "b.mix").write_bytes(build_mix({"aimd.ini": "[S]\nSecret=1\n"}, flags=0x0002))
        (mod / "c.mix").write_bytes(b"not a mix")
        entries, found = mod_scan.scan_folder(mod)
        self.assertEqual(found, ["artmd.ini"])
        self.assertEqual(entries, [("S", "ArtKey")])

    def test_stock_origin(self):
        stock = self.root / "stock.txt"
        stock.write_text("Strength\nStockOnly\n", encoding="utf-8")
        origins = mod_scan.load_origins(self.root / "data", stock=stock)
        self.assertEqual(mod_scan.classify("Strength", origins), "engine")
        self.assertEqual(mod_scan.classify("StockOnly", origins), "stock")
        self.assertEqual(mod_scan.classify("Nothing", origins), "unknown")
        mod = self.root / "mod"
        mod.mkdir()
        (mod / "rulesmd.ini").write_text("[T]\nStockOnly=5\nOther=6\n", encoding="latin-1")
        out = io.StringIO()
        with redirect_stdout(out):
            self.assertEqual(mod_scan.main([str(mod), "--stock", str(stock)]), 0)
        self.assertIn("stock", out.getvalue())
        self.assertIn("covered: 1 of 2", out.getvalue())

    def test_missing_files(self):
        self.assertEqual(mod_scan.main([str(self.root)]), 2)


if __name__ == "__main__":
    unittest.main()
