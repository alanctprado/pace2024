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
 * Fraction Type Implementation
 */

#ifndef __PACE2024__FRACTION_HPP
#define __PACE2024__FRACTION_HPP

#include <cassert>
#include <numeric>

namespace banana {
namespace library {

/**
 * Polimorphic Fraction Type.
 *
 * Handles arithmetic operations on fractions.
 *
 * The type T should have arithmetic operations implemented,
 * and the std::gcd function.
 *
 * Be careful with overflow! the product of two values shouldn't
 * exceed the limit of T.
 *
 * In most cases, T=int or T=`long long` should suffice.
 */
template <class T> class Fraction
{
protected:
  T _num, _den;

public:
  Fraction(const T &_num=0, const T &_den=1);

  const T& num() const;
  const T& den() const;
};

template <class T>
Fraction<T>::Fraction(const T &n, const T &d) : _num(n), _den(d)
{
  assert(_den != 0);
  if (_den < 0)
  {
    _num *= -1;
    _den *= -1;
  }
  T g = std::gcd(_num, _den);
  _num /= g;
  _den /= g;
}

template <class T>
const T& Fraction<T>::num() const {
    return _num;
}

template <class T>
const T& Fraction<T>::den() const {
    return _den;
}

template <class T>
inline bool operator <(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return (lhs.num() * rhs.den() < lhs.den() * rhs.num());
}

template <class T>
inline bool operator ==(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return (lhs.num() == rhs.num() && lhs.den() == rhs.den());
}

template <class T>
inline Fraction<T> operator +(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num() * rhs.den() + lhs.den() * rhs.num(), lhs.den() * rhs.den()};
}

template <class T>
inline Fraction<T> operator -(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num() * rhs.den() - lhs.den() * rhs.num(), lhs.den() * rhs.den()};
}

template <class T>
inline Fraction<T> operator *(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num() * rhs.num(), lhs.den() * rhs.den()};
}

template <class T>
inline Fraction<T> operator /(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {rhs.num() * rhs.den(), lhs.den() * rhs.num()};
}

} // namespace library
} // namespace banana

#endif // __PACE2024__FRACTION_HPP
