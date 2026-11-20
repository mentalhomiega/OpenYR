"""Tests scripts/audio_check.py on recordings and logs made up here, so they need no game files.

  python tests/audiocheck/test_audio_check.py

Each case places known sounds into a made-up recording, writes the log lines the game would write for them, and
checks what the tool reports: sounds found at their place and level, a sound cut short, silence where a sound was
logged, sound no line explains, announcer lines that overlap, repeat or start late, a sound played at the wrong rate,
clipping, a DC offset, the clip mode, and the matching without numpy.
"""

import argparse
import math
import os
import random
import shutil
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'scripts'))

import audio_check  # noqa: E402

RATE = 48000
SOURCE_RATE = 22050


def noise(seconds, seed, rate=SOURCE_RATE):
    """A band of noise shaped by a slow wobble, distinct for each seed."""
    generator = random.Random(seed)
    count = int(seconds * rate)
    out = []
    low = 0.0
    for i in range(count):
        low = low * 0.7 + generator.uniform(-1.0, 1.0) * 0.3
        envelope = 0.5 + 0.5 * math.sin(2.0 * math.pi * (1.5 + seed % 5) * i / rate)
        out.append(max(-0.95, min(0.95, low * 2.5 * envelope)))
    return out


class Recording:
    """A made-up stereo recording and the log lines that go with it."""

    def __init__(self, seconds):
        self.left = [0.0] * int(seconds * RATE)
        self.right = [0.0] * int(seconds * RATE)
        self.lines = ['f=0 ms=0 pos=0 capture start rate=48000 channels=2 tick=1/45']
        self.handle = 100

    def add(self, samples, rate, start, gain, pan=0.0, factor=1.0, length=None):
        stretched = audio_check.resample(samples, RATE / (rate * factor))
        if length is not None:
            stretched = stretched[:int(length * RATE)]
        left = gain * min(1.0 - pan, 1.0)
        right = gain * min(1.0 + pan, 1.0)
        first = int(start * RATE)
        for i, value in enumerate(stretched):
            if first + i < len(self.left):
                self.left[first + i] += float(value) * left
                self.right[first + i] += float(value) * right

    def log(self, start, text):
        pos = int(round(start * RATE))
        self.lines.append('f=%d ms=%d pos=%d %s' % (int(start * 45), int(start * 1000), pos, text))

    def event(self, start, end, name, files, gain, group='sfx', end_reason='finished', pan=0.0):
        self.handle += 1
        h = self.handle
        self.log(start, 'event started h=%d type=%s group=%s prio=50 level=1.000 gain=%.4f pan=%.2f pitch=1.000 voice=0 by=0 sounds=%s loop=0-1 cycles=1'
                 % (h, name, group, gain, pan, files))
        if end is not None:
            self.log(end, 'event %s h=%d type=%s group=%s prio=50 level=1.000 gain=%.4f pan=0.00 pitch=1.000 voice=-1 by=0 sounds="" loop=0-0 cycles=0'
                     % (end_reason, h, name, group, gain))
        return h

    def eva(self, request, start, end, line, stem, gain=1.0):
        self.log(request, 'eva request line=%s now=0' % line)
        self.log(request, 'eva queued line=%s prio=50 type=queue pending=1' % line)
        self.handle += 1
        h = self.handle
        self.log(start, 'event started h=%d type=<stream> group=speech prio=255 level=1.000 gain=%.4f pan=0.00 pitch=1.000 voice=0 by=0 sounds="" loop=0-0 cycles=0' % (h, gain))
        self.log(start, 'stream open h=%d file=%s.WAV group=speech level=1.000 loop=0 rate=22050 channels=1' % (h, stem))
        self.log(start, 'eva start line=%s file=%s.WAV h=%d volume=1.000' % (line, stem, h))
        self.log(end, 'event finished h=%d type=<stream> group=speech prio=255 level=1.000 gain=1.0000 pan=0.00 pitch=1.000 voice=-1 by=0 sounds="" loop=0-0 cycles=0' % h)
        self.log(end, 'eva end line=%s' % line)

    def write(self, folder, name):
        self.lines.sort(key=lambda line: int(line.split()[2][4:]))
        audio = audio_check.Audio(RATE, [self.left, self.right])
        if audio_check.np is not None:
            audio.channels = [audio_check.np.asarray(self.left), audio_check.np.asarray(self.right)]
        path = os.path.join(folder, name + '.wav')
        audio_check.write_wav(path, audio)
        with open(os.path.join(folder, name + '.log'), 'w') as handle:
            handle.write('\n'.join(self.lines) + '\n')
        return path


def options(**changes):
    values = dict(range=None, late_ms=100, eva_late_ms=1200, quiet_db=-45.0, floor_db=-50.0, repeat_seconds=5.0)
    values.update(changes)
    return argparse.Namespace(**values)


class AudioCheckTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.folder = tempfile.mkdtemp(prefix='audiocheck-')
        cls.sounds = os.path.join(cls.folder, 'case.sounds')
        os.makedirs(cls.sounds)
        cls.clips = {}
        for index, (name, seconds) in enumerate((('boom', 0.4), ('click', 0.25), ('ceva001', 0.8), ('ceva002', 0.6),
                                                  ('stray', 0.3), ('fast', 0.5), ('gone', 0.4))):
            samples = noise(seconds, index + 1)
            cls.clips[name] = samples
            audio = audio_check.Audio(SOURCE_RATE, [samples])
            audio_check.write_wav(os.path.join(cls.sounds, name + '.wav'), audio)

        recording = Recording(9.0)
        clips = cls.clips
        recording.add(clips['boom'], SOURCE_RATE, 0.5, 0.5)
        recording.event(0.5, 0.95, 'Boom', 'boom', 0.5)
        recording.add(clips['click'], SOURCE_RATE, 1.2, 0.3, pan=-1.0)
        recording.event(1.2, 1.5, 'Click', 'click', 0.3, pan=-1.0)
        # Logged as finished, but only its first third is in the recording.
        recording.add(clips['boom'], SOURCE_RATE, 1.8, 0.5, length=0.13)
        recording.event(1.8, 2.25, 'Boom', 'boom', 0.5)
        # Two announcer lines that overlap, and the first line again soon after; the second starts late.
        recording.add(clips['ceva001'], SOURCE_RATE, 2.6, 0.45)
        recording.eva(2.5, 2.6, 3.45, 'EVA_First', 'ceva001', gain=0.45)
        recording.add(clips['ceva002'], SOURCE_RATE, 3.0, 0.45)
        recording.eva(1.5, 3.0, 3.65, 'EVA_Second', 'ceva002', gain=0.45)
        recording.add(clips['ceva001'], SOURCE_RATE, 4.0, 0.45)
        recording.eva(3.9, 4.0, 4.85, 'EVA_First', 'ceva001', gain=0.45)
        # Sound that no log line explains.
        recording.add(clips['stray'], SOURCE_RATE, 5.2, 0.5)
        # Played twice as fast as it should be.
        recording.add(clips['fast'], SOURCE_RATE, 6.0, 0.5, factor=2.0)
        recording.event(6.0, 6.6, 'Fast', 'fast', 0.5)
        # Logged but never mixed.
        recording.event(7.0, 7.5, 'Gone', 'gone', 0.5)
        # Clipping in the last half second on the left, and a DC offset throughout on the right.
        for i in range(int(8.5 * RATE), int(9.0 * RATE)):
            recording.left[i] = 0.97 if (i // 40) % 2 else -0.97
        for i in range(len(recording.right)):
            recording.right[i] += 0.01
        cls.capture = recording.write(cls.folder, 'case')
        cls.report, _, _ = audio_check.run_check(cls.capture, os.path.join(cls.folder, 'case.log'), [cls.sounds], options())

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.folder, ignore_errors=True)

    def sound(self, name, start):
        for record in self.report['sounds']:
            if record['name'] == name and abs(record['start'] - start) < 0.01:
                return record
        self.fail('%s at %.2f not reported' % (name, start))

    def flags(self, kind):
        return [flag for flag in self.report['flags'] if flag['flag'] == kind]

    def test_found_at_place_and_level(self):
        boom = self.sound('Boom', 0.5)
        self.assertEqual(boom['status'], 'found')
        self.assertLessEqual(abs(boom['offset_ms']), 2)
        self.assertLessEqual(abs(boom['level_error_db']), 1.0)
        self.assertEqual(boom['played'], 1.0)

    def test_panned_level(self):
        click = self.sound('Click', 1.2)
        self.assertIn(click['status'], ('found', 'found-weak'))
        self.assertLessEqual(abs(click['level_error_db']), 1.0)

    def test_cut_short(self):
        cut = [flag for flag in self.flags('cut-short') if flag['name'] == 'Boom']
        self.assertEqual(len(cut), 1)
        self.assertAlmostEqual(cut[0]['start'], 1.8, places=2)
        self.assertLess(cut[0]['played'], 0.75)

    def test_missing(self):
        missing = [flag for flag in self.flags('missing') if flag['name'] == 'Gone']
        self.assertEqual(len(missing), 1)
        self.assertEqual(missing[0]['status'], 'silent')

    def test_rate_mismatch(self):
        pitch = self.flags('pitch')
        self.assertEqual(len(pitch), 1)
        self.assertEqual(pitch[0]['name'], 'Fast')
        self.assertAlmostEqual(pitch[0]['factor'], 2.0, places=3)

    def test_unexplained(self):
        stray = self.flags('unexplained')
        self.assertTrue(any(5.1 <= flag['start'] <= 5.3 for flag in stray), stray)

    def test_eva(self):
        overlap = self.flags('eva-overlap')
        self.assertEqual([(flag['first'], flag['second']) for flag in overlap], [('EVA_First', 'EVA_Second')])
        repeat = self.flags('eva-repeat')
        self.assertEqual([flag['line'] for flag in repeat], ['EVA_First'])
        late = self.flags('eva-late')
        self.assertEqual([flag['line'] for flag in late], ['EVA_Second'])
        self.assertEqual(late[0]['delay_ms'], 1500)
        self.assertEqual(self.sound('EVA_First', 2.6)['status'], 'found')

    def test_clipping_and_dc(self):
        clipping = self.flags('clipping')
        self.assertEqual([flag['channel'] for flag in clipping], ['L'])
        dc = self.flags('dc-offset')
        self.assertEqual([flag['channel'] for flag in dc], ['R'])

    def test_text_report(self):
        text = audio_check.format_run_report(self.report)
        self.assertIn('EVA_Second', text)
        self.assertIn('eva-overlap', text)

    def test_clip_mode(self):
        clip_options = argparse.Namespace(clip_score=0.6, eva_pattern=r'^[a-z]?eva\d', repeat_seconds=5.0)
        report, _ = audio_check.clip_check(self.capture, self.sounds, clip_options)
        found = [(item['name'], round(item['start'], 1)) for item in report['found']]
        self.assertIn(('boom', 0.5), found)
        self.assertIn(('ceva001', 2.6), found)
        self.assertIn(('ceva001', 4.0), found)
        self.assertIn(('stray', 5.2), found)
        self.assertNotIn('gone', [name for name, _ in found])
        self.assertTrue(any(flag['flag'] == 'eva-repeat' for flag in report['flags']))

    def test_log_parsing(self):
        entry = audio_check.parse_line('[04:05:06.123] AUDIOLOG f=12 ms=300 pos=14400 eva dropped line="EVA Name" reason=queue-full by=X')
        self.assertEqual((entry['f'], entry['ms'], entry['pos'], entry['kind'], entry['action']), (12, 300, 14400, 'eva', 'dropped'))
        self.assertEqual((entry['line'], entry['reason'], entry['by']), ('EVA Name', 'queue-full', 'X'))
        self.assertIsNone(audio_check.parse_line('Speech: request EVA_UnitReady'))


class FallbackTest(unittest.TestCase):

    def test_match_without_numpy(self):
        saved = audio_check.np
        audio_check.np = None
        try:
            template = noise(0.05, 9, rate=2000)
            signal = [0.0] * 120 + [0.5 * x for x in template] + [0.0] * 80
            offset, score = audio_check.best_match(signal, template)
            self.assertEqual(offset, 120)
            self.assertGreater(score, 0.99)
            self.assertEqual(len(audio_check.resample([0.0, 1.0, 2.0, 3.0], 2.0)), 8)
        finally:
            audio_check.np = saved


if __name__ == '__main__':
    unittest.main()
