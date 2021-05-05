#pragma once
#include <cstdio>
#include <string>

#define DISABLE_COPY(ClassType)                                                \
  ClassType(const ClassType&) = delete;                                        \
  ClassType& operator=(const ClassType&) = delete;

namespace basic {
namespace fileutils {
class FileReaderWriterBase {
public:
  FileReaderWriterBase(const char* path, const char* mode) {
    file_ = fopen(path, mode);
  }

  bool valid() const {
    return file_ != nullptr;
  }

  ~FileReaderWriterBase() {
    if (file_) {
      fclose(file_);
      file_ = nullptr;
    }
  }

protected:
  FILE* file_;
};

class FileReader : public FileReaderWriterBase {
public:
  FileReader(const std::string& path) :
      FileReaderWriterBase(path.c_str(), "r+") {}

  std::size_t read(void* buf, int size) {
    if (file_ == nullptr) {
      return 0;
    }
    return std::fread(buf, 1, size, file_);
  }
};

class FileWriter : public FileReaderWriterBase {
public:
  FileWriter(const std::string& path) :
      FileReaderWriterBase(path.c_str(), "wb+") {}

  std::size_t write(const void* buf, int size) {
    if (file_ == nullptr) {
      return 0;
    }
    return std::fwrite(buf, 1, size, file_);
  }
};
}  // namespace fileutils
}  // namespace basic
