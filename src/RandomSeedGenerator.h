/*
** Copyright (c) 2017 Markku Pulkkinen. All rights reserved.
** Contact: markku.j.pulkkinen@gmail.com
**
** This file is part of warpwallet-tool software distribution.
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

#ifndef RANDOMSEEDGENERATOR_H
#define RANDOMSEEDGENERATOR_H

#include <assert.h>
#include <cstdint>

#include <iterator>
#include <map>
#include <memory>
#include <random>

enum class SeedChar {
  kUndef = 0,
  kAll,
  kDigit,
  kLetter,
  kSmallLetter,
  kCapitalLetter
};

enum class SeedDictionary { kUndef = 0, kFinnish, kEnglish };

using Password = std::vector<uint8_t>;
using Passphrase = std::vector<Password>;
using RangeMinMax = std::pair<uint32_t, uint32_t>;
using Dictionary = std::map<uint32_t, std::vector<uint8_t> >;

/// \class RandomSeedGenerator
/// \brief Generates random seed containing random chars or words.
///
class RandomSeedGenerator {
 public:
  RandomSeedGenerator(SeedChar chars = SeedChar::kAll);
  RandomSeedGenerator(SeedDictionary dict = SeedDictionary::kFinnish);

  virtual ~RandomSeedGenerator() {}

  void init();

  /// \brief generates password of given length.
  bool generatePassword(Password& pass, size_t len) const;

  /// \brief generates passphrase containing given number of words.
  bool generatePassphrase(Passphrase& phrase, size_t cnt) const;

 private:
  /// random number engine based on Mersenne Twister algorithm
  std::unique_ptr<std::mt19937> engine_;

  SeedChar chars_;
  RangeMinMax charRange_;

  SeedDictionary dict_;
  RangeMinMax wordRange_;
  Dictionary words_;
};

#endif  // RANDOMSEEDGENERATOR_H
