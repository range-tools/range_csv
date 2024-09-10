# RangeCSV

C++ range for loop interface for csv.bz2 files.

Dependencies:

``` sh
sudo apt install libbz2-dev
```

Example usage:

```c++
RangeBz2File bzfile{file_descriptor};
RangeCSV csv{bzfile};

for (RangeCSV::Node n : csv) {
  if (n.type == RangeCSV::NEW_LINE)
    std::cout << '\n';
  else if (n.type == RangeCSV::SCALAR)
    std::cout << n.val << '\t';
}
```

See `tests/parse_and_print.cpp` for a full example.

## Bonus:
- You can also use this library to read bz2 files character by character, just
  iterate over `bzfile` in the example above instead of `csv`.

License: MIT
