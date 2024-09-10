#include <iostream>
#include <range_csv.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <path_to_file.csv.bz2>\n";
    return 0;
  }

  const char *filename = argv[1];
  RangeFile file{filename};

  if (!file.get()) {
    std::cout << "Failed to open file\n";
    return EXIT_FAILURE;
  }
  RangeBz2File bzfile{file.get()};

  // for (const char c : bzfile) {
  //   std::cout << c;
  // }

  RangeCSV csv{bzfile};

  for (RangeCSV::Node n : csv) {
    if (n.type == RangeCSV::NEW_LINE)
      //std::cout << "\n---\n"; // debug
      std::cout << '\n';
    else if (n.type == RangeCSV::SCALAR)
      //std::cout << "n.val: " << n.val << '\n';
      std::cout << n.val << '\t';
  }
}
