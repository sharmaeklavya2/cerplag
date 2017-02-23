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

def get_symbol(s):
	if s in terminals:
		return s
	else:
		return '<{}>'.format(s)

def get_term(x):
	if len(x) == 0:
		return "e"
	else:
		return ' '.join([get_symbol(s) for s in x])

for x, alphas in json_obj.items():
	print('<{}> ->'.format(x), " | ".join([get_term(alpha) for alpha in alphas]))
