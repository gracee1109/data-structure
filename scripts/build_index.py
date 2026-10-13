#!/usr/bin/env python3
"""Scan local PDFs into a metadata-only exam index; never export answers or PDF text."""
import argparse
import csv
import json
import re
from collections import Counter
from pathlib import Path

import fitz  # PyMuPDF

SCHOOLS = ['臺大', '政大', '陽交大', '成大', '中央', '中山', '中興', '中正']
CHAPTER = re.compile(r'^(臺大|政大|陽交大|成大|中央|中山|中興|中正)(11[1-5])\s*年\s+(.+)')
DASH = '[‐‑–−-]'
QUESTION = re.compile(
    rf'^(?P<label>(?:Section\s+II\s+)?(?:'
    r'第\d+(?:\.\d+)?(?:～\d+(?:\.\d+)?)?(?:\([A-Za-z0-9]+\))*\s*題'
    rf'|[A-Z]{DASH}(?:[IVX]+|\d+)(?:{DASH}\d+)*(?:\([a-z0-9ivx]+\))*'
    r'|[IVX]+\.[a-z0-9]+(?:\([a-z0-9]+\))*'
    r'|(?:程式設計題|短答題|問答題|選擇題|是非題|填充題|填空|問答)\d*(?:\([a-zA-Z0-9]+\))*'
    r'))\s*[（(]'
)
TOPICS = {
    '樹': r'\btree\b|\btrees\b|\bBST\b|\bAVL\b|B[+‐‑–-]?tree|red[ ‐‑–-]?black|splay|traversal|in[‐‑–-]?order|pre[‐‑–-]?order|post[‐‑–-]?order|二元樹|二元搜尋樹|紅黑樹|樹高|走訪|BinarySearchTree|rangeSumBST',
    '排序': r'\bsort\b|sorting|mergesort|quicksort|heapsort|bubble|排序|排序法|partition scheme|median[‐‑–-]of[‐‑–-]three',
    '動態規劃': r'dynamic programming|\bLCS\b|longest common|knapsack|matrix[ ‐‑–-]chain|minimum path sum|grid filled|Pascal|動態規劃|背包|矩陣鏈|最長共同|最長公共',
    '圖論': r'\bgraph\b|\bgraphs\b|digraph|\bBFS\b|\bDFS\b|breadth[ ‐‑–-]first|depth[ ‐‑–-]first|Dijkstra|Floyd[ ‐‑–-]Warshall|Kruskal|\bPrim\b|\bMST\b|spanning tree|shortest path|topolog|4[‐‑–-]cycle|FourCycle|adjacen|strongly connected|bipartite|cut vertex|圖論|最短路徑|拓撲|生成樹|連通分量|相鄰矩陣|迷宮|maze',
    '堆疊／佇列': r'\bstack\b|\bstacks\b|\bqueue\b|\bqueues\b|postfix|prefix notation|prefix expression|infix|parentheses|expression tree|佇列|堆疊|括號|前置|後置|中置',
    '鏈結串列／陣列': r'linked list|linked[‐‑–-]list|鏈結|鏈結串列|\bDLL\b|InsertAfter|reverse.*list|invert.*list|row[ ‐‑–-]major|column[ ‐‑–-]major|陣列位址|array address|minimum value of an array|character array|three[‐‑–-]dimensional',
    '雜湊': r'\bhash\b|\bhashing\b|hash table|hash function|HashCode|hashIndex|linear probing|double hashing|separate chaining|closed hashing|open addressing|Bloom Filter|雜湊|碰撞處理',
    '堆積／優先佇列': r'\bheap\b|\bheaps\b|heapsort|heapify|priority[‐‑– -]queue|\bHuffman\b|streaming.*median|雙堆積|堆積|建堆|中位數|優先佇列',
    '搜尋／字串': r'binary search(?! tree)|sequential search|linear search|\bKMP\b|\bTrie\b|tries|pattern match|rotated|search\(txt|substring|字串比對|二元搜尋(?!樹)|二分搜尋',
    '遞迴／分治': r'recurs|divide[ ‐‑–-]and[ ‐‑–-]conquer|divide and conquer|palindrome|Fibonacci|stairs|staircase|遞迴|分治|回文|費氏',
    '貪婪法': r'\bgreedy\b|fractional knapsack|job sequencing|optimal merge|貪婪|貪心',
    '複雜度／遞迴式': r'complexity|recurrence|asymptotic|big[ ‐‑–-]o\b|\bΘ\b|\bΩ\b|amortized|polynomially bounded|running time|run time|複雜度|成長率|遞迴式|攤還|space requirement|total steps|Stirling|Master Theorem',
    '稀疏矩陣／多項式／集合': r'sparse matrix|sparse matrices|tridiagonal|orthogonal list|\bCSR\b|polynomial addition|generalized list|bit vector|稀疏矩陣|多項式|一般化串列|廣義串列|集合表示',
    '計算理論': r'NP[‐‑–-]complete|NP[‐‑–-]hard|P≠NP|hamiltonian|traveling[‐‑–-]salesman|2[‐‑–-]SAT|3[‐‑–-]SAT|subset sum|partition problem|halting problem|clique problem|independent set|vertex cover|backtracking|branch and bound|prune and search',
    '位元運算': r'bitwise|位元運算|位元\(bitwise\)',
    '抽象資料型態': r'abstract data type|\bADT\b|data structure design|data structures? for|data structures? used|secondary key|inverted list|reentrant list|Composite.*pattern|associative array|symbol table|符號表|資料結構選型',
}
COMPILED = {k: re.compile(v, re.I) for k, v in TOPICS.items()}
# 非資料結構卷的術語碰撞：例如 memory heap、CPU queue、網路碰撞、密碼雜湊。
UNRELATED = re.compile(
    r'CPU scheduling|priority scheduling|multilevel feedback|round robin|semaphore|context switch|'
    r'operating system|multiprocessing|interrupt|spooling|security attack|heap memory|stack memory|memory layout|public[‐‑–-]key|cryptograph|digital signature|'
    r'encryption|encoding|blockchain|\bMD5\b|\bSHA[‐‑–-]?1\b|rootkit|cybersecurity|Ethernet|carrier sense|CSMA|'
    r'CAP theorem|NoSQL|normalization|database transaction|entity.relationship|polymorphism|'
    r'object.oriented programming|inheritance|self.attention|Transformer|RAID|encrypted', re.I)
STOP = {'參考答案', '答題範例', '解析'}


def normalized_label(label):
    return re.sub(DASH, '-', re.sub(r'\s+', '', label))


def major_label(label):
    label = normalized_label(label)
    # 保留題型／Section，不混淆同卷中的選擇題1與問答題1。
    label = re.sub(r'\([^)]*\)', '', label)
    label = re.sub(r'^(第\d+)\..*題$', r'\1題', label)
    label = re.sub(r'^([IVX]+)\.[a-z0-9]+$', r'\1', label)
    return label


def topics_for(question):
    body = '\n'.join(question['text'])
    full_text = question['heading'] + '\n' + body
    # 選項中的干擾術語不是考點；分類以題幹為主。
    choices = list(re.finditer(r'\([A-E]\)|(?:^|\n)[A-E]\.', body))
    if len(choices) >= 2:
        body = body[:choices[0].start()]
    text = question['heading'] + '\n' + body
    topics = [k for k, pattern in COMPILED.items() if pattern.search(text)]
    is_ds = '資料結構' in question['subject']
    if not topics and (is_ds or not UNRELATED.search(full_text)):
        topics = [k for k, pattern in COMPILED.items() if pattern.search(full_text)]
    if not is_ds and UNRELATED.search(full_text):
        # 保留明確的資結／演算法命題；不因選項含 stack/hash 而收進純計概題。
        strong = re.search(r'\bAVL\b|\bBST\b|binary search tree|B[+‐‑–-]?tree|Dijkstra|'
                           r'Kruskal|spanning tree|shortest path|postfix|infix|'
                           r'linked list|hash table|linear probing|\bTrie\b|'
                           r'dynamic programming|knapsack|sorting algorithm', full_text, re.I)
        if not strong:
            return []
    # 少數泛稱索引題，考點只在選項中；固定到已抽查的題目位置。
    overrides = {('臺大', 115, '第16題'): ['樹'],
                 ('臺大', 115, '第1題'): ['雜湊', '複雜度／遞迴式'],
                 ('政大', 111, '第10題'): ['搜尋／字串'],
                 ('中山', 112, '第15題'): ['堆疊／佇列'],
                 ('中正', 111, '第7題'): ['稀疏矩陣／多項式／集合']}
    if (question['school'], question['year'], question['label']) in overrides:
        return overrides[(question['school'], question['year'], question['label'])]
    if is_ds and not topics:
        return ['其他資結觀念（待細分）']
    return topics


def read_pdf(path, volume):
    questions, exams = [], []
    current = None
    question = None
    capture = False
    pages = 0
    with fitz.open(path) as doc:
        pages = len(doc)
        for page_no, page in enumerate(doc, 1):
            # 兩冊前12頁為書首／目錄；避免把目錄當成題目。
            if page_no <= 12:
                continue
            for line in page.get_text().splitlines():
                if '260927' in line or '© 橙' in line or '本檔案' in line:
                    continue  # 僅用於本機文字辨識，輸出不包含 PDF 文字。
                chapter = CHAPTER.match(line)
                if chapter:
                    key = (chapter[1], int(chapter[2]), chapter[3].strip())
                    if key != current:
                        current = key
                        exams.append(dict(school=key[0], year=key[1], subject=key[2],
                                          volume=volume, start_page=page_no))
                        question = None
                        capture = False
                    continue
                match = QUESTION.match(line)
                if match and current:
                    label = normalized_label(match['label'])
                    question = dict(school=current[0], year=current[1], subject=current[2],
                                    volume=volume, page=page_no, label=label, heading=line,
                                    text=[])
                    questions.append(question)
                    capture = True
                    continue
                if line.strip() in STOP:
                    capture = False
                if capture and question and line not in {'必拿', '鑑別', '挑戰', '可略'}:
                    question['text'].append(line)
    return questions, exams, pages


def aggregate(questions):
    grouped = {}
    for q in questions:
        topics = topics_for(q)
        if not topics:
            continue
        major = major_label(q['label'])
        key = (q['school'], q['year'], q['subject'], q['volume'], major)
        if key not in grouped:
            grouped[key] = dict(school=q['school'], year=q['year'], subject=q['subject'],
                                volume=q['volume'], major=major, pages=set(), labels=[], topics=set())
        row = grouped[key]
        row['pages'].add(q['page'])
        if q['label'] not in row['labels']:
            row['labels'].append(q['label'])
        row['topics'].update(topics)
    rows = list(grouped.values())
    for row in rows:
        row['pages'] = sorted(row['pages'])
        classified = [t for t in TOPICS if t in row['topics']]
        row['topics'] = classified or ['其他資結觀念（待細分）']
    return sorted(rows, key=lambda r: (SCHOOLS.index(r['school']), -r['year'],
                                      0 if '資料結構' in r['subject'] else 1,
                                      r['subject'], min(r['pages']), r['major']))


def format_pages(pages):
    runs = []
    start = end = pages[0]
    for n in pages[1:]:
        if n == end + 1:
            end = n
        else:
            runs.append(str(start) if start == end else f'{start}–{end}')
            start = end = n
    runs.append(str(start) if start == end else f'{start}–{end}')
    return ', '.join(runs)


def write_outputs(out, rows, exams, counts):
    out.mkdir(parents=True, exist_ok=True)
    columns = ['索引ID', '學校', '年度（民國）', '考科', '大題號', '涵蓋小題', '冊別', 'PDF頁碼', '主題分類']
    public_rows = []
    for i, row in enumerate(rows, 1):
        row['id'] = f'IDX-{i:04d}'
        public_rows.append([row['id'], row['school'], row['year'], row['subject'], row['major'],
                            '、'.join(row['labels']), row['volume'], format_pages(row['pages']),
                            '、'.join(row['topics'])])
    with (out / '題目索引.csv').open('w', encoding='utf-8-sig', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(columns)
        writer.writerows(public_rows)
    (out / '題目索引.json').write_text(json.dumps(rows, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    report = ['# 第一階段：資料結構與演算法題目索引', '',
              f'掃描上冊 {counts["上冊"]} 頁、下冊 {counts["下冊"]} 頁，共 {sum(counts.values())} 頁；'
              f'辨識 {len(exams)} 份考卷，彙整 {len(rows)} 筆與資結／演算法相關的大題索引。', '',
              '本階段只建立題號、位置與主題清單；未提供答案、解法、複雜度分析或 C++ 實作。', '',
              '## 閱讀規則', '',
              '- **PDF 頁碼**從檔案第一頁起算為 1，包含封面與目錄；不是書上印刷頁碼。',
              '- 頁碼列的是相關題幹或小題標題出現的位置；範圍中的連續頁可合併顯示。'
              '圖、續頁與解答可能另在相鄰頁，這不是整題內容的完整起訖範圍。',
              '- 同一大題的小題合併；羅馬數字 `I.1／I.2` 合併為 `I`。'
              '成大 `A-1-1` 等編號保留原有層級，避免將不同選擇題合併。',
              '- 考科與題型一併保留，區分同校同年度不同組別，以及選擇題1與問答題1。',
              '- 主題由題幹與題目標題的關鍵詞推定；一題可有多個標籤。'
              '「其他資結觀念（待細分）」表示需在下一階段檢視圖片／原題。',
              '- 包含資料結構卷，以及計概／資訊科技卷中的相關題目；不收錄純作業系統、網路、資料庫或管理概念題。'
              '這是快速文字掃描索引，已抽查部分位置，尚未逐題人工覆核。',
              '- 只輸出索引 metadata；PDF、圖片、題幹全文與參考解答不列入儲存庫。', '',
              '## 學校摘要', '', '| 學校 | 來源年度 | 索引筆數 |', '|---|---|---:|']
    for school in SCHOOLS:
        years = sorted({e['year'] for e in exams if e['school'] == school})
        report.append(f'| {school} | {"、".join(map(str, years))} | {sum(r["school"] == school for r in rows)} |')
    report += ['', '陽交大來源只有 113–115 年，沒有 111、112 年；中興的資結題出現在計算機概論合卷。', '',
               '## 主題摘要', '', '| 主題（可重複計數） | 索引筆數 |', '|---|---:|']
    topics = Counter(t for r in rows for t in r['topics'])
    report += [f'| {t} | {n} |' for t, n in topics.most_common()]
    report += ['', '## 題目清單', '']
    for school in SCHOOLS:
        start = len(report)
        report += [f'### {school}', '', '| 索引ID | 年度 | 考科 | 大題號 | 冊別 | PDF頁碼 | 主題分類 |',
                   '|---|---:|---|---|---|---|---|']
        for row in rows:
            if row['school'] == school:
                report.append(f'| {row["id"]} | {row["year"]} | {row["subject"]} | {row["major"]} | '
                              f'{row["volume"]} | {format_pages(row["pages"])} | {"、".join(row["topics"])} |')
        report.append('')
        per_school = [f'# {school}：第一階段題目索引', '',
                      'PDF頁碼從封面起算為1。分類為文字掃描結果；規則與限制見 [完整索引](題目索引.md)。', '']
        per_school.extend(report[start + 2:])
        (out / f'{school}.md').write_text('\n'.join(per_school), encoding='utf-8')
    (out / '題目索引.md').write_text('\n'.join(report), encoding='utf-8')
    with (out / '考卷目錄.csv').open('w', encoding='utf-8-sig', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['學校', '年度（民國）', '考科', '冊別', 'PDF開始頁碼'])
        for e in exams:
            writer.writerow([e['school'], e['year'], e['subject'], e['volume'], e['start_page']])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--upper', required=True, type=Path)
    parser.add_argument('--lower', required=True, type=Path)
    parser.add_argument('--output', type=Path, default=Path('docs/index'))
    args = parser.parse_args()
    questions, exams, counts = [], [], {}
    for path, volume in [(args.upper, '上冊'), (args.lower, '下冊')]:
        q, e, n = read_pdf(path, volume)
        questions.extend(q)
        exams.extend(e)
        counts[volume] = n
    rows = aggregate(questions)
    write_outputs(args.output, rows, exams, counts)
    print(f'Pages: {sum(counts.values())}; exams: {len(exams)}; detected question headings: {len(questions)}; index rows: {len(rows)}')
    for school in SCHOOLS:
        print(f'{school}: {sum(r["school"] == school for r in rows)}')


if __name__ == '__main__':
    main()
