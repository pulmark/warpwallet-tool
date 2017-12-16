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
#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <bitset>
#include <experimental/optional>
#include <sstream>
#include <string>

#include "CoinKeyPair.h"
#include "RandomSeedGenerator.h"
#include "UserInterface.h"
#include "WarpKeyGenerator.h"
#include "json.hpp"

using json = nlohmann::json;

enum OptionsOutputEnum : std::uint8_t {
  kKeysAddress = 1,
  kKeysPrivKey,
  kKeysPublicKey,
  kRootKey
};

using OptionsOutput = std::bitset<8>;
using PassWordSaltKeyMap = std::map<std::pair<Password, Password>, CoinKeyPair>;
using KeyVect = std::vector<CoinKeyPair>;

///
/// \brief A class for executing user commands and parameters
///
class CommandInterpreter {
 public:
  explicit CommandInterpreter(UserInterface& ui) : ui_(ui) {}

  void execute();

  std::ostringstream& result() { return result_; }

 private:
  void doDefault();
  void doAttach();
  void doGenerateCoin();
  void doGenerateCoinRandom();
  void doGenerateWalletDTS();
  void doGenerateWalletHD();
  void doTest();

  void initJSON();
  void flushJSON();

  void addJSON(const UserInterface& ui);
  void addJSON(const UserInterface::WalletDTS& wallet);
  void addJSON(const UserInterface::WalletHD& wallet);
  void addJSON(const UserInterface& ui, const UserInterface::Attach& attach,
               const ByteVect& pwd);
  void addJSON(const CoinKeyPair& coin);
  void addJSON(const KeyVect& coins,
               const OptionsOutput& options = OptionsOutput(0xff));

  void addJSON(const PassWordSaltKeyMap& coins);
  void addJSON(const std::string& name, uint64_t combination, uint64_t cnt,
               uint64_t ms);

  /// reference into user I/O parameters
  UserInterface& ui_;

  /// stream for JSON formatted result after command is processed
  std::ostringstream result_;

  /// JSON object for building command result
  json out_;
};

#endif  // COMMANDINTERPRETER_H
