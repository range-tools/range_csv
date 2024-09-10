#include "range_csv.h"

RangeBz2File::RangeBz2File(FILE *file) : file{file} {
  // setup read buffer
  buf[buf_len] = '\0';

  // setup BZ2
  int bzerror{0};
  int verbosity{0}; // 0-4, bigger means more verbose
  bzfile = BZ2_bzReadOpen(&bzerror, file, verbosity, 0, nullptr, 0);
  if (bzerror != BZ_OK) {
    std::cout << "bzReadOpen error: " << bzerror << '\n';
    BZ2_bzReadClose(&bzerror, bzfile);
    error = READ_OPEN;
  }
}

RangeBz2File::~RangeBz2File() {
  if (error)
    return;

  int bzerror{};
  BZ2_bzReadClose(&bzerror, bzfile);
}

RangeBz2File::Iterator::Iterator(RangeBz2File *range, bool done)
    : range{range}, done{done} {
  if (done)
    return;

  last_block = range->read_next_chunk();
  current_char = &range->buf[0];
}
