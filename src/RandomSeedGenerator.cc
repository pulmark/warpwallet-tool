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

#include <algorithm>
#include <array>
#include <cstring>
#include <map>

#include "RandomSeedGenerator.h"
#include "randutils.hpp"

namespace {
/// \brief PASSWD_CHARS - all allowed password characters: digits + alpabets
const std::array<unsigned char, 62> PASSWD_CHARS = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c',
    'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/// \brief RANGE - character ranges
const std::map<SeedChar, std::pair<unsigned int, unsigned int>> RANGE = {
    {SeedChar::kUndef, {0, 0}},         {SeedChar::kAll, {0, 61}},
    {SeedChar::kDigit, {0, 9}},         {SeedChar::kLetter, {10, 61}},
    {SeedChar::kSmallLetter, {10, 35}}, {SeedChar::kCapitalLetter, {36, 61}}};
}

RandomSeedGenerator::RandomSeedGenerator(SeedChar chars)
    : chars_(chars), dict_(SeedDictionary::kUndef) {
  charRange_.first = RANGE.at(chars_).first;
  charRange_.second = RANGE.at(chars_).second;
}

RandomSeedGenerator::RandomSeedGenerator(SeedDictionary dict)
    : chars_(SeedChar::kUndef), dict_(dict) {}

void RandomSeedGenerator::init() {
  /*
    // generate long enough init seed sequence so that no special warm-up time
    // is required for the M-T algorithm engine.
    std::array<int, 624> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  */
  // use randutils advanced seed generator for initialization
  engine_ = std::make_unique<std::mt19937>(randutils::auto_seed_256{}.base());

  if (dict_ != SeedDictionary::kUndef) {
    /// \todo read allowed words from file
  }
}

bool RandomSeedGenerator::generatePassword(Password& pwd, size_t len) const {
  if (chars_ == SeedChar::kUndef)
    throw std::invalid_argument("RandomSeedGenerator:: undefined char set");
  if (len < 2)
    throw std::invalid_argument("RandomSeedGenerator:: password too short");

  // use uniform distribution in given character range
  std::uniform_int_distribution<unsigned int> dist(charRange_.first,
                                                   charRange_.second);

  // init and then fill password buffer with random chars
  pwd.resize(len);
  auto fill = [&](unsigned char& i) { i = PASSWD_CHARS[dist(*engine_)]; };
  std::for_each(pwd.begin(), pwd.end(), fill);

  return true;
}

bool RandomSeedGenerator::generatePassphrase(Passphrase& phrase,
                                             size_t cnt) const {
  if (dict_ == SeedDictionary::kUndef)
    throw std::invalid_argument("RandomSeedGenerator:: undefined dictionary");
  if (cnt < 2)
    throw std::invalid_argument("RandomSeedGenerator:: passphrase too short");

  // use uniform distribution in given word range
  std::uniform_int_distribution<uint32_t> dist(wordRange_.first,
                                               wordRange_.second);

  // init and then fill phrase with random words
  phrase.resize(cnt);
  auto fill = [&](std::vector<uint8_t>& w) { w = words_.at(dist(*engine_)); };
  std::for_each(phrase.begin(), phrase.end(), fill);

  return true;
}
