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
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <experimental/optional>
#include <sstream>
#include <string>

#include "CoinKeyPair.h"
#include "RandomSeedGenerator.h"

/// crack-address -p <password length> <salt> <address>
const std::string OPER_ATTACH("crack-address");

/// generate-key -p <password> <salt>
const std::string OPER_GENERATE_COIN("generate-key");

/// generate-key-random -p <password length> <salt> <key count>
const std::string OPER_GENERATE_COIN_RANDOM("generate-key-random");

/// generate-wallet-dts -p <password> <salt> <magic number> <key count>
const std::string OPER__GENERATE_WALLET_DTS(
    "generate-wallet-deterministic-simple");

/// generate-wallet-dts -p <password> <salt> <external key count> <internal key
/// count>
const std::string OPER__GENERATE_WALLET_HD(
    "generate-wallet-deterministic-bib32");

/// test -p <test identifier> <test vector file name>
const std::string OPER_TEST("test");

/// default operation, no parameters (generate keypair with fixed password:salt)
const std::string OPER_DEFAULT("default");

const std::string OPER_UNDEF("undef");

using UserArguments = std::vector<std::string>;

/// \class UserInterface
/// \brief Processes user input and displays command results.
///
class UserInterface {
 public:
  UserInterface(std::ostream& out);

  void reset();

  bool parse(int argc, char** argv);

  void show(const std::ostringstream& result);

  UserArguments arguments_;

  /// coin network identifier, default Bitcoin.
  CoinId cid_;

  /// operation code
  std::string oper_;

  /// password
  Password pwd_;

  /// salt
  Password salt_;

  /// random key generation parameters
  struct Random {
    operator bool() const {
      return (pwd_len_ >= 2 && pwd_len_ < 65535 && keys_ >= 1 && keys_ < 1000);
    }
    uint16_t pwd_len_;
    unsigned int keys_;
  };
  std::experimental::optional<Random> random_;

  /// attach parameters
  struct Attach {
    operator bool() const {
      return (pwd_len_ >= 2 && pwd_len_ < 65535 && address_.size() != 0);
    }
    uint16_t pwd_len_;
    ByteVect address_;
  };
  std::experimental::optional<Attach> attach_;

  /// simple deterministic wallet parameters
  struct WalletDTS {
    operator bool() const { return (keys_ >= 1 && keys_ < 1000); }
    unsigned int magic_;
    unsigned int keys_;
    bool is_watch_only_;
    std::string root_;
  };
  std::experimental::optional<WalletDTS> dts_wallet_;

  /// hierarchical deterministic wallet parameters
  struct WalletHD {
    operator bool() const {
      return (external_keys_ >= 1 && external_keys_ < 1000 &&
              internal_keys_ < 1000);
    }
    bool is_watch_only_;
    unsigned int external_keys_;
    unsigned int internal_keys_;
  };
  std::experimental::optional<WalletHD> hd_wallet;

  /// file name containing test vectors
  std::experimental::optional<std::string> fnTest_;

  /// file name containing words for passphrase dictionary
  std::experimental::optional<std::string> fnDict_;

  /// output stream to save or display
  std::ostream& out_;
};

#endif  // USERINTERFACE_H
