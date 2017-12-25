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
#include <fstream>
#include <iostream>
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

/// country codes for supported language dictionaries
const std::vector<std::string> PASSPHRASE_LANG = {
    "en", "us", "gb", "au", "ca", "ie",   "io",   "it",
    "fr", "es", "fi", "jp", "kr", "cn-s", "cn-t", "xx"};

/// file names for language dictionaries (BIP-0039)
const std::string DICT_ENGLISH = "english.txt";
const std::string DICT_FRENCH = "french.txt";
const std::string DICT_SPANISH = "spanish.txt";
const std::string DICT_ITALIAN = "italian.txt";
const std::string DICT_FINNISH = "finnish.txt";
const std::string DICT_KOREAN = "korean.txt";
const std::string DICT_JAPAN = "japanese.txt";
const std::string DICT_CHINESE_SIMPLE = "chinese_simplified.txt";
const std::string DICT_CHINESE_TRAD = "chinese_traditional.txt";
const std::string DICT_WORD = "word.txt";
const std::string DICT_DEFAULT = DICT_WORD;

/// mapping of country code into dictionary file name
const std::map<std::string, std::string> PASSPHRASE_DICT = {
    {"en", DICT_ENGLISH},        {"us", DICT_ENGLISH},
    {"gb", DICT_ENGLISH},        {"au", DICT_ENGLISH},
    {"ca", DICT_ENGLISH},        {"ie", DICT_ENGLISH},
    {"io", DICT_ENGLISH},        {"es", DICT_SPANISH},
    {"it", DICT_ITALIAN},        {"fr", DICT_FRENCH},
    {"fi", DICT_FINNISH},        {"kr", DICT_KOREAN},
    {"jp", DICT_JAPAN},          {"cn-s", DICT_CHINESE_SIMPLE},
    {"cn-t", DICT_CHINESE_TRAD}, {"xx", DICT_WORD},
};
}

RandomSeedGenerator::RandomSeedGenerator(SeedChar chars)
    : chars_(chars), dict_("") {
  charRange_.first = RANGE.at(chars_).first;
  charRange_.second = RANGE.at(chars_).second;
}

RandomSeedGenerator::RandomSeedGenerator(const SeedDictionary& dict)
    : chars_(SeedChar::kUndef), dict_(dict) {}

RandomSeedGenerator::RandomSeedGenerator(
    const std::vector<unsigned char>& custom)
    : chars_(SeedChar::kCustom),
      custom_(custom.begin(), custom.end()),
      dict_("") {
  charRange_.first = 0;
  charRange_.second = custom_.size() - 1;
  if (custom_.size() < 2)
    throw std::invalid_argument(
        "RandomSeedGenerator:: custom charset too short");
}

bool RandomSeedGenerator::isSupportedLanguage(const std::string& code) {
  auto it = std::find(PASSPHRASE_LANG.begin(), PASSPHRASE_LANG.end(), code);
  return (it != PASSPHRASE_LANG.end());
}

long double RandomSeedGenerator::combinations(uint32_t length) const {
  if (chars_ != SeedChar::kUndef) {
    unsigned int cnt = (charRange_.second - charRange_.first) + 1;
    return std::pow(cnt, length);
  }
  if (!dict_.empty()) {
    unsigned int cnt = (wordRange_.second - wordRange_.first) + 1;
    return std::pow(cnt, length);
  }
  return 0;
}

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
  randutils::auto_seed_256 seeder;
  engine_ = std::make_unique<std::mt19937>(seeder);

  // read engine's state from file if exists
  std::ifstream fengine(PRNG_FNAME, std::ifstream::in);
  if (fengine.good()) fengine >> *engine_;
  fengine.close();

  // read password's distribution from file if exists
  if (chars_ != SeedChar::kUndef) {
    charDistribution_ =
        std::make_unique<std::uniform_int_distribution<unsigned int>>(
            charRange_.first, charRange_.second);

    std::ifstream fdistribution(DIST_CHAR_FNAME, std::ifstream::in);
    if (fdistribution.good()) fdistribution >> *charDistribution_;
    fdistribution.close();
  }

  // init dictionary and it's distribution utilized for passphrase generation
  if (!dict_.empty()) {
    initDictionary();

    wordDistribution_ =
        std::make_unique<std::uniform_int_distribution<unsigned int>>(
            wordRange_.first, wordRange_.second);

    std::ifstream fdistribution(DIST_WORD_FNAME, std::ifstream::in);
    if (fdistribution.good()) fdistribution >> *wordDistribution_;
    fdistribution.close();
  }
}

void RandomSeedGenerator::save() {
  std::ofstream ofs(PRNG_FNAME);
  ofs << *engine_;
  ofs.close();

  if (chars_ != SeedChar::kUndef) {
    std::ofstream ofs(DIST_CHAR_FNAME);
    ofs << *charDistribution_;
    ofs.close();
  }

  if (!dict_.empty()) {
    std::ofstream ofs(DIST_WORD_FNAME);
    ofs << *wordDistribution_;
    ofs.close();
  }
}

bool RandomSeedGenerator::generatePassword(Password& pwd, size_t len,
                                           const Password& mask) const {
  if (chars_ == SeedChar::kUndef)
    throw std::invalid_argument("RandomSeedGenerator:: undefined char set");
  if (len < 2)
    throw std::invalid_argument("RandomSeedGenerator:: password too short");
  if (mask.size() > 0 && mask.size() != len)
    throw std::invalid_argument("RandomSeedGenerator:: invalid mask size");

  // init and then fill password buffer with random chars
  pwd.resize(len);
  if (mask.size() == 0) {
    if (chars_ != SeedChar::kCustom) {
      auto fill = [&](unsigned char& i) {
        i = PASSWD_CHARS[(*charDistribution_)(*engine_)];
      };
      std::for_each(pwd.begin(), pwd.end(), fill);
    } else {
      auto fill = [&](unsigned char& i) {
        i = custom_.at((*charDistribution_)(*engine_));
      };
      std::for_each(pwd.begin(), pwd.end(), fill);
    }
  } else {
    // init pwd with mask and then replace char '* | # | < | >' with random char
    pwd = mask;
    if (chars_ != SeedChar::kCustom) {
      auto fill = [&](unsigned char& i) {
        if (i == '*') {
          i = PASSWD_CHARS[(*charDistribution_)(*engine_)];
        } else if (i == '#') {
          // loop until digit
          do {
            uint8_t k = PASSWD_CHARS[(*charDistribution_)(*engine_)];
            if (!std::isdigit(k)) continue;
            i = k;
            break;
          } while (1);
        } else if (i == '<' || i == '>') {
          // loop until lower or upper alpha
          do {
            uint8_t k = PASSWD_CHARS[(*charDistribution_)(*engine_)];
            if (!std::isalpha(k)) continue;
            if (i == '<' && std::islower(k)) {
              i = k;
              break;
            }
            if (i == '>' && std::isupper(k)) {
              i = k;
              break;
            }
          } while (1);
        }
      };
      std::for_each(pwd.begin(), pwd.end(), fill);
    } else {
      auto fill = [&](unsigned char& i) {
        if (i == '*') {
          i = custom_[(*charDistribution_)(*engine_)];
        } else if (i == '#') {
          // loop until digit
          do {
            uint8_t k = custom_[(*charDistribution_)(*engine_)];
            if (!std::isdigit(k)) continue;
            i = k;
            break;
          } while (1);
        } else if (i == '<' || i == '>') {
          // loop until lower or upper alpha
          do {
            uint8_t k = custom_[(*charDistribution_)(*engine_)];
            if (!std::isalpha(k)) continue;
            if (i == '<' && std::islower(k)) {
              i = k;
              break;
            }
            if (i == '>' && std::isupper(k)) {
              i = k;
              break;
            }
          } while (1);
        }
      };
      std::for_each(pwd.begin(), pwd.end(), fill);
    }
  }
  return true;
}

bool RandomSeedGenerator::generatePassphrase(Passphrase& phrase, size_t cnt,
                                             const uint8_t& delim) const {
  if (dict_.empty())
    throw std::invalid_argument("RandomSeedGenerator:: dictionary undefined");
  if (cnt < 2)
    throw std::invalid_argument("RandomSeedGenerator:: passphrase too short");

  // init and then fill phrase with random words
  phrase.resize(cnt);
  for (auto& w : phrase) {
    // counter to avoid infinite loop when checking duplicates
    // if PRNG generates same number 100x times in a row, there
    // is something seriously wrong with initialization seed
    unsigned int retries{100};
    unsigned int i{0};
    Password word;
    do {
      word = words_.at((*wordDistribution_)(*engine_));
      // no duplicate words allowed, retry if found
      auto last = std::find(phrase.begin(), phrase.end(), word);
      if (last == phrase.end()) break;
    } while (++i < retries);
    w.reserve(word.size());
    w = word;
  };

  // add delimiter
  uint8_t lim = delim;
  auto add = [&](std::vector<uint8_t>& w) {
    w.reserve(w.size() + 1);
    w.push_back(lim);
  };
  std::for_each(phrase.begin(), phrase.end() - 1, add);

  return true;
}

void RandomSeedGenerator::initDictionary() {
  if (dict_.empty())
    throw std::invalid_argument("RandomSeedGenerator:: dictionary undefined");

  std::string fn(dict_);

  // try to map dict into file name for specific language (BIP-0039)
  auto it = PASSPHRASE_DICT.find(fn);
  if (it != PASSPHRASE_DICT.end()) {
    fn = PASSPHRASE_DICT.at(dict_);
  }

  // try to open dictionary file
  std::ifstream ifs(fn, std::ifstream::in);
  if (!ifs.is_open()) {
    dict_.clear();
    throw std::invalid_argument(
        "RandomSeedGenerator:: invalid dictionary file name");
  }

  // try to init dictionary words vector by reading the file
  try {
    std::string line;
    while (std::getline(ifs, line)) {
      Password word{line.begin(), line.end()};
      words_.push_back(word);
    }
    ifs.close();
    wordRange_ = {0, words_.size() - 1};
  } catch (...) {
    dict_.clear();
    ifs.close();
    throw std::invalid_argument(
        "RandomSeedGenerator:: error processing dictionary file");
  }
}

/*
    ideas for PasswordValidator class

    RuleId id_;
    Password reference_;

    std::map<Password, size_t> forbidden_;

    RangeMinMax digits_;
    RangeMinMax alphas_;

    void setRule(const RuleId id, const Password& pwd) {
      id_ = id;
      reference_.resize(pwd.size());
      reference_ = pwd;
    }

    void setDigitRange(size_t min, size_t max) {
      digits_.first = min;
      digits_.second = max;
    }

    void setAlphaRange(size_t min, size_t max) {
      alphas_.first = min;
      alphas_.second = max;
    }

    void addIllegalPassword(const Password& pwd, unsigned int compareChars) {
      forbidden_.insert({pwd, compareChars});
    }


  // following generation some sanity checks for new password

  // check forbidden passwords if any
  for (const auto& m : forbidden_) {
    size_t cnt = m.second;
    Password illegal = m.first;

    // determine how many chars to compare and then do comparison
    size_t chars = std::min(pwd.size(), cnt);
    Password s1(pwd.begin(), pwd.begin() + chars);
    Password s2(illegal.begin(), illegal.begin() + chars);
    if (s1 == s2) return false;
  }

  // check digits count if set
  if (digits_.first || digits_.second) {
    auto n = std::count_if(pwd.begin(), pwd.end(),
                           [](const uint8_t& i) { return std::isdigit(i); });
    if (n < digits_.first || n > digits_.second) return false;
  }

  // check alphas count if set
  if (alphas_.first || alphas_.second) {
    auto n = std::count_if(pwd.begin(), pwd.end(),
                           [](const uint8_t& i) { return std::isalpha(i); });
    if (n < digits_.first || n > digits_.second) return false;
  }

  if (id_ == RuleId::kUndef) return true;

  if (id_ == RuleId::kAsciiSumEqual) {
    // the sum of char ascii table values must be same
    int sum_ref{0}, sum_pwd{0};
    for (const auto& a : reference_) sum_ref = int(a);
    for (const auto& a : pwd) sum_pwd = int(a);
    return (sum_ref == sum_pwd);
  }

  if (id_ == RuleId::kInternalPatternEqual) {
    // same pattern in the password compared to reference
    // example 'b234FEzz'
    // @todo parse reference to find general internal structure
    //  Simple rule-set:
    //        1. is char digit | upper | lower
    //        2. find possible relation of concecutive nums & chars
    //           2.1 numbers, chars in ascending, descending order
    //           2.2 same adjacent numbers, chars
    //           2.3 pwd begin, end char is same or equal position related
    //               to ascii chart relative position from start, end

    // 1. pwd[0]: first letter is lower alphabet
    if (!(std::isalpha(pwd[0]) && std::islower(pwd[0]))) return false;

    // 2. pwd[1,3]: numbers in rising order
    if (!(std::isdigit(pwd[1]) && std::isdigit(pwd[2]) && std::isdigit(pwd[3])))
      return false;
    if (!(int(pwd[1]) + 1 == int(pwd[2]) && int(pwd[1]) + 2 == int(pwd[3])))
      return false;

    // 3. pwd[4,5]: upper alphabets next to each other ie. FE
    if (!(std::isalpha(pwd[4]) && std::isupper(pwd[4]))) return false;
    if (!(std::isalpha(pwd[5]) && std::isupper(pwd[5]))) return false;
    if (pwd[4] - 1 != pwd[5]) return false;

    // 4. pwd[6,7]: small alphabets, must be same
    if (!(std::isalpha(pwd[6]) && std::islower(pwd[6]))) return false;
    if (!(std::isalpha(pwd[7]) && std::islower(pwd[7]))) return false;
    if (pwd[6] != pwd[7]) return false;
  }
  return true;
}
*/
