/******************************************************************************
 * Top contributors (to current version):
 *   Kaio Vieira
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Basic Fenwick Tree Implementation
 */

#ifndef __PACE2024__FENWICK_HPP
#define __PACE2024__FENWICK_HPP

#include <vector>

namespace banana {
namespace library {

/**
 * Polimorphic Fenwick Tree.
 * 
 * Data structure to perform element-wise updates in an array, 
 * and range queries. The operations `update`, `suffixQuery` and
 * `rangeQuery` work in O(log n).
 *
 * The type T should implement an associative binary `+` operation.
 *
 * In most cases, T=int or T=`long long` should suffice.
 */
template <class T>
class FenwickTree
{
 protected:
  int m_treeSize;
  std::vector<T> m_tree;

 public:
  /** Empty tree constructor */
  FenwickTree(int n);

  /** Constructs a tree from a initial array (in O(n log n)) */
  FenwickTree(const std::vector<T>& array);

  /** Adds 'value' to the i-th element (0-based) */
  void update(int i, const T& value);

  /** Get the sum of all elements with index >= l */
  T suffixQuery(int l) const;

  /** Get the sum of all elements in the range [l, r] */
  T rangeQuery(int l, int r) const;
};

template <class T>
FenwickTree<T>::FenwickTree(int n) : m_treeSize(n), m_tree(n + 1) {}

template <class T>
FenwickTree<T>::FenwickTree(const std::vector<T>& array) 
    : m_treeSize(array.size()), 
      m_tree(m_treeSize + 1) 
{
  for (int i = 0; i < (int)array.size(); i++)
  {
    for (int j = i + 1; j > 0; j -= j & (-j)) 
    {
      m_tree[j] = m_tree[j] + array[i];
    }
  }
}

template <class T>
void FenwickTree<T>::update(int i, const T& value)
{
  for (i++; i > 0; i -= i & (-i)) { m_tree[i] = m_tree[i] + value; }
}

template <class T>
T FenwickTree<T>::suffixQuery(int l) const
{
  T answer = T();
  for (int i = l + 1; i <= m_treeSize; i += i & (-i))
  {
    answer = answer + m_tree[i];
  }
  return answer;
}

template <class T>
T FenwickTree<T>::rangeQuery(int l, int r) const
{
  return suffixQuery(l) - suffixQuery(r + 1);
}

} // namespace library
} // namespace banana

#endif  // __PACE2024__FENWICK_HPP
