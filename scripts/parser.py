#!/usr/bin/env python3
'Checks if a grammar is LL1 and makes a parsing table for it.'

from __future__ import print_function
from collections import defaultdict, OrderedDict
from collections.abc import Iterable
import sys
import argparse
import json

def debug(*args, **kwargs):
    return print(*args, file=sys.stderr, **kwargs)

END_STR = 'EOF'

def is_nsi(k):
    return isinstance(k, Iterable) and not isinstance(k, (str, bytes))

def get_sortkey(k, non_term_rank):
    if is_nsi(k):
        return (len(k), non_term_rank.get(k[0], 10**9))
    else:
        return non_term_rank.get(k, 10**9)
        
    
def print_dict(dp, non_term_rank, file=sys.stdout):
    for k, v in sorted(dp.items(), key=(lambda x: get_sortkey(x[0], non_term_rank))):
        if is_nsi(k):
            k2 = list(k)
        else:
            k2 = repr(k)
        print('{}: {}'.format(k2, v), file=file)

class ParseTable:
    start_symbol = None
    grammar = {}
    non_term_rank = {}
    terminals = set()
    first_dp = {}
    follow_dp = defaultdict(set)
    parse_table = defaultdict(list)
    __follow_changed = True
    is_ll1 = False

    def __init__(self, start_symbol, grammar):
        self.start_symbol = start_symbol
        self.grammar = grammar
        for i, k in enumerate(grammar.keys()):
            self.non_term_rank[k] = i
        self.update_terminals()
        self.update_first()
        self.update_follow()
        self.update_parse_table()

    def update_terminals(self):
        self.terminals = set()
        for alphas in self.grammar.values():
            for alpha in alphas:
                self.terminals.update(alpha)
        self.terminals.difference_update(self.grammar.keys())

    def first_symbol(self, ch):
        #debug('first_symbol({})'.format(ch))
        w = (ch,)
        if ch in self.terminals:
            return {ch}
        else:
            if w not in self.first_dp:
                s = set()
                for alpha in self.grammar[ch]:
                    s.update(self.first_str(tuple(alpha)))
                self.first_dp[w] = s
            return self.first_dp[w]

    def first_str(self, w):
        #debug('first_str({})'.format(list(w)))
        if len(w) == 0:
            return {''}
        elif w[0] in self.terminals:
            return {w[0]}
        else:
            if w not in self.first_dp:
                s = self.first_symbol(w[0])
                if '' in s:
                    self.first_dp[w] = self.first_str(w[1:]).union(s - {''})
                else:
                    self.first_dp[w] = s
            return self.first_dp[w]

    def update_follow_helper(self, X, alpha):
        #debug('update_follow_helper({}, {})'.format(X, alpha))
        for i, ch in enumerate(alpha):
            if ch not in self.terminals:
                s = self.first_str(tuple(alpha[i+1:]))
                s2 = s - {''}
                if not(s2 <= self.follow_dp[ch]):
                    self.__follow_changed = True
                    self.follow_dp[ch].update(s2)
                if '' in s:
                    s3 = self.follow_dp[X]
                    if not(s3 <= self.follow_dp[ch]):
                        self.__follow_changed = True
                        self.follow_dp[ch].update(self.follow_dp[X])

    def update_follow(self):
        self.follow_dp[self.start_symbol] = {END_STR}
        while self.__follow_changed:
            self.__follow_changed = False
            for X, alphas in self.grammar.items():
                for alpha in alphas:
                    self.update_follow_helper(X, alpha)
            debug('calculated follow')

    def update_first(self):
        for X, alphas in self.grammar.items():
            self.first_symbol(X)
            for alpha in alphas:
                self.first_str(tuple(alpha))

    def update_parse_table(self):
        self.update_follow()
        for X, alphas in self.grammar.items():
            for alpha in alphas:
                fa = self.first_str(tuple(alpha))
                if '' in fa:
                    terms = self.follow_dp[X].union(fa - {''})
                else:
                    terms = fa
                for term in terms:
                        self.parse_table[(X, term)].append(alpha)
        self.is_ll1 = True
        for X, alphas in self.parse_table.items():
            if len(alphas) > 1:
                self.is_ll1 = False

def main(*args):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('file', help='File containing grammar')
    parser.add_argument('-j', action='store_true', default=False, help='Use JSON')
    parser.add_argument('--first', action='store_true', default=False, help='Print first')
    parser.add_argument('--follow', action='store_true', default=False, help='Print follow')
    args = parser.parse_args(*args)

    with open(args.file) as fobj:
        if args.j:
            decoder = json.JSONDecoder(object_pairs_hook=OrderedDict)
            start_symbol, grammar = decoder.decode(fobj.read())
        else:
            start_symbol = fobj.readline().strip()
            grammar = OrderedDict()
            rule_to_line = {}
            count = 0
            for line in fobj:
                if line.strip():
                    X, *alpha = line.split()[:-1]
                    rule_to_line[tuple(alpha)] = count
                    if X in grammar:
                        grammar[X].append(alpha)
                    else:
                        grammar[X] = [alpha]
                    count += 1

    pt = ParseTable(start_symbol, grammar)

    if args.first:
        print('first:')
        print_dict(pt.first_dp, pt.non_term_rank)
        print()
    if args.follow:
        print('follow:')
        print_dict(pt.follow_dp, pt.non_term_rank)
        print()
    if args.j:
        print_dict(pt.parse_table, pt.non_term_rank)
        print()
    else:
        def sortkey(x):
            return (pt.non_term_rank.get(x[0][0], 10**9), x[0][1])
        for (X, a), alphas in sorted(pt.parse_table.items(), key=sortkey):
            print(X, a, rule_to_line[tuple(alphas[0])])

    ambg_rules = {k: v for k, v in pt.parse_table.items() if len(v) > 1}
    if ambg_rules:
        print("\nambiguous rules:", file=sys.stderr)
        print_dict(ambg_rules, file=sys.stderr)
    else:
        print("\nLanguage is LL(1)", file=sys.stderr)

if __name__ == '__main__':
    main()
