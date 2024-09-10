#ifndef RANGE_CSV_H_
#define RANGE_CSV_H_

#include <iostream>
#include <bzlib.h>
#include <stdio.h>
#include <string>
#include <string_view>

class RangeBz2File {
public:
  // TODO: Have nice user interface for error reporing
  enum ErrorType {
    NONE,
    READ_OPEN,
    BZ_READ,
  };

  RangeBz2File(FILE *file);
  ~RangeBz2File();

  class Iterator {
  public:
    // iterator traits
    using difference_type = int;
    using value_type = const char;
    using pointer = const char *;
    using reference = const char &;
    using iterator_category = std::forward_iterator_tag;

    Iterator(RangeBz2File *range, bool done);

    Iterator &operator++() {
      ++current_char;
      if (*current_char == '\0') {
        if (last_block) {
          done = true;
        } else {
          last_block = range->read_next_chunk();
          current_char = &range->buf[0];
        }
      }

      return *this;
    }
    Iterator operator++(int) {
      Iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(Iterator other) const { return done == other.done; }
    bool operator!=(Iterator other) const { return !(*this == other); }

    const char operator*() { return *current_char; }

  private:
    RangeBz2File *range;
    bool done;
    char *current_char;
    bool last_block{false};
  };

  Iterator begin() { return {this, false}; }
  Iterator end() { return {this, true}; }

private:
  FILE *file;
  BZFILE *bzfile;
  ErrorType error{NONE};

  // buffer for reading source text data
  static constexpr int buf_len{1024};
  char buf[buf_len + 1];

  // if true, should stop after this, otherwise continue
  bool read_next_chunk() {
    int bzerror{0};
    int nread{0};
    nread = BZ2_bzRead(&bzerror, bzfile, buf, buf_len);
    buf[nread] = '\0'; // make sure to null terminate the result

    if (bzerror != BZ_OK && bzerror != BZ_STREAM_END) {
      std::cout << "bzRead error: " << bzerror << '\n';
      BZ2_bzReadClose(&bzerror, bzfile);

      error = BZ_READ;
      return true;
    }

    if (bzerror == BZ_STREAM_END) {
      return true;
    }

    return false;
  }
};

class RangeFile {
public:
  RangeFile(std::string_view filename) {
    file = fopen(filename.begin(), "rb");
  }
  ~RangeFile() { fclose(file); }

  FILE * get() { return file; }

private:
  FILE *file;
};

// TODO: Support reading from plain text files, and not just bz2 files
//template<typename T>
class RangeCSV {
public:
  using T = RangeBz2File; // debug
  RangeCSV(T& range) : in_begin{range.begin()}, in_end{range.end()} {}

  enum NodeType {
    SCALAR,
    NEW_LINE
  };
  struct Node {
    NodeType type;
    std::string_view val; // only valid if type == SCALAR
  };

  class Iterator {
  public:
    // iterator traits
    using difference_type = int;
    using value_type = Node;
    using pointer = Node;
    using reference = Node;
    using iterator_category = Node;

    Iterator(RangeCSV *range, bool done) : range{range}, done{done} {
      if (done)
        return;

      last_node = range->next_node();
    }

    Iterator &operator++() {
      if (!last_node)
        last_node = range->next_node();
      else
        done = true;

      return *this;
    }
    Iterator operator++(int) {
      Iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(Iterator other) const { return done == other.done; }
    bool operator!=(Iterator other) const { return !(*this == other); }

    Node operator*() const {
      return {range->node_type, range->current_string};
    }

  private:
    RangeCSV *range;
    bool done;
    bool last_node{false};
  };

  Iterator begin() { return {this, false}; }
  Iterator end() { return {this, true}; }

private:
  T::Iterator in_begin;
  T::Iterator in_end;

  NodeType    node_type{SCALAR};
  std::string current_string;

  // return true if done processing text, false otherwise
  bool next_node() {
    current_string.clear();

    char ch = *in_begin;
    if (ch == '\n') {
      // new line case
      node_type = NEW_LINE;
      return (++in_begin == in_end);
    }

    // regular string case
    node_type = SCALAR;

    // quoted string case
    if (ch == '"') {
      ++in_begin; // look at the next character

      for (; in_begin != in_end; ++in_begin) {
        ch = *in_begin;
        if (ch == '\\' && ++in_begin != in_end) {
          ch = *in_begin;
          current_string.push_back(ch);
          continue;
        }

        if (ch == '"' && ++in_begin != in_end) {
          ch = *in_begin;
          if (ch == '"') {
            current_string.push_back('"');
          } else if (ch == '\n') {
            break;
            //return in_begin == in_end; // done
          } else if (ch == ',') {
            ++in_begin;
            break;
          } else {
            std::cout << "\nERROR: bad format for quoted string\n";
            return true;
          }
          continue;
        }

        // not quoted character case
        current_string.push_back(ch);
      }

      return in_begin == in_end;
    }

    // regular string case
    for (; in_begin != in_end; ++in_begin) {
      char ch = *in_begin;

      // done!
      if (ch == ',') {
        // move cursor to next character before finishing
        ++in_begin;
        return in_begin == in_end;
      }
      if (ch == '\n') {
        // don't move cursor to next character before finishing
        // (since next node will be NEW_LINE)
        //++in_begin;
        return in_begin == in_end;
      }

      // otherwise just consume characters
      current_string.push_back(ch);
    }

    return ++in_begin == in_end;
  }
};


#endif // RANGE_CSV_H_
