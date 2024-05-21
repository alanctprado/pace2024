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

namespace banana {
namespace library {

/**
 * Polimorphic Fraction Type.
 *
 * Handles arithmetic operations on fractions.
 *
 * The type T should have arithmetic operations implemented, and
 * a function `gcd` to find the greatest common divisor between
 * two numbers.
 *
 * Be careful with overflow! the product of two values shouldn't
 * exceed the limit of T.
 *
 * In most cases, T=int or T=`long long` should suffice.
 */
template <class T> class Fraction
{
protected:
  T num, den;

public:
  Fraction(const T &_num=0, const T &_den=1);
};

template <class T>
Fraction<T>::Fraction(const T &_num, const T &_den) : num(_num), den(_den)
{
  assert(den != 0);
  if (den < 0)
  {
    num *= -1;
    den *= -1;
  }
  T g = gcd(num, den);
  num /= g;
  den /= g;
}

template <class T>
inline bool operator <(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return (lhs.num * rhs.den < lhs.den * rhs.num);
}

template <class T>
inline Fraction<T> operator +(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num * rhs.den + lhs.den * rhs.num, lhs.den * rhs.den};
}

template <class T>
inline Fraction<T> operator -(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num * rhs.den - lhs.den * rhs.num, lhs.den * rhs.den};
}

template <class T>
inline Fraction<T> operator *(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {lhs.num * rhs.num, lhs.den * rhs.den};
}

template <class T>
inline Fraction<T> operator /(const Fraction<T> &lhs, const Fraction<T> &rhs)
{
  return {rhs.num * rhs.den, lhs.den * rhs.num};
}

} // namespace library
} // namespace banana

#endif // __PACE2024__FRACTION_HPP
