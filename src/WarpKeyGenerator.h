/*
** Copyright (c) 2017 Markku Pulkkinen. All rights reserved.
** Contact: markku.j.pulkkinen@gmail.com
**
** This file is part of libwarputil software distribution.
**
** This software is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This software is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WARPKEYGENERATOR_H
#define WARPKEYGENERATOR_H

#include <array>
#include <cstdint>
#include <vector>

#include "CoinKeyPair.h"

/// \brief The WarpKeyGenerator class
class WarpKeyGenerator {
 public:
  WarpKeyGenerator() {}
  virtual ~WarpKeyGenerator() {}

  void init();

  int generate(const ByteVect& pwd, const ByteVect& salt, SecretKey& out);
};

#endif  // WARPKEYGENERATOR_H
