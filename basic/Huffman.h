#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "Utils.h"

namespace basic {
template <typename ValueType>
struct HuffmanTreeNode {
  HuffmanTreeNode() : pval(nullptr), left(nullptr), right(nullptr) {}

  ValueType* pval;
  HuffmanTreeNode* left;
  HuffmanTreeNode* right;
};

template <typename ValueType>
class HuffmanEncoder {
public:
  HuffmanEncoder(ValueType* data, int size);
  // TODO: impl
  ~HuffmanEncoder();
  DISABLE_COPY(HuffmanEncoder)

  bool encode();
  std::map<ValueType*, std::string> get_huffman_table() {
    return huffman_table_;
  }

private:
  using TreeNode = HuffmanTreeNode<ValueType>;

  void build_tree();
  void build_huffman_table();

private:
  ValueType* data_;
  int size_;
  TreeNode* root_;
  std::map<ValueType*, std::string> huffman_table_;
};

template <typename ValueType>
inline HuffmanEncoder<ValueType>::HuffmanEncoder(ValueType* data, int size) :
    data_(data), size_(size), root_(nullptr) {}

template <typename ValueType>
inline HuffmanEncoder<ValueType>::~HuffmanEncoder() {
  using DfsFunc = std::function<void(TreeNode*)>;
  DfsFunc dfs_del = [&](TreeNode* root) {
    if (!root) {
      return;
    }
    auto* left = root->left;
    auto* right = root->right;
    delete root;
    root = nullptr;
    dfs_del(left);
    dfs_del(right);
  };
  dfs_del(root_);
  root_ = nullptr;
  huffman_table_.clear();
  data_ = nullptr;
  size_ = 0;
}

// TODO: check if data is nullptr
template <typename ValueType>
inline bool HuffmanEncoder<ValueType>::encode() {
  build_tree();
  build_huffman_table();
  return true;
}

template <typename ValueType>
inline void HuffmanEncoder<ValueType>::build_tree() {
  auto counter_comp_func = [](const TreeNode* lhs, const TreeNode* rhs) {
    return *(lhs)->pval < *(rhs)->pval;
  };

  using namespace std;
  using HuffMap = map<TreeNode*, int, decltype(counter_comp_func)>;
  HuffMap counter(counter_comp_func);
  for (int i = 0; i < size_; ++i) {
    TreeNode* node = new TreeNode();
    node->pval = &data_[i];
    ++counter[node];
  }

  using PqValType =
      pair<typename HuffMap::key_type, typename HuffMap::mapped_type>;
  auto pq_comp_func = [](const PqValType& lhs, const PqValType& rhs) {
    return lhs.second > rhs.second;
  };
  using HuffPq =
      priority_queue<PqValType, vector<PqValType>, decltype(pq_comp_func)>;

  HuffPq pq(pq_comp_func);
  for (auto it = counter.begin(); it != counter.end(); ++it) {
    pq.push(make_pair(it->first, it->second));
  }

  while (pq.size() > 1) {
    auto lnode = pq.top();
    pq.pop();
    auto rnode = pq.top();
    pq.pop();

    TreeNode* node = new TreeNode();
    node->left = lnode.first;
    node->right = rnode.first;
    pq.push(make_pair(node, lnode.second + rnode.second));
  }

  root_ = pq.top().first;
}

template <typename ValueType>
inline void HuffmanEncoder<ValueType>::build_huffman_table() {
  using DfsFunc = std::function<void(TreeNode*, std::string)>;
  DfsFunc dfs_build = [&](TreeNode* root, std::string codec) {
    if (!root) {
      return;
    }

    // leaf node
    if (root->left == nullptr && root->right == nullptr) {
      if (root->pval) {
        huffman_table_[root->pval] = codec;
      }
    }

    if (root->left) {
      codec.push_back('0');
      dfs_build(root->left, codec);
      codec.pop_back();
    }
    if (root->right) {
      codec.push_back('1');
      dfs_build(root->right, codec);
    }
  };
  dfs_build(root_, "");
}

class HuffmanFileEncoder {
public:
  HuffmanFileEncoder(const std::string& input_path,
                     const std::string& output_path) :
      reader_(input_path),
      writer_(output_path + ".librecat") {}

  DISABLE_COPY(HuffmanFileEncoder);

  bool encode() {
    if (!reader_.valid() || !writer_.valid()) {
      return false;
    }

    write_header();

    const int BUF_SIZE = 32768;
    char buf[BUF_SIZE];
    int nread = 0;
    while ((nread = reader_.read(buf, BUF_SIZE)) > 0) {
      HuffmanEncoder<char> encoder(buf, nread);
      if (!encoder.encode()) {
        return false;
      }

      auto huff_table = encoder.get_huffman_table();
      {
        std::string buf;
        for (const auto& entry : huff_table) {
          buf.push_back('{');
          buf.push_back(*entry.first);
          buf.push_back(',');
          buf += entry.second;
          buf.push_back('}');
        }
        write_huffman_table(buf.c_str(), buf.size());
      }

      for (int i = 0; i < nread; ++i) {
        if (huff_table.count(buf + i)) {
          auto& codec = huff_table[buf + i];
          if (!writer_.write(codec.c_str(), codec.size())) {
            return false;
          }
        }
      }

      write_seperator();
    }

    write_footer();
    return true;
  }

private:
  void write_header() {
    std::string buf = "librecat\n";
    writer_.write(buf.c_str(), buf.size());
  }

  void write_footer() {
    std::string buf = "tacerbil";
    writer_.write(buf.c_str(), buf.size());
  }

  bool write_huffman_table(const void* buf, int size) {
    return writer_.write(buf, size) > 0;
  }

  void write_seperator() {
    std::string seperator = "\n====seperator====\n";
    writer_.write(seperator.c_str(), seperator.size());
  }

private:
  fileutils::FileReader reader_;
  fileutils::FileWriter writer_;
};
}  // namespace basic
