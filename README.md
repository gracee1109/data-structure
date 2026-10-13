# 八校資管所：資料結構與演算法考題索引

**第一階段只提供目錄。** 目前掃描兩冊共 754 頁、68 份考卷，整理出 **471 筆大題索引**；沒有解答、複雜度分析或 C++ 程式碼。

- [完整索引](docs/index/題目索引.md)：學校、民國年度、考科、大題號、PDF頁碼、主題。
- [CSV 索引](docs/index/題目索引.csv)：含小題編號，適合下載後用 Excel／Google Sheets 篩選。
- [JSON 索引](docs/index/題目索引.json)：供後續分批處理。
- [68 份考卷目錄](docs/index/考卷目錄.csv)：各卷的 PDF 開始頁碼。

| 學校 | 資料年度（民國） | 大題索引筆數 | 分校目錄 |
|---|---|---:|---|
| 臺大 | 111–115 | 24 | [臺大](docs/index/臺大.md) |
| 政大 | 111–115 | 34 | [政大](docs/index/政大.md) |
| 陽明交大 | 113–115 | 10 | [陽交大](docs/index/陽交大.md) |
| 成大 | 111–115 | 88 | [成大](docs/index/成大.md) |
| 中央 | 111–115 | 57 | [中央](docs/index/中央.md) |
| 中山 | 111–115 | 170 | [中山](docs/index/中山.md) |
| 中興 | 111–115 | 14 | [中興](docs/index/中興.md) |
| 中正 | 111–115 | 74 | [中正](docs/index/中正.md) |

PDF頁碼從**封面為第1頁**起算，不是印刷頁碼。同一大題的小題合併，因此筆數不等於小題總數。中興的資結題收在計算機概論合卷，陽明交大來源未包含111、112年。

主題是快速文字掃描的初步分類；已抽查八校部分題號與頁碼，尚未逐題人工覆核。7筆暫列「其他資結觀念（待細分）」，需後續檢視原題／圖片。各卷資料結構題，以及計概卷中涉及資料結構、演算法和計算理論的題目均可列入；不代表已完成逐題解答或精確的出題頻率統計。

來源為使用者提供的《資管所考古題111–115計概資結全解》上、下冊，v0.4，2026年9月。本儲存庫只保存題目位置與分類資訊。

## 重新掃描

Python 3.12與PyMuPDF 1.26.6已在目前雲端環境實際執行過。PDF保留於本機，參數指定各冊路徑：

```bash
python3 -m venv --system-site-packages /workspace/.venvs/exam-index
/workspace/.venvs/exam-index/bin/python -m pip install -r requirements-index.txt
/workspace/.venvs/exam-index/bin/python scripts/build_index.py \
  --upper '/path/to/上冊.pdf' \
  --lower '/path/to/下冊.pdf' \
  --output docs/index
python3 tests/validate_index.py
```

`--output`目錄中的同名索引檔會重新產生。腳本只掃描文字層，不執行OCR，不匯出題幹或解答。圖片題的精確分類留待下一階段覆核。

## 分批處理

先確認索引，再以索引ID指定題目，例如「處理IDX-0001至IDX-0005」。下一階段的C++17實作、解答與延伸題，須在使用者確認批次後才開始。
