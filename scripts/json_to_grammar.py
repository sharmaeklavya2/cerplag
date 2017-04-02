#!/usr/bin/env python3

from collections import OrderedDict
import json
import sys, os


decoder = json.JSONDecoder(object_pairs_hook=OrderedDict)
start_symbol, json_obj = decoder.decode(open(sys.argv[1]).read())

terminals = set()
for x, alphas in json_obj.items():
    for alpha in alphas:
        terminals.update(alpha)

terminals.difference_update(json_obj.keys())

print(start_symbol)
for X, alphas in json_obj.items():
    for alpha in alphas:
        print(X, *alpha, '.')
