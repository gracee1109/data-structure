#!/usr/bin/env python3
"""Compile and execute every indexed NCCU C++17 solution without repository build outputs."""
import argparse
import concurrent.futures
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--jobs', type=int, default=2)
    p.add_argument('--sanitize', action='store_true')
    p.add_argument('--write-results', action='store_true')
    args = p.parse_args()
    index = json.loads((ROOT / 'docs/index/題目索引.json').read_text())
    ids = [r['id'] for r in index if r['school'] == '政大']
    sources = [ROOT / 'solutions/nccu' / (id + '.cpp') for id in ids]
    if (ROOT / 'tests/nccu_properties.cpp').is_file():
        sources.append(ROOT / 'tests/nccu_properties.cpp')
    assert all(s.is_file() for s in sources), 'Missing solution file'
    assert {s.stem for s in (ROOT / 'solutions/nccu').glob('IDX-*.cpp')} == set(ids)
    flags = ['-std=c++17', '-Wall', '-Wextra', '-Wpedantic', '-Werror']
    flags += ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer'] if args.sanitize else ['-O2']
    env = dict(os.environ)
    if args.sanitize:
        env['ASAN_OPTIONS'] = 'detect_leaks=1:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    failures = []
    with tempfile.TemporaryDirectory(prefix='nccu-tests-') as temp:
        def run(source):
            executable = Path(temp) / source.stem
            compile = subprocess.run(['g++', *flags, str(source), '-o', str(executable)], capture_output=True, text=True)
            if compile.returncode:
                return source.stem, False, compile.stdout + compile.stderr
            result = subprocess.run([str(executable)], env=env, capture_output=True, text=True, timeout=30)
            if result.returncode:
                return source.stem, False, result.stdout + result.stderr
            if args.write_results and source.stem in ids:
                out = ROOT / 'solutions/nccu/results'
                out.mkdir(exist_ok=True)
                (out / (source.stem + '.txt')).write_text(result.stdout)
            return source.stem, True, result.stdout
        with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
            for id, passed, output in pool.map(run, sources):
                if passed and id == 'nccu_properties':
                    print(output.strip())
                if not passed:
                    failures.append(id)
                    print(f'FAIL {id}\n{output}')
        print(f'{len(sources)-len(failures)}/{len(sources)} executables passed; {len(ids)} indexed solutions; mode={"ASan+UBSan" if args.sanitize else "C++17 -O2"}')
    if failures:
        raise SystemExit(1)

if __name__ == '__main__':
    main()
