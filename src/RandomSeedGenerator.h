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

enum class RuleId {
  kUndef = 0,
  kAsciiSumEqual,
  kInternalStructureEqual,
};

enum class SeedChar {
  kUndef = 0,
  kAll,
  kDigit,
  kLetter,
  kSmallLetter,
  kCapitalLetter,
  kCustom
};

using SeedDictionary = std::string;
using Password = std::vector<uint8_t>;
using Passphrase = std::vector<Password>;
using RangeMinMax = std::pair<uint32_t, uint32_t>;
using Dictionary = std::vector<Password>;

/// \class RandomSeedGenerator
/// \brief Generates random seed containing random chars or words.
///
class RandomSeedGenerator {
 public:
  RandomSeedGenerator(SeedChar chars = SeedChar::kAll);
  RandomSeedGenerator(const SeedDictionary& dict);
  RandomSeedGenerator(const Password& custom);

  virtual ~RandomSeedGenerator() {}

  /// \brief initializes PRNG, distribution, charset and possible dictionary
  void init();

  /// \brief saves state of PRNG, distribution into file
  void save();

  /// \brief generates password of given length.
  bool generatePassword(Password& pass, size_t len,
                        const Password& mask = Password(0)) const;

  /// \brief generates passphrase containing given number of words.
  bool generatePassphrase(Passphrase& phrase, size_t cnt,
                          const uint8_t& delim = ' ') const;

  /// \brief returns number of possible combinations for given length
  long double combinations(uint32_t length) const;

  /// \brief checks if given dictionary language is supported
  static bool isSupportedLanguage(const std::string& code);

 private:
  void initDictionary();

  /// PRNG engine based on Mersenne Twister algorithm
  std::unique_ptr<std::mt19937> engine_;

  /// character set identifier
  SeedChar chars_;

  /// character range of charset
  RangeMinMax charRange_;

  /// custom character set
  std::vector<unsigned char> custom_;

  /// distribution utilized by PRNG
  std::unique_ptr<std::uniform_int_distribution<unsigned int> >
      charDistribution_;

  /// dictionary identifier, filename
  SeedDictionary dict_;

  /// word range
  RangeMinMax wordRange_;

  /// collection of words
  Dictionary words_;

  /// distribution utilized by PRNG
  std::unique_ptr<std::uniform_int_distribution<unsigned int> >
      wordDistribution_;

  /// constant fnames to save PRNG & distribution states
  static constexpr const char* PRNG_FNAME = "prng.dat";
  static constexpr const char* DIST_CHAR_FNAME = "dist_char.dat";
  static constexpr const char* DIST_WORD_FNAME = "dist_word.dat";
};

#endif  // RANDOMSEEDGENERATOR_H
