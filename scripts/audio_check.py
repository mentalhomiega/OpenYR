"""Checks a recording of the game's mix against the audio test log and the sounds it names.

  python scripts/audio_check.py run <capture.wav> [--log FILE] [--sounds DIR] [--json FILE] [--report FILE]
                                    [--spectrogram FILE --range START:END] [options]
  python scripts/audio_check.py clip <recording> --library DIR [--json FILE] [--report FILE] [--spectrogram FILE]

"run" reads a recording the audiocapture step wrote, the .log beside it and the .sounds folder beside it (each can be
given instead). For every sound the log says started, it looks for that sound in the recording at the logged time and
reports whether it is there, how far from the logged time, how loud against the level the log predicts, and how much
of it played. It also flags clipping, silence where a sound should be, sound the log does not explain, announcer
lines that overlap, repeat or start late, a DC offset, and sounds that match only at another playback rate.

"clip" takes any recording, such as the sound track of a video (ffmpeg converts anything that is not a 16-bit WAV),
and a folder of decoded sounds (the audiodump step writes one), and lists which of those sounds occur and when.

The text report goes to standard output unless --report names a file; --json writes the same findings as JSON.
--spectrogram writes a greyscale PNG of a time range, with a mark at the top for each sound the log started.

Needs Python 3 and its standard library. numpy makes it much faster; without it the matching runs on a coarser copy
of the audio and says so. docs/AUDIO_TESTING.md describes the workflow and the limits.
"""

import argparse
import json
import math
import os
import re
import shlex
import shutil
import struct
import subprocess
import sys
import tempfile
import wave
import zlib

try:
    import numpy as np
except ImportError:  # The tool still runs, more slowly and more coarsely.
    np = None

MIX_RATE = 48000
# The mixer's level curve: a 0..1 level becomes the linear gain level ** (5/3).
PERCEPTUAL_EXPONENT = 5.0 / 3.0
# The mixer shapes samples above this magnitude to keep them below full scale.
SOFT_CLIP = 0.9

# The mixer's resampler, linear with a fourth-order low-pass filter, delays a sound by about this many frames.
MIXER_DELAY_FRAMES = 4
# Matching works on a mono copy at this rate (numpy) or the coarse rate (no numpy).
MATCH_RATE = 8000
COARSE_RATE = 2000
# How far before and after the logged start a sound is looked for, in seconds.
SEARCH_BEFORE = 0.05
SEARCH_AFTER = 1.0
# The longest stretch of a sound the matching uses, and the shortest it can match reliably.
TEMPLATE_SECONDS = 1.5
MIN_MATCH_SECONDS = 0.03
# A normalised correlation at least this high counts as the sound being there.
MATCH_SCORE = 0.5
# A quarter of a sound counts as played when it correlates at least this well where it should be.
PART_SCORE = 0.35
# A sound counts as in the mix when the level that fits it best is within this much of the level the log predicts.
LEVEL_SLACK_DB = 6.0
# A sound predicted this much quieter (-20 dB) than the mix at its place is reported masked, not missing.
MASKED = 0.1
# A stretch quieter than this (-60 dBFS) counts as silence.
SILENCE = 0.001
# A weaker correlation still counts within this many seconds of the logged start when the level matches the log.
WEAK_SCORE = 0.2
WEAK_SLACK = 0.01
WEAK_LEVEL_DB = 4.0
# Playback rates tried when a sound does not match at its own rate.
RATE_FACTORS = (0.5, 2.0, 44100.0 / 48000.0, 48000.0 / 44100.0, 22050.0 / 48000.0, 48000.0 / 22050.0)

LINE = re.compile(r'(?:^|AUDIOLOG )(f=-?\d+ ms=\d+.*)$')
LOG_STAMP = re.compile(r'^\[\d\d:\d\d:\d\d\.\d+\]\s?')


# ----------------------------------------------------------------------------------------------------------------
# Audio files


class Audio:
    """Samples as floats in -1..1, one list or array per channel."""

    def __init__(self, rate, channels):
        self.rate = rate
        self.channels = channels

    @property
    def frames(self):
        return len(self.channels[0]) if self.channels else 0

    @property
    def seconds(self):
        return self.frames / float(self.rate) if self.rate else 0.0

    def mono(self):
        if len(self.channels) == 1:
            return self.channels[0]
        if np is not None:
            return (self.channels[0] + self.channels[1]) * 0.5
        return [(a + b) * 0.5 for a, b in zip(self.channels[0], self.channels[1])]


def read_wav(path):
    """A 16-bit PCM WAV. Other formats go through ffmpeg first (see convert_with_ffmpeg)."""
    with wave.open(path, 'rb') as w:
        if w.getsampwidth() != 2:
            raise ValueError('%s: only 16-bit PCM is read directly' % path)
        count = w.getnchannels()
        rate = w.getframerate()
        data = w.readframes(w.getnframes())
    if np is not None:
        samples = np.frombuffer(data, dtype='<i2').astype(np.float64) / 32768.0
        channels = [samples[c::count].copy() for c in range(count)]
    else:
        values = struct.unpack('<%dh' % (len(data) // 2), data)
        channels = [[v / 32768.0 for v in values[c::count]] for c in range(count)]
    return Audio(rate, channels)


def write_wav(path, audio):
    frames = audio.frames
    with wave.open(path, 'wb') as w:
        w.setnchannels(len(audio.channels))
        w.setsampwidth(2)
        w.setframerate(audio.rate)
        out = bytearray()
        for i in range(frames):
            for channel in audio.channels:
                value = int(math.floor(float(channel[i]) * 32767.0 + 0.5))
                out += struct.pack('<h', max(-32768, min(32767, value)))
        w.writeframes(bytes(out))


def convert_with_ffmpeg(path, workdir):
    """Any audio or video file as a 48 kHz stereo 16-bit WAV, through ffmpeg on the PATH."""
    ffmpeg = shutil.which('ffmpeg')
    if ffmpeg is None:
        raise SystemExit('audio_check: %s is not a 16-bit WAV and ffmpeg is not on the PATH to convert it' % path)
    target = os.path.join(workdir, 'converted.wav')
    subprocess.run([ffmpeg, '-v', 'error', '-y', '-i', path, '-vn', '-ac', '2', '-ar', str(MIX_RATE), '-c:a', 'pcm_s16le', target],
                   check=True)
    return target


def load_any(path, workdir):
    try:
        return read_wav(path)
    except (wave.Error, ValueError, EOFError):
        return read_wav(convert_with_ffmpeg(path, workdir))


# ----------------------------------------------------------------------------------------------------------------
# Signal helpers. Each takes plain lists or numpy arrays and returns the same kind.


def resample(samples, factor):
    """Linear interpolation to factor times as many samples."""
    count = len(samples)
    length = max(1, int(round(count * factor)))
    if count == 0:
        return samples
    if np is not None:
        positions = np.arange(length) / factor
        return np.interp(positions, np.arange(count), samples)
    out = []
    for i in range(length):
        p = i / factor
        j = int(p)
        if j >= count - 1:
            out.append(samples[-1])
        else:
            t = p - j
            out.append(samples[j] * (1.0 - t) + samples[j + 1] * t)
    return out


def decimate(samples, rate, target):
    """Averages blocks down to roughly the target rate; returns the samples and the rate they are at."""
    step = max(1, int(round(rate / float(target))))
    if step == 1:
        return samples, rate
    if np is not None:
        # Everything above the new band is removed first, so sound above it cannot fold down into it; the mixer's
        # resampler and ours treat those high frequencies differently.
        values = np.asarray(samples, dtype=np.float64)
        spectrum = np.fft.rfft(values)
        spectrum[np.fft.rfftfreq(len(values), 1.0 / rate) > 0.45 * rate / step] = 0.0
        return np.fft.irfft(spectrum, len(values))[::step], rate / float(step)
    out = [sum(samples[i:i + step]) / step for i in range(0, len(samples) - step + 1, step)]
    return out, rate / float(step)


def rms(samples):
    if len(samples) == 0:
        return 0.0
    if np is not None:
        return float(np.sqrt(np.mean(np.square(samples))))
    return math.sqrt(sum(x * x for x in samples) / len(samples))


def ac_rms(samples):
    """The level without the DC offset, so an offset alone does not count as sound."""
    if len(samples) == 0:
        return 0.0
    if np is not None:
        values = np.asarray(samples)
        return float(np.sqrt(np.mean(np.square(values - np.mean(values)))))
    mean = sum(samples) / len(samples)
    return math.sqrt(sum((x - mean) ** 2 for x in samples) / len(samples))


def db(value):
    return 20.0 * math.log10(value) if value > 1e-9 else -180.0


def dot(a, b):
    if np is not None:
        return float(np.dot(a, b))
    return sum(x * y for x, y in zip(a, b))


def best_match(signal, template):
    """The offset into signal where template correlates best, and the normalised correlation there."""
    n = len(signal)
    m = len(template)
    if m == 0 or n < m:
        return 0, 0.0
    if np is not None:
        signal = np.asarray(signal, dtype=np.float64)
        template = np.asarray(template, dtype=np.float64)
        size = 1 << int(math.ceil(math.log2(n + m)))
        spectrum = np.fft.rfft(signal, size) * np.conj(np.fft.rfft(template, size))
        raw = np.fft.irfft(spectrum, size)[:n - m + 1]
        cumulative = np.concatenate(([0.0], np.cumsum(signal * signal)))
        energy = cumulative[m:] - cumulative[:n - m + 1]
        norm = math.sqrt(float(np.dot(template, template)))
        scores = raw / (np.sqrt(np.maximum(energy, 1e-12)) * max(norm, 1e-12))
        index = int(np.argmax(scores))
        return index, float(scores[index])
    norm = math.sqrt(sum(x * x for x in template)) or 1e-12
    best = (0, -2.0)
    energy = sum(x * x for x in signal[:m])
    for offset in range(n - m + 1):
        if offset > 0:
            energy += signal[offset + m - 1] ** 2 - signal[offset - 1] ** 2
        value = sum(signal[offset + k] * template[k] for k in range(m))
        score = value / (math.sqrt(max(energy, 1e-12)) * norm)
        if score > best[1]:
            best = (offset, score)
    return best


def all_matches(signal, template, threshold, spacing):
    """Every offset where template correlates at least threshold, at least spacing samples apart, best first."""
    n = len(signal)
    m = len(template)
    if m == 0 or n < m:
        return []
    if np is None:
        found = []
        for start in range(0, n - m + 1, max(spacing, 1)):
            offset, score = best_match(signal[start:start + spacing + m], template)
            if score >= threshold:
                found.append((start + offset, score))
        return found
    signal = np.asarray(signal, dtype=np.float64)
    template = np.asarray(template, dtype=np.float64)
    size = 1 << int(math.ceil(math.log2(n + m)))
    raw = np.fft.irfft(np.fft.rfft(signal, size) * np.conj(np.fft.rfft(template, size)), size)[:n - m + 1]
    cumulative = np.concatenate(([0.0], np.cumsum(signal * signal)))
    energy = cumulative[m:] - cumulative[:n - m + 1]
    norm = math.sqrt(float(np.dot(template, template)))
    scores = raw / (np.sqrt(np.maximum(energy, 1e-12)) * max(norm, 1e-12))
    # Quiet stretches give noisy scores, so an offset must also hold some of the template's energy.
    scores[energy < (norm * norm) * 0.01] = 0.0
    found = []
    order = np.argsort(scores)[::-1]
    taken = np.zeros(len(scores), dtype=bool)
    for index in order:
        score = float(scores[index])
        if score < threshold:
            break
        if taken[index]:
            continue
        found.append((int(index), score))
        taken[max(0, index - spacing):index + spacing] = True
    return sorted(found)


# ----------------------------------------------------------------------------------------------------------------
# The log


def parse_line(text):
    """One log line as a dict: f, ms, pos, kind, action and the key=value fields."""
    match = LINE.search(LOG_STAMP.sub('', text.rstrip('\r\n')))
    if match is None:
        return None
    try:
        words = shlex.split(match.group(1))
    except ValueError:
        return None
    entry = {'pos': -1, 'kind': '', 'action': ''}
    for word in words:
        key, equals, value = word.partition('=')
        if not entry['kind']:
            if equals and key in ('f', 'ms', 'pos'):
                entry[key] = int(value)
            elif not equals:
                entry['kind'] = word
        elif equals:
            entry[key] = value
        elif not entry['action']:
            entry['action'] = word
    return entry if entry['kind'] else None


def read_log(path):
    entries = []
    with open(path, encoding='utf-8', errors='replace') as handle:
        for text in handle:
            entry = parse_line(text)
            if entry is not None:
                entries.append(entry)
    return entries


def entry_seconds(entry, rate):
    """Where in the recording an entry falls: its position, else its clock reading."""
    if entry.get('pos', -1) >= 0:
        return entry['pos'] / float(rate)
    return entry.get('ms', 0) / 1000.0


class Occurrence:
    """A sound the log says the mixer started."""

    def __init__(self, handle, start, frame):
        self.handle = handle
        self.start = start
        self.frame = frame
        self.kind = 'sfx'
        self.name = ''
        self.files = []
        self.gain = 0.0
        self.pan = 0.0
        self.pitch = 1.0
        self.group = ''
        self.end = None
        self.end_reason = ''
        self.request = None
        self.live = False
        self.loop = None
        self.cycles = 1
        self.template = None
        self.reference = 0.0
        self.coefficient = 0.0
        self.changes = []
        self.findings = {}


def occurrences_from_log(entries, rate):
    """Every start the log records, with what it played, its predicted gain, and when and how it ended."""
    found = []
    live = {}
    streams = {}
    for entry in entries:
        kind = entry['kind']
        action = entry['action']
        seconds = entry_seconds(entry, rate)
        handle = entry.get('h')
        if kind == 'event' and action in ('started', 'live'):
            item = Occurrence(handle, seconds, entry.get('f', 0))
            item.live = action == 'live'
            item.name = entry.get('type', '')
            item.group = entry.get('group', '')
            item.gain = float(entry.get('gain', 0.0))
            item.pan = float(entry.get('pan', 0.0))
            item.pitch = float(entry.get('pitch', 1.0)) or 1.0
            sounds = entry.get('sounds', '')
            item.files = [s for s in sounds.split(',') if s]
            loop = re.match(r'(\d+)-(\d+)$', entry.get('loop', ''))
            if loop and item.files:
                item.loop = (int(loop.group(1)), int(loop.group(2)))
            item.cycles = int(entry.get('cycles', 1) or 1)
            item.kind = {'unitvoice': 'voice', 'speech': 'eva', 'music': 'music'}.get(item.group, 'sfx')
            found.append(item)
            live[handle] = item
        elif kind == 'stream' and action in ('open', 'live'):
            streams[handle] = entry
            item = live.get(handle)
            if item is not None and not item.files:
                item.files = [os.path.splitext(entry.get('file', ''))[0]]
                item.cycles = -1 if entry.get('loop') == '1' else 1
                if item.name == '<stream>':
                    item.name = entry.get('file', '')
        elif kind == 'eva' and action == 'start':
            item = live.get(entry.get('h'))
            if item is not None:
                item.kind = 'eva'
                item.name = entry.get('line', item.name)
        elif kind == 'music' and action == 'start':
            item = live.get(entry.get('h'))
            if item is not None:
                item.kind = 'music'
                item.name = entry.get('theme', item.name)
        elif kind == 'event' and action == 'retarget':
            item = live.get(handle)
            if item is not None:
                item.changes.append((seconds, float(entry.get('gain', 0.0)), float(entry.get('pan', 0.0)), float(entry.get('ramp', 0.0))))
        elif kind == 'event' and action in ('finished', 'stopped', 'cut-limit', 'cut-budget'):
            item = live.pop(handle, None)
            if item is not None:
                item.end = seconds
                item.end_reason = action
    return found


def eva_requests(entries, rate):
    """Each announcer request in order: (seconds, line name)."""
    return [(entry_seconds(e, rate), e.get('line', '')) for e in entries if e['kind'] == 'eva' and e['action'] == 'request']


# ----------------------------------------------------------------------------------------------------------------
# Sounds on disk


class Library:
    """Decoded sounds by lower-case file stem."""

    def __init__(self, folders):
        self.paths = {}
        self.cache = {}
        for folder in folders:
            if folder and os.path.isdir(folder):
                for name in os.listdir(folder):
                    stem, ext = os.path.splitext(name)
                    if ext.lower() == '.wav':
                        self.paths.setdefault(stem.lower(), os.path.join(folder, name))

    def get(self, stem):
        key = stem.lower()
        if key not in self.paths:
            return None
        if key not in self.cache:
            self.cache[key] = read_wav(self.paths[key])
        return self.cache[key]

    def names(self):
        return sorted(self.paths)


def expected_signal(item, library, rate, until):
    """The occurrence as the mixer adds it to the mono mix of the two channels, at the mix rate: its sounds back to
    back at its pitch, the body repeated while it loops, scaled by its gain and pan and by every change the log
    records. Returns the signal and the loudest scale it reaches, or None when a sound file is missing.
    """
    parts = []
    for stem in item.files:
        sound = library.get(stem)
        if sound is None:
            return None
        parts.append(resample(sound.mono(), rate / (sound.rate * item.pitch)))
    if not parts:
        return None
    # A loop repeats its body until it ends, or until the recording does; one that never ends in the log is drawn
    # up to the given time.
    first, last = item.loop if item.loop else (0, len(parts))
    pieces = list(parts[:last])
    end = item.end if item.end is not None else until
    if (item.cycles < 0 or item.cycles > 1) and last > first:
        body = parts[first:last]
        body_length = sum(len(p) for p in body)
        played = sum(len(p) for p in pieces)
        repeats = item.cycles - 1 if item.cycles > 1 else int(max(0.0, (end - item.start) * rate - played) // max(body_length, 1)) + 1
        for _ in range(max(0, repeats)):
            pieces.extend(body)
    pieces.extend(parts[last:])
    if np is not None:
        joined = np.concatenate([np.asarray(p, dtype=np.float64) for p in pieces])
    else:
        joined = [x for p in pieces for x in p]
    if item.cycles != 1:
        joined = joined[:max(1, int((end - item.start) * rate))]
    # The level the sound starts at, then each change the log records, ramped as the mixer ramps it.
    points = [(0.0, mix_scale(item.gain, item.pan), 0.0)]
    points += [(t - item.start, mix_scale(gain, pan), ramp) for t, gain, pan, ramp in item.changes]
    reference = max(value for _, value, _ in points)
    count = len(joined)
    envelope = np.full(count, points[0][1]) if np is not None else [points[0][1]] * count
    for (at, value, ramp) in points[1:]:
        start = max(0, int(at * rate))
        if start >= count:
            break
        steps = float(max(1, int(ramp * rate)))
        before = envelope[start]
        if np is not None:
            done = np.minimum(1.0, (np.arange(start, count) - start + 1) / steps)
            envelope[start:] = before + (value - before) * done
        else:
            for i in range(start, count):
                envelope[i] = before + (value - before) * min(1.0, (i - start + 1) / steps)
    joined = joined * envelope if np is not None else [x * e for x, e in zip(joined, envelope)]
    return joined, reference


def mix_scale(gain, pan):
    """How much of a sound the mono mix of the two channels holds at this gain and pan."""
    return gain * (min(1.0 - pan, 1.0) + min(1.0 + pan, 1.0)) * 0.5


# ----------------------------------------------------------------------------------------------------------------
# Checks


class Matcher:
    """The recording's mono mix at the matching rate, and what is left of it once every sound the log names is
    taken out at the level that fits it best."""

    def __init__(self, capture):
        self.capture = capture
        target = MATCH_RATE if np is not None else COARSE_RATE
        self.signal, self.rate = decimate(capture.mono(), capture.rate, target)
        self.residual = self.signal.copy() if np is not None else list(self.signal)
        self.coarse = np is None

    def prepare(self, signal, rate):
        mono, at = decimate(signal, rate, self.rate)
        if abs(at - self.rate) > 1e-6:
            mono = resample(mono, self.rate / at)
        return mono

    def place(self, item):
        """Where the occurrence's samples go in the signal: (first index, its template clipped to the signal)."""
        first = max(0, int(round(item.start * self.rate)))
        return first, item.template[:max(0, len(self.signal) - first)]

    def add(self, item, scale):
        first, template = self.place(item)
        if np is not None:
            self.residual[first:first + len(template)] += scale * np.asarray(template)
        else:
            for i, value in enumerate(template):
                self.residual[first + i] += scale * value

    def own(self, item, before=0.0, after=0.0):
        """The residual with this occurrence put back, over its own stretch widened by the seconds given; and the
        index that stretch starts at."""
        first, template = self.place(item)
        self.add(item, item.coefficient)
        a = max(0, first - int(before * self.rate))
        b = min(len(self.residual), first + len(template) + int(after * self.rate))
        piece = self.residual[a:b]
        piece = piece.copy() if np is not None else list(piece)
        self.add(item, -item.coefficient)
        return piece, a


def prepare_occurrence(item, library, matcher):
    """Builds the occurrence's template at the matching rate, or sets a status saying why it cannot be checked."""
    findings = item.findings
    capture = matcher.capture
    expected = expected_signal(item, library, capture.rate, capture.seconds)
    if expected is None:
        findings['status'] = 'no-file'
        return False
    joined, reference = expected
    item.reference = reference
    findings['duration'] = round(len(joined) / float(capture.rate), 3)
    findings['expected_db'] = round(db(reference), 1)
    # A sound the game stopped early is checked only on the part that played before the stop.
    if item.end is not None and item.end_reason != 'finished' and item.end - item.start < len(joined) / float(capture.rate):
        joined = joined[:int((item.end - item.start) * capture.rate)]
        if len(joined) < capture.rate * MIN_MATCH_SECONDS:
            findings['status'] = 'too-short'
            return False
    # Only what falls inside the recording can be compared.
    joined = joined[:max(1, int((capture.seconds - item.start) * capture.rate))]
    if np is not None:
        joined = np.concatenate((np.zeros(MIXER_DELAY_FRAMES), joined))
    else:
        joined = [0.0] * MIXER_DELAY_FRAMES + list(joined)
    item.template = matcher.prepare(joined, capture.rate)
    if reference <= 0.0 or rms(item.template) < 1e-7:
        findings['status'] = 'silent-sound'
        return False
    return True


def fit_levels(items, matcher):
    """Fits every template at its logged place at once, so sounds that overlap share the mix between them, and sets
    each occurrence's coefficient: 1 when it is in the mix at the level the log predicts, 0 when it is absent."""
    if np is None:
        # Without numpy, the loudest sounds are fitted and taken out first.
        for item in sorted(items, key=lambda i: -i.reference):
            first, template = matcher.place(item)
            piece = matcher.residual[first:first + len(template)]
            item.coefficient = dot(piece, template) / max(dot(template, template), 1e-12)
            matcher.add(item, -item.coefficient)
        return
    count = len(items)
    if count == 0:
        return
    places = [matcher.place(item) for item in items]
    gram = np.zeros((count, count))
    target = np.zeros(count)
    order = sorted(range(count), key=lambda i: places[i][0])
    for position, i in enumerate(order):
        first_i, template_i = places[i]
        end_i = first_i + len(template_i)
        target[i] = np.dot(matcher.signal[first_i:end_i], template_i)
        gram[i, i] = np.dot(template_i, template_i)
        for j in order[position + 1:]:
            first_j, template_j = places[j]
            if first_j >= end_i:
                break
            end = min(end_i, first_j + len(template_j))
            value = np.dot(template_i[first_j - first_i:end - first_i], template_j[:end - first_j])
            gram[i, j] = gram[j, i] = value
    ridge = 1e-6 * max(np.trace(gram) / count, 1e-12)
    coefficients = np.linalg.solve(gram + ridge * np.eye(count), target)
    for item, coefficient in zip(items, coefficients):
        item.coefficient = float(coefficient)
        matcher.add(item, -item.coefficient)


def leading_silence(signal):
    """The samples before a signal first reaches a hundredth of its peak."""
    if np is not None:
        values = np.abs(np.asarray(signal))
        if len(values) == 0:
            return 0
        loud = np.nonzero(values > values.max() * 0.01)[0]
        return int(loud[0]) if len(loud) else 0
    peak = max((abs(x) for x in signal), default=0.0)
    for index, value in enumerate(signal):
        if abs(value) > peak * 0.01:
            return index
    return 0


def judge_occurrence(item, matcher):
    """Says whether the occurrence is in the mix at its logged place and level, where it is when it is elsewhere,
    whether it plays at the wrong rate, and how much of it played."""
    findings = item.findings
    template = item.template
    # Silence at the start of a sound tells nothing, so the comparison starts where the sound does.
    lead = leading_silence(template)
    head = template[lead:lead + int(TEMPLATE_SECONDS * matcher.rate)]
    piece, origin = matcher.own(item, SEARCH_BEFORE, SEARCH_AFTER)
    first, _ = matcher.place(item)
    at_place = first - origin + lead
    local = piece[at_place:at_place + len(head)]
    score = dot(local, head) / math.sqrt(max(dot(local, local), 1e-12) * max(dot(head, head), 1e-12)) if len(local) == len(head) else 0.0
    findings['score'] = round(score, 3)
    findings['coefficient'] = round(item.coefficient, 3)
    # A long sound under louder ones correlates weakly yet far above chance, so the bar falls with its length.
    weak_bar = max(WEAK_SCORE / 2, 10.0 / math.sqrt(max(len(head), 1)))
    present = item.coefficient > 0.0 and abs(db(item.coefficient)) <= LEVEL_SLACK_DB and score >= min(WEAK_SCORE, weak_bar)
    offset = at_place
    if not present or score < MATCH_SCORE:
        # Elsewhere in the window: late or early.
        moved, moved_score = best_match(piece, head)
        if moved_score >= MATCH_SCORE and moved_score > score + 0.1 and abs(moved - at_place) > WEAK_SLACK * matcher.rate:
            segment = piece[moved:moved + len(head)]
            scale = dot(segment, head) / max(dot(head, head), 1e-12)
            if scale > 0.0:
                present, offset, score = True, moved, moved_score
                findings['score'] = round(score, 3)
                item.coefficient = scale
    if not present:
        # A sound that only matches at another rate was played at the wrong pitch.
        for factor in RATE_FACTORS:
            other = resample(head, 1.0 / factor)
            other_offset, other_score = best_match(piece, other)
            if other_score >= MATCH_SCORE and other_score > score + 0.15:
                findings['status'] = 'rate-mismatch'
                findings['rate_factor'] = round(factor, 4)
                findings['score'] = round(other_score, 3)
                return
        level = ac_rms(local)
        findings['window_db'] = round(db(level), 1)
        if level < SILENCE:
            findings['status'] = 'silent'
        elif rms(head) < MASKED * ac_rms(matcher.signal[first:first + len(head)]):
            # Far quieter than what else plays there, so its absence cannot be told from its presence.
            findings['status'] = 'masked'
        else:
            findings['status'] = 'not-found'
        return
    findings['status'] = 'found' if score >= MATCH_SCORE else 'found-weak'
    findings['offset_ms'] = int(round((offset - at_place) / matcher.rate * 1000))
    offset -= lead
    findings['level_error_db'] = round(db(abs(item.coefficient)), 1)
    findings['measured_db'] = round(db(abs(item.coefficient) * item.reference), 1)
    # How much of it played: each quarter of the template must still be where it belongs, at its level.
    quarters = 4
    played = 0
    for q in range(quarters):
        a = int(len(template) * q / quarters)
        b = int(len(template) * (q + 1) / quarters)
        part = template[a:b]
        segment = piece[offset + a:offset + b]
        if len(segment) < len(part):
            break
        expected = rms(part) * abs(item.coefficient)
        if expected < rms(template) * abs(item.coefficient) * 0.05 or expected < 0.3 * ac_rms(segment):
            # Too quiet against what else plays there to tell either way.
            played = q + 1
            continue
        value = dot(segment, part) / math.sqrt(max(dot(segment, segment), 1e-12) * max(dot(part, part), 1e-12))
        part_scale = dot(segment, part) / max(dot(part, part), 1e-12)
        level_holds = value >= WEAK_SCORE / 2 and part_scale > 0 and abs(db(part_scale / abs(item.coefficient))) <= WEAK_LEVEL_DB
        if value < PART_SCORE and not level_holds:
            break
        played = q + 1
    findings['played'] = played / float(quarters)
    if played < quarters:
        findings['cut'] = True


def check_capture(capture, findings):
    """Clipping, DC offset and overall level."""
    stats = {}
    for index, channel in enumerate(capture.channels):
        name = 'LR'[index] if len(capture.channels) == 2 else str(index)
        if np is not None:
            values = np.asarray(channel)
            peak = float(np.max(np.abs(values))) if len(values) else 0.0
            mean = float(np.mean(values)) if len(values) else 0.0
            shaped = int(np.count_nonzero(np.abs(values) > SOFT_CLIP))
            full = int(np.count_nonzero(np.abs(values) >= 32767.0 / 32768.0))
        else:
            peak = max((abs(v) for v in channel), default=0.0)
            mean = sum(channel) / len(channel) if channel else 0.0
            shaped = sum(1 for v in channel if abs(v) > SOFT_CLIP)
            full = sum(1 for v in channel if abs(v) >= 32767.0 / 32768.0)
        stats[name] = {'peak_db': round(db(peak), 1), 'rms_db': round(db(rms(channel)), 1), 'dc': round(mean, 5),
                       'soft_clipped': shaped, 'full_scale': full}
        if shaped > 0:
            findings.append({'flag': 'clipping', 'channel': name, 'samples': shaped, 'full_scale': full,
                             'text': '%d samples on channel %s above %.1f, where the mixer shapes peaks; %d at full scale'
                                     % (shaped, name, SOFT_CLIP, full)})
        if abs(mean) > 0.005:
            findings.append({'flag': 'dc-offset', 'channel': name, 'dc': round(mean, 5),
                             'text': 'channel %s has a DC offset of %+.4f' % (name, mean)})
    return stats


def check_unexplained(capture, items, findings, frame_seconds=0.02, floor_db=-50.0, margin=0.1, minimum=0.06):
    """Stretches louder than floor_db that no logged sound covers."""
    mono = capture.mono()
    step = int(capture.rate * frame_seconds)
    covered = []
    for item in items:
        end = item.end
        if end is None:
            duration = item.findings.get('duration')
            end = item.start + duration if duration is not None else capture.seconds
        covered.append((item.start - margin, end + margin))
    stretches = []
    current = None
    for index in range(0, len(mono) - step + 1, step):
        t = index / float(capture.rate)
        loud = db(ac_rms(mono[index:index + step])) > floor_db
        inside = any(a <= t <= b for a, b in covered)
        if loud and not inside:
            if current is None:
                current = [t, t + frame_seconds]
            else:
                current[1] = t + frame_seconds
        elif current is not None:
            stretches.append(current)
            current = None
    if current is not None:
        stretches.append(current)
    for a, b in stretches:
        if b - a >= minimum:
            findings.append({'flag': 'unexplained', 'start': round(a, 3), 'end': round(b, 3),
                             'text': 'sound at %.2f-%.2f s that no logged sound explains' % (a, b)})


def check_eva(entries, items, rate, findings, repeat_seconds, late_ms):
    eva = [i for i in items if i.kind == 'eva']
    eva.sort(key=lambda i: i.start)
    for a, b in zip(eva, eva[1:]):
        end = a.end if a.end is not None else a.start + a.findings.get('duration', 0.0)
        if b.start < end - 0.01:
            findings.append({'flag': 'eva-overlap', 'first': a.name, 'second': b.name, 'start': round(b.start, 3),
                             'text': '%s starts at %.2f s while %s still plays (until %.2f s)' % (b.name, b.start, a.name, end)})
    last = {}
    for item in eva:
        previous = last.get(item.name)
        if previous is not None and item.start - previous < repeat_seconds:
            findings.append({'flag': 'eva-repeat', 'line': item.name, 'start': round(item.start, 3),
                             'text': '%s plays again %.2f s after it last started' % (item.name, item.start - previous)})
        last[item.name] = item.start
    # A line is late when it starts long after the announcer was free for it: after its request, and after the line
    # before it ended.
    requests = eva_requests(entries, rate)
    used = set()
    previous_end = None
    for item in eva:
        for index, (t, name) in enumerate(requests):
            if index in used or name != item.name or t > item.start + 1e-6:
                continue
            used.add(index)
            item.request = t
            free = t if previous_end is None else max(t, previous_end)
            delay = (item.start - free) * 1000.0
            item.findings['request_delay_ms'] = int(round((item.start - t) * 1000.0))
            item.findings['wait_ms'] = int(round(delay))
            if delay > late_ms:
                findings.append({'flag': 'eva-late', 'line': item.name, 'delay_ms': int(round(delay)),
                                 'text': '%s starts %d ms after the announcer was free for it (threshold %d ms)' % (item.name, delay, late_ms)})
            break
        previous_end = item.end if item.end is not None else item.start + item.findings.get('duration', 0.0)


def run_check(capture_path, log_path, sound_folders, options):
    capture = read_wav(capture_path)
    report = {'capture': capture_path, 'rate': capture.rate, 'channels': len(capture.channels),
              'seconds': round(capture.seconds, 3), 'numpy': np is not None, 'flags': [], 'sounds': []}
    flags = report['flags']
    if np is None:
        flags.append({'flag': 'no-numpy', 'text': 'numpy is not installed; sounds were matched at %d Hz, which misses '
                                                  'short or quiet ones' % COARSE_RATE})
    entries = read_log(log_path) if log_path and os.path.exists(log_path) else []
    report['log'] = log_path
    report['log_lines'] = len(entries)
    for entry in entries:
        if entry['kind'] == 'capture' and entry['action'] == 'start' and int(entry.get('rate', capture.rate)) != capture.rate:
            flags.append({'flag': 'rate-mismatch', 'text': 'the log states %s Hz but the recording is %d Hz' % (entry['rate'], capture.rate)})
        if entry['kind'] == 'capture' and entry['action'] == 'pause':
            flags.append({'flag': 'capture-paused', 'start': round(entry_seconds(entry, capture.rate), 3),
                          'text': 'the recording paused at %.2f s (%s)' % (entry_seconds(entry, capture.rate), entry.get('reason', ''))})
    report['levels'] = check_capture(capture, flags)
    library = Library(sound_folders)
    items = occurrences_from_log(entries, capture.rate)
    matcher = Matcher(capture)
    checkable = []
    for item in items:
        if item.live:
            item.findings['status'] = 'already-playing'
        elif not options.range or options.range[0] <= item.start <= options.range[1]:
            if prepare_occurrence(item, library, matcher):
                checkable.append(item)
    fit_levels(checkable, matcher)
    for item in checkable:
        judge_occurrence(item, matcher)
    for item in items:
        status = item.findings.get('status')
        name = item.name
        if status in ('silent', 'not-found') and item.findings.get('expected_db', -180) > options.quiet_db:
            flags.append({'flag': 'missing', 'name': name, 'start': round(item.start, 3), 'status': status,
                          'text': '%s %s at %.2f s: %s' % (item.kind, name, item.start,
                                                            'silence where it should play' if status == 'silent' else 'not found in the mix')})
        if status == 'rate-mismatch':
            flags.append({'flag': 'pitch', 'name': name, 'start': round(item.start, 3), 'factor': item.findings['rate_factor'],
                          'text': '%s at %.2f s matches only at %.3f times its rate' % (name, item.start, item.findings['rate_factor'])})
        if status in ('found', 'found-weak') and abs(item.findings['offset_ms']) > options.late_ms:
            flags.append({'flag': 'late', 'name': name, 'start': round(item.start, 3), 'offset_ms': item.findings['offset_ms'],
                          'text': '%s heard %+d ms from its logged start' % (name, item.findings['offset_ms'])})
        if item.findings.get('cut') and item.end_reason == 'finished':
            flags.append({'flag': 'cut-short', 'name': name, 'start': round(item.start, 3), 'played': item.findings['played'],
                          'text': '%s at %.2f s stops after %d%% though the log says it finished' % (name, item.start, 100 * item.findings['played'])})
    check_eva(entries, items, capture.rate, flags, options.repeat_seconds, options.eva_late_ms)
    if entries:
        check_unexplained(capture, items, flags, floor_db=options.floor_db)
    for item in items:
        record = {'start': round(item.start, 3), 'frame': item.frame, 'kind': item.kind, 'name': item.name,
                  'files': item.files, 'handle': item.handle, 'gain': item.gain, 'pan': item.pan, 'pitch': item.pitch,
                  'end': round(item.end, 3) if item.end is not None else None, 'end_reason': item.end_reason}
        record.update(item.findings)
        report['sounds'].append(record)
    return report, capture, items


def clip_check(path, library_folder, options):
    with tempfile.TemporaryDirectory() as workdir:
        capture = load_any(path, workdir)
    report = {'recording': path, 'rate': capture.rate, 'channels': len(capture.channels),
              'seconds': round(capture.seconds, 3), 'numpy': np is not None, 'flags': [], 'found': []}
    if np is None:
        report['flags'].append({'flag': 'no-numpy', 'text': 'numpy is not installed; matched at %d Hz, slowly' % COARSE_RATE})
    report['levels'] = check_capture(capture, report['flags'])
    library = Library([library_folder])
    matcher = Matcher(capture)
    eva_name = re.compile(options.eva_pattern, re.I)
    for stem in library.names():
        sound = library.get(stem)
        template = matcher.prepare(sound.mono(), sound.rate)[:int(TEMPLATE_SECONDS * matcher.rate)]
        if rms(template) < 1e-4 or len(template) < matcher.rate * 0.05:
            continue
        spacing = max(1, len(template) // 2)
        for offset, score in all_matches(matcher.signal, template, options.clip_score, spacing):
            at = offset / matcher.rate
            segment = matcher.signal[offset:offset + len(template)]
            scale = dot(segment, template) / max(dot(template, template), 1e-12)
            report['found'].append({'start': round(at, 3), 'name': stem, 'score': round(score, 3),
                                    'seconds': round(sound.seconds, 3), 'level_db': round(db(abs(scale)), 1)})
    report['found'].sort(key=lambda f: (f['start'], -f['score']))
    last = {}
    eva = []
    for item in report['found']:
        if not eva_name.search(item['name']):
            continue
        eva.append(item)
        previous = last.get(item['name'])
        if previous is not None and item['start'] - previous < options.repeat_seconds:
            report['flags'].append({'flag': 'eva-repeat', 'line': item['name'], 'start': item['start'],
                                    'text': '%s heard again %.2f s after it was last heard' % (item['name'], item['start'] - previous)})
        last[item['name']] = item['start']
    for a, b in zip(eva, eva[1:]):
        if b['start'] < a['start'] + a['seconds'] - 0.05 and a['name'] != b['name']:
            report['flags'].append({'flag': 'eva-overlap', 'first': a['name'], 'second': b['name'], 'start': b['start'],
                                    'text': '%s heard at %.2f s while %s still plays' % (b['name'], b['start'], a['name'])})
    return report, capture


# ----------------------------------------------------------------------------------------------------------------
# Output


def format_run_report(report):
    out = []
    out.append('Audio check: %s (%d Hz, %d channels, %.2f s)' % (report['capture'], report['rate'], report['channels'], report['seconds']))
    out.append('Log: %s, %d lines; %d sounds started' % (report.get('log') or '-', report.get('log_lines', 0), len(report['sounds'])))
    for name, stats in report['levels'].items():
        out.append('Channel %s: peak %.1f dBFS, RMS %.1f dBFS, DC %+.4f, %d samples shaped, %d at full scale'
                   % (name, stats['peak_db'], stats['rms_db'], stats['dc'], stats['soft_clipped'], stats['full_scale']))
    out.append('')
    out.append('%8s %6s %-5s %-30s %-16s %7s %6s %7s %8s %6s  %s' % ('time s', 'frame', 'kind', 'name', 'file', 'expect', 'score',
                                                                  'offset', 'level', 'played', 'status'))
    for s in report['sounds']:
        if 'status' not in s:
            continue
        files = ','.join(s['files'])
        out.append('%8.3f %6d %-5s %-30s %-16s %7s %6s %7s %8s %6s  %s%s' % (
            s['start'], s['frame'], s['kind'], s['name'][:30], files[:16],
            '%.1f' % s['expected_db'] if 'expected_db' in s else '-',
            '%.2f' % s['score'] if 'score' in s else '-',
            '%+dms' % s['offset_ms'] if 'offset_ms' in s else '-',
            '%+.1fdB' % s['level_error_db'] if 'level_error_db' in s else '-',
            '%d%%' % (100 * s['played']) if 'played' in s else '-',
            s['status'], ' (%s)' % s['end_reason'] if s.get('end_reason') and s['end_reason'] != 'finished' else ''))
    out.append('')
    if report['flags']:
        out.append('Flags:')
        for flag in report['flags']:
            out.append('  %-14s %s' % (flag['flag'], flag['text']))
    else:
        out.append('Flags: none')
    statuses = {}
    for s in report['sounds']:
        statuses[s.get('status', 'skipped')] = statuses.get(s.get('status', 'skipped'), 0) + 1
    out.append('')
    out.append('Summary: ' + ', '.join('%d %s' % (count, status) for status, count in sorted(statuses.items())) +
               '; %d flags' % len(report['flags']))
    return '\n'.join(out) + '\n'


def format_clip_report(report):
    out = ['Clip check: %s (%d Hz, %d channels, %.2f s)' % (report['recording'], report['rate'], report['channels'], report['seconds'])]
    for name, stats in report['levels'].items():
        out.append('Channel %s: peak %.1f dBFS, RMS %.1f dBFS, DC %+.4f' % (name, stats['peak_db'], stats['rms_db'], stats['dc']))
    out.append('')
    out.append('%8s  %-24s %6s %8s %8s' % ('time s', 'sound', 'score', 'level', 'length'))
    for item in report['found']:
        out.append('%8.3f  %-24s %6.2f %+7.1fdB %7.2fs' % (item['start'], item['name'], item['score'], item['level_db'], item['seconds']))
    out.append('')
    out.append('Flags:' if report['flags'] else 'Flags: none')
    for flag in report['flags']:
        out.append('  %-14s %s' % (flag['flag'], flag['text']))
    return '\n'.join(out) + '\n'


def write_png(path, pixels, width, height):
    """A greyscale PNG from rows of 0..255 values."""
    raw = b''.join(b'\x00' + bytes(row) for row in pixels)

    def chunk(kind, data):
        body = kind + data
        return struct.pack('>I', len(data)) + body + struct.pack('>I', zlib.crc32(body) & 0xFFFFFFFF)

    with open(path, 'wb') as handle:
        handle.write(b'\x89PNG\r\n\x1a\n')
        handle.write(chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 0, 0, 0, 0)))
        handle.write(chunk(b'IDAT', zlib.compress(raw, 9)))
        handle.write(chunk(b'IEND', b''))


def spectrogram(path, capture, start, end, marks=(), height=256, columns_per_second=200):
    """Writes a spectrogram of start..end seconds: time runs left to right, frequency bottom to top up to 12 kHz,
    louder is lighter on a 90 dB scale. A short bright tick along the top marks each start in marks."""
    if np is None:
        return 'spectrogram skipped: it needs numpy'
    mono = np.asarray(capture.mono())
    a = max(0, int(start * capture.rate))
    b = min(len(mono), int(end * capture.rate))
    if b - a < 1024:
        return 'spectrogram skipped: the range holds too little audio'
    size = 2048
    hop = max(1, capture.rate // columns_per_second)
    window = np.hanning(size)
    columns = []
    for offset in range(a, b - size, hop):
        spectrum = np.abs(np.fft.rfft(mono[offset:offset + size] * window))
        columns.append(spectrum)
    if not columns:
        return 'spectrogram skipped: the range is shorter than one analysis window'
    image = np.array(columns).T
    top = int(12000 * size / capture.rate)
    image = image[:top]
    levels = 20.0 * np.log10(np.maximum(image, 1e-9) / (size / 4))
    scaled = np.clip((levels + 90.0) / 90.0, 0.0, 1.0)
    rows = np.linspace(0, top - 1, height).astype(int)[::-1]
    pixels = (scaled[rows] * 255).astype(np.uint8)
    width = pixels.shape[1]
    for t in marks:
        column = int((t - start) * columns_per_second)
        if 0 <= column < width:
            pixels[:6, column] = 255
    write_png(path, pixels.tolist(), width, height)
    return 'spectrogram written to %s (%d x %d)' % (path, width, height)


def time_range(text):
    if not text:
        return None
    start, _, end = text.partition(':')
    return (float(start or 0.0), float(end or 1e9))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = parser.add_subparsers(dest='mode', required=True)
    run = sub.add_parser('run', help='check an audiocapture recording against its log')
    run.add_argument('capture')
    run.add_argument('--log', help='the audio test log (default: the recording\'s name with .log)')
    run.add_argument('--sounds', action='append', default=[], help='folder of decoded sounds (default: <name>.sounds); repeatable')
    run.add_argument('--library', action='append', default=[], help='another folder of decoded sounds, such as audiodump writes')
    clip = sub.add_parser('clip', help='find known sounds in any recording, without a log')
    clip.add_argument('recording')
    clip.add_argument('--library', required=True, help='folder of decoded sounds to look for')
    clip.add_argument('--score', dest='clip_score', type=float, default=0.6, help='correlation needed to report a sound (default 0.6)')
    clip.add_argument('--eva-pattern', default=r'^[a-z]?eva\d', help='names treated as announcer lines (regular expression)')
    for p in (run, clip):
        p.add_argument('--json', help='write the findings as JSON to this file')
        p.add_argument('--report', help='write the text report to this file instead of standard output')
        p.add_argument('--spectrogram', help='write a PNG spectrogram of --range (default the first 10 s) to this file')
        p.add_argument('--range', type=time_range, help='START:END in seconds: limits the spectrogram, and for run the sounds checked')
        p.add_argument('--repeat-seconds', type=float, default=5.0, help='flag an announcer line heard again within this time (default 5)')
    run.add_argument('--late-ms', type=int, default=100, help='flag a sound heard further than this from its logged start (default 100)')
    run.add_argument('--eva-late-ms', type=int, default=600, help='flag an announcer line starting this long after it was asked for and the line before ended (default 600)')
    run.add_argument('--quiet-db', type=float, default=-45.0, help='sounds predicted quieter than this are not flagged as missing (default -45)')
    run.add_argument('--floor-db', type=float, default=-50.0, help='stretches louder than this need a logged sound (default -50)')
    options = parser.parse_args(argv)

    if options.mode == 'run':
        stem = os.path.splitext(options.capture)[0]
        log_path = options.log or stem + '.log'
        folders = (options.sounds or [stem + '.sounds']) + options.library
        report, capture, items = run_check(options.capture, log_path, folders, options)
        text = format_run_report(report)
        marks = [i.start for i in items]
    else:
        report, capture = clip_check(options.recording, options.library, options)
        text = format_clip_report(report)
        marks = [f['start'] for f in report['found']]
    if options.spectrogram:
        start, end = options.range or (0.0, 10.0)
        note = spectrogram(options.spectrogram, capture, start, min(end, capture.seconds), marks)
        text += note + '\n'
        report['spectrogram'] = note
    if options.json:
        with open(options.json, 'w', encoding='utf-8') as handle:
            json.dump(report, handle, indent=1)
    if options.report:
        with open(options.report, 'w', encoding='utf-8') as handle:
            handle.write(text)
    else:
        sys.stdout.write(text)
    return 0


if __name__ == '__main__':
    sys.exit(main())
