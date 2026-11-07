#!/usr/bin/env python3
"""Consolidated dclang evidence inventory and integrity checks (not a runtime replayer)."""
import argparse, json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def load(p):return json.loads((ROOT/p).read_text())
def main():
 p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');p.add_argument('--summary',action='store_true');a=p.parse_args()
 prim=load('docs/ai/data/primitives.json');std=load('docs/ai/data/stdlib.json')
 pc=load('tests/ai/primitives.json');sc=load('tests/ai/stdlib.json')
 names=[x['name'] for x in prim['primitives']]
 assert len(names)==213 and len(set(names))==213, 'primitive inventory mismatch'
 assert all(x.get('stack_effect') for x in prim['primitives']), 'missing primitive stack signature'
 assert len(std['modules'])==20, 'module inventory mismatch'
 assert all('record' in x and 'source' in x for x in pc['cases']+sc['cases'])
 if a.summary or not a.check:
  print('Primitives:',len(names),'with signatures:',sum(bool(x.get('stack_effect')) for x in prim['primitives']))
  print('Modules:',len(std['modules']),'definitions:',sum(len(m['words']) for m in std['modules']))
  print('Historical primitive cases:',len(pc['cases']),'standard-library cases:',len(sc['cases']))
  print('Source audit artifacts:',len(pc['source_audits']))
 if a.check:print('Integrity check passed (not a runtime test).')
if __name__=='__main__':main()
