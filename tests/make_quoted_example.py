#!/usr/bin/env python3

import pandas as pd

df = pd.DataFrame({
    'a': [1,2,3,4,5],
    'b': [['"f\\oo"'], ['"a"','b'], ['c'], ['"d"','e'], ['x', 'y']]
})
df.to_csv('py_quoted.csv.bz2')
