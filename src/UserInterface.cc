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

#include <chrono>

#include "CLI.hpp"
#include "UserInterface.h"

using namespace CLI;

namespace {

const int DEFAULT_RANDOM_PWD_LEN{8};
const Password DEFAULT_PWD{'M', 'a', 'k', 'e', ' ', 'W', 'A',
                           'R', 'P', ' ', 'G', 'r', 'e', 'a',
                           't', ' ', 'A', 'g', 'a', 'i', 'n'};
const Password DEFAULT_SALT{'l', 'e', 't', '@', 'm', 'e', '.', 'i', 'n'};
const CoinId DEFAULT_COIN{CoinId::kBitCoin};

enum CoinEnum : std::uint32_t {
  BitCoin = 1,
  BitCoinTest = 2,
  LiteCoin = 3,
  LiteCoinTest = 4
};

enum CommandEnum : std::uint32_t {
  GenerateKeys = 1,
  GenerateKeysRandom = 2,
  AttachAddress = 3,
  GenerateWalletSD = 4,
  GenerateWalletBIP32 = 5,
  Test = 6
};
}

UserInterface::UserInterface(std::ostream& out)
    : arguments_(0),
      cid_(DEFAULT_COIN),
      oper_(OPER_DEFAULT),
      pwd_(DEFAULT_PWD),
      salt_(DEFAULT_SALT),
      out_(out) {}

void UserInterface::reset() {
  pwd_ = DEFAULT_PWD;
  salt_ = DEFAULT_SALT;
  oper_ = OPER_DEFAULT;
}

void UserInterface::show(const std::ostringstream& result) {
  out_ << result.str();
}

/// \todo parse() method way too long, refactor it
bool UserInterface::parse(int argc, char** argv) {
  if (argc <= 1) return true;
  reset();

  // init arguments vector, needed later
  arguments_ = UserArguments(argv + 1, argv + argc);

  // create CLI parser
  CLI::App app("WarpWallet Utility Tool");

  // init network option
  CoinEnum coin;
  CLI::Option* opt_coin = app.add_set(
      "-n,--network", coin, {BitCoin, BitCoinTest, LiteCoin, LiteCoinTest});
  opt_coin->set_type_name(
      " enum/network in\n"
      "\t{BitCoin = 1,\n"
      "\t BitCoinTest = 2,\n"
      "\t LiteCoin = 3,\n"
      "\t LiteCoinTest = 4} ");
  opt_coin->set_default_val(" 1");

  // init command option
  CommandEnum cmd;
  CLI::Option* opt_cmd = app.add_set(
      "-c,--command", cmd, {GenerateKeys, GenerateKeysRandom, AttachAddress,
                            GenerateWalletSD, GenerateWalletBIP32, Test});
  opt_cmd->set_type_name(
      " enum/command in\n"
      "\t{GenerateKeys = 1,\n"
      "\t GenerateKeysRandom = 2,\n"
      "\t Attach = 3,\n"
      "\t GenerateWalletSD = 4,\n"
      "\t GenerateWalletBIP32 = 5,\n"
      "\t Test = 6} ");
  opt_cmd->set_default_val("1");

  // init command parameters option
  std::vector<std::string> params{"password", "let@me.in"};
  CLI::Option* opt_params = app.add_option("-p,--params", params);
  opt_params->set_type_name(
      "command parameters:\n"
      "\t1 = {password salt}\n"
      "\t2 = {password-length salt key-count}\n"
      "\t3 = {password-len salt coin-address}\n"
      "\t4 = {password salt magic-number key-count is-watch-only}\n"
      "\t5 = {password salt external-key-count internal-key-count "
      "is-watch-only}\n"
      "\t6 = {test-number test-vector-file-name} ");
  opt_params->set_default_val(" 'Make Warp Great Again' let@me.in");

  // run parser
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    app.exit(e);
  }

  bool has_network(opt_coin->count() > 0);
  bool has_command(opt_cmd->count() > 0);
  bool has_params(opt_params->count() > 0);

  // process parser results
  do {
    // help -> exit
    if (arguments_.at(0) == "-h" || arguments_.at(0) == "--help") {
      oper_ = OPER_UNDEF;
      break;
    }

    // no network -> exit
    if (!has_network) {
      oper_ = OPER_UNDEF;
      break;
    }

    // init network
    cid_ = CoinId(coin);

    // no command, select default operation, parameters -> exit
    if (!has_command) {
      oper_ = OPER_DEFAULT;
      break;
    }

    // process command
    if (cmd == GenerateKeys)
      oper_ = OPER_GENERATE_COIN;
    else if (cmd == GenerateKeysRandom)
      oper_ = OPER_GENERATE_COIN_RANDOM;
    else if (cmd == AttachAddress)
      oper_ = OPER_ATTACH;
    else if (cmd == GenerateWalletSD)
      oper_ = OPER__GENERATE_WALLET_DTS;
    else if (cmd == GenerateWalletBIP32)
      oper_ = OPER__GENERATE_WALLET_HD;
    else if (cmd == Test)
      oper_ = OPER_TEST;
    else {
      // unknown command ->  exit
      oper_ = OPER_UNDEF;
      break;
    }

    /* Process parameters for commands:
    GenerateKeys = 1,
    GenerateKeysRandom = 2,
    AttachAddress = 3,
    GenerateWalletSD = 4,
    GenerateWalletBIP32 = 5,
    Test = 6
    */
    switch (cmd) {
      default:
        oper_ = OPER_UNDEF;
        break;

      case GenerateKeys:
        // {password salt}
        if (!has_params) {
          std::stringstream ss;
          ss << oper_ << " parameters {password salt} missing";
          throw std::invalid_argument(ss.str());
        }
        try {
          pwd_.clear();
          pwd_.resize(params.at(0).size());
          std::copy(params.at(0).begin(), params.at(0).end(), pwd_.begin());
          salt_.clear();
          if (params.size() > 1) {
            salt_.resize(params.at(1).size());
            std::copy(params.at(1).begin(), params.at(1).end(), salt_.begin());
          }
        } catch (std::exception& e) {
          std::stringstream ss;
          ss << oper_ << " invalid parameter set {password salt}";
          throw std::invalid_argument(ss.str());
        }
        break;
      case GenerateKeysRandom:
        // {password-length salt keys-count}
        if (!has_params) {
          std::stringstream ss;
          ss << oper_
             << " parameters {password-length salt keys-count} missing";
          throw std::invalid_argument(ss.str());
        }
        try {
          pwd_.clear();
          UserInterface::Random temp;
          temp.pwd_len_ = std::stoi(params.at(0));
          salt_.resize(params.at(1).size());
          std::copy(params.at(1).begin(), params.at(1).end(), salt_.begin());
          temp.keys_ = std::stoi(params.at(2));
          random_ = temp;
        } catch (std::exception& e) {
          std::stringstream ss;
          ss << oper_
             << " invalid parameter set {password-length salt keys-count}";
          throw std::invalid_argument(ss.str());
        }
        break;
      case AttachAddress:
        // {password-length salt coin-adress}
        if (!has_params) {
          std::stringstream ss;
          ss << oper_
             << " parameters {password-length salt coin-address} missing";
          throw std::invalid_argument(ss.str());
        }
        try {
          UserInterface::Attach temp;
          temp.pwd_len_ = std::stoi(params.at(0));
          salt_.resize(params.at(1).size());
          std::copy(params.at(1).begin(), params.at(1).end(), salt_.begin());
          temp.address_.resize(params.at(2).size());
          std::copy(params.at(2).begin(), params.at(2).end(),
                    temp.address_.begin());
          attach_ = temp;
        } catch (std::exception& e) {
          std::stringstream ss;
          ss << oper_
             << " invalid parameter set {password-length salt coin-address}";
          throw std::invalid_argument(ss.str());
        }
        break;
      case GenerateWalletSD:
        // {password salt magic-number keys}
        if (!has_params) {
          std::stringstream ss;
          ss << oper_ << " parameters {password salt magic-number keys "
                         "is-watch-only} missing";
          throw std::invalid_argument(ss.str());
        }
        try {
          pwd_.resize(params.at(0).size());
          std::copy(params.at(0).begin(), params.at(0).end(), pwd_.begin());
          salt_.resize(params.at(1).size());
          std::copy(params.at(1).begin(), params.at(1).end(), salt_.begin());
          UserInterface::WalletDTS temp;
          temp.magic_ = std::stoi(params.at(2));
          temp.keys_ = std::stoi(params.at(3));
          temp.is_watch_only_ = (std::stoi(params.at(4)) == 1);
          dts_wallet_ = temp;
        } catch (std::exception& e) {
          std::stringstream ss;
          ss << oper_ << " invalid parameter set {password salt magic-number "
                         "keys is-watch-only}";
          throw std::invalid_argument(ss.str());
        }
        break;
      case GenerateWalletBIP32:
        // {password salt external-keys internal-keys watch-only}
        if (!has_params) {
          std::stringstream ss;
          ss << oper_ << " parameters {password salt ext-keys int-keys "
                         "is-watch-only} missing";
          throw std::invalid_argument(ss.str());
        }
        try {
          pwd_.resize(params.at(0).size());
          std::copy(params.at(0).begin(), params.at(0).end(), pwd_.begin());
          salt_.resize(params.at(1).size());
          std::copy(params.at(1).begin(), params.at(1).end(), salt_.begin());
          UserInterface::WalletHD temp;
          temp.external_keys_ = std::stoi(params.at(2));
          temp.internal_keys_ = std::stoi(params.at(3));
          temp.is_watch_only_ = (std::stoi(params.at(4)) == 1);
          hd_wallet = temp;
        } catch (std::exception& e) {
          std::stringstream ss;
          ss << oper_ << " invalid parameter set {password salt ext-keys "
                         "int-keys is-watch-only}";
          throw std::invalid_argument(ss.str());
        }
        break;
      case Test:
        break;
    }
  } while (false);

  return (oper_ != OPER_UNDEF);
}
