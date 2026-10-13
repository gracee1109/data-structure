"""Validate the saved index schema and representative source locations."""
import csv
import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'docs/index'
rows = json.loads((OUT / '題目索引.json').read_text(encoding='utf-8'))
assert rows, 'Empty index'
assert len({r['id'] for r in rows}) == len(rows), 'Duplicate IDs'
assert len({(r['school'], r['year'], r['subject'], r['volume'], r['major']) for r in rows}) == len(rows)
limits = {'上冊': 340, '下冊': 414}
for row in rows:
    assert row['year'] in range(111, 116)
    assert row['pages'] and all(13 <= p <= limits[row['volume']] for p in row['pages'])
    assert row['pages'] == sorted(set(row['pages']))
    assert row['labels'] and row['topics']
    assert not {'text', 'question', 'answer', 'heading'} & row.keys()

# 題號／頁碼／主題已對照原PDF文字層；八校各至少一個定位錨點。
anchors = [
    ('臺大', 115, '第4題', '上冊', 14, '堆疊／佇列'),
    ('臺大', 112, '程式設計題', '上冊', 61, '搜尋／字串'),
    ('政大', 112, 'I', '上冊', 163, '動態規劃'),
    ('政大', 115, '第6題', '上冊', 90, '雜湊'),
    ('陽交大', 113, '第12題', '上冊', 226, '圖論'),
    ('成大', 114, 'A-1-4', '上冊', 264, '堆疊／佇列'),
    ('中央', 115, '第3題', '下冊', 32, '堆積／優先佇列'),
    ('中山', 113, '第8題', '下冊', 188, '雜湊'),
    ('中興', 115, '第6題', '下冊', 267, '動態規劃'),
    ('中興', 113, '第5題', '下冊', 287, '鏈結串列／陣列'),
    ('中正', 112, '第12題', '下冊', 392, '動態規劃'),
]
for school, year, major, volume, page, topic in anchors:
    assert any(r['school'] == school and r['year'] == year and r['major'] == major
               and r['volume'] == volume and page in r['pages'] and topic in r['topics']
               for r in rows), (school, year, major)
with (OUT / '題目索引.csv').open(encoding='utf-8-sig', newline='') as f:
    assert len(list(csv.DictReader(f))) == len(rows)
with (OUT / '考卷目錄.csv').open(encoding='utf-8-sig', newline='') as f:
    exams = list(csv.DictReader(f))
assert len(exams) == 68
assert Counter(r['school'] for r in rows) == Counter({'臺大':24,'政大':34,'陽交大':10,'成大':88,'中央':57,'中山':170,'中興':14,'中正':74})
assert all((OUT / (r['school'] + '.md')).is_file() for r in rows)
assert not list(ROOT.rglob('*.cpp'))
print(f'PASS: {len(rows)} rows, 68 exams, 11 source anchors, CSV/JSON consistency; no C++ files')
