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

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "CoinKeyPair.h"
#include "CommandInterpreter.h"
#include "UserInterface.h"
#include "WarpKeyGenerator.h"

extern std::string byte2HexString(const uint8_t* data, int len);

namespace {
std::string ByteVect2String(const ByteVect& v) {
  std::ostringstream ss;
  if (!v.empty()) {
    std::copy(std::begin(v), std::end(v),
              std::ostream_iterator<unsigned char>(ss));
  }
  return ss.str();
}
}

void CommandInterpreter::execute() {
  if (ui_.oper_.compare(OPER_GENERATE_COIN) == 0) {
    doGenerateCoin();
  } else if (ui_.oper_.compare(OPER_GENERATE_COIN_RANDOM) == 0) {
    doGenerateCoinRandom();
  } else if (ui_.oper_.compare(OPER_ATTACH) == 0) {
    doAttach();
  } else if (ui_.oper_.compare(OPER__GENERATE_WALLET_DTS) == 0) {
    doGenerateWalletDTS();
  } else if (ui_.oper_.compare(OPER__GENERATE_WALLET_HD) == 0) {
    doGenerateWalletHD();
  } else if (ui_.oper_.compare(OPER_TEST) == 0) {
    doTest();
  } else if (ui_.oper_.compare(OPER_DEFAULT) == 0) {
    doDefault();
  } else
    throw std::domain_error("undefined command");
}

void CommandInterpreter::doDefault() {
  result_ << "Running default command <generate-key>" << std::endl;
  ui_.oper_ = OPER_GENERATE_COIN;
  doGenerateCoin();
}

void CommandInterpreter::doGenerateCoin() {
  if (ui_.pwd_.size() < 2 || ui_.pwd_.size() > 65535)
    throw std::out_of_range(
        "generate-coin: password length out of range [2,65535]");
  if (ui_.salt_.size() > 65535)
    throw std::out_of_range(
        "generate-coin: salt length out of range [0, 65535]");
  SecretKey priv;
  WarpKeyGenerator key_gen;
  key_gen.generate(ui_.pwd_, ui_.salt_, priv);
  CoinKeyPair coin(ui_.cid_);
  coin.create(priv.data(), priv.size());
  initJSON();
  addJSON(ui_);
  addJSON(coin);
  flushJSON();
}

void CommandInterpreter::doGenerateCoinRandom() {
  if (!ui_.random_ || !ui_.random_.value())
    throw std::invalid_argument(
        "generate-coin-random: invalid command parameters <password length | "
        "salt | keys count>");

  RandomSeedGenerator pwd_gen(SeedChar::kAll);
  pwd_gen.init();
  auto cnt = ui_.random_.value().keys_;
  auto pwd_len = ui_.random_.value().pwd_len_;
  std::map<std::pair<Password, Password>, CoinKeyPair> coins;
  while (cnt > 0) {
    Password pwd(pwd_len);
    pwd_gen.generatePassword(pwd, pwd.size());
    WarpKeyGenerator key_gen;
    SecretKey priv;
    key_gen.generate(pwd, ui_.salt_, priv);
    CoinKeyPair coin(ui_.cid_);
    coin.create(priv.data(), priv.size());
    std::pair<Password, Password> pair = std::make_pair(pwd, ui_.salt_);
    auto ret = coins.emplace(std::make_pair(pair, coin));
    if (ret.second) --cnt;
  }
  initJSON();
  addJSON(ui_);
  addJSON(coins);
  flushJSON();
}

void CommandInterpreter::doAttach() {
  if (!ui_.attach_ || !ui_.attach_.value())
    throw std::invalid_argument(
        "attach: invalid parameters <password length | salt | address>");

  auto len = ui_.attach_.value().pwd_len_;
  ByteVect pwd(len);
  CoinKeyPair coin(ui_.cid_);
  CoinKeyPair challenge(ui_.cid_, false, ui_.attach_.value().address_);
  WarpKeyGenerator key_gen;
  RandomSeedGenerator pwd_gen(SeedChar::kAll);
  pwd_gen.init();
  SecretKey secret;
  bool isFound{false};
  auto cnt(0);
  auto start = std::chrono::system_clock::now();
  // loop until coin address of challenge found
  do {
    pwd_gen.generatePassword(pwd, pwd.size());
    key_gen.generate(pwd, ui_.salt_, secret);
    coin.create(secret.data(), secret.size());
    isFound = coin.equals(challenge);
    cnt++;
  } while (!isFound);
  auto stop = std::chrono::system_clock::now();
  auto elapsed = stop - start;
  auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
  auto combination(pow(62, pwd.size()));
  if (isFound) {
    initJSON();
    addJSON(ui_, ui_.attach_.value(), pwd);
    addJSON("performance", combination, cnt, msec.count());
    addJSON(coin);
    flushJSON();
  }
}

void CommandInterpreter::doGenerateWalletDTS() {
  if (!ui_.dts_wallet_ || !ui_.dts_wallet_.value())
    throw std::invalid_argument(
        "generate-wallet-deterministic-simple: invalid parameters <password  | "
        "salt | magic-number "
        "| key-count | is-watch-only>");
  // generate root key
  WarpKeyGenerator key_gen;
  SecretKey root;
  key_gen.generate(ui_.pwd_, ui_.salt_, root);

  unsigned long long idx = ui_.dts_wallet_.value().magic_;
  unsigned long long cnt = ui_.dts_wallet_.value().keys_ + idx;
  std::string root_hex = byte2HexString(root.data(), root.size());
  KeyVect coins;
  coins.clear();
  for (unsigned long long i = idx; i < cnt; i++) {
    // simple deterministic algorithm for child creation
    // child = string(root.hex) + string(i)
    ByteVect child;
    child.reserve(root_hex.size());
    child.insert(child.begin(), root_hex.begin(), root_hex.end());
    std::string add = std::to_string(i);
    child.reserve(child.size() + add.size());
    child.insert(child.end(), add.begin(), add.end());

    // generate new key using child as password
    SecretKey secret;
    key_gen.generate(child, ui_.salt_, secret);
    CoinKeyPair coin(ui_.cid_);
    coin.create(secret.data(), secret.size());
    coins.insert(std::end(coins), coin);
  }
  ui_.dts_wallet_.value().root_ = root_hex;
  initJSON();
  addJSON(ui_);
  addJSON(ui_.dts_wallet_.value());
  OptionsOutput options = OptionsOutput(0xff);
  if (ui_.dts_wallet_.value().is_watch_only_) {
    options.reset(OptionsOutputEnum::kKeysPrivKey);
    options.reset(OptionsOutputEnum::kRootKey);
  }
  addJSON(coins, options);
  flushJSON();
}

void CommandInterpreter::doGenerateWalletHD() {
  throw std::domain_error(
      "generate-wallet-deterministic-hierachical-bib32 command not "
      "implemented");
  /*
      SecretKey seed;
      WarpKeyGenerator key_gen;
      key_gen.generate(pwd, salt, seed);
      std::string hex(byte2HexString(seed.data(), seed.size()));

      // init master node with the seed

      // init account

      // generate wallet chains
      int chains = (internal_address_cnt == 0 ? 1 : 2);
      for (auto j = 0; j < chains; j++) {
        // generate external/internal addresses
        int addr_cnt = (j == 0 ? external_address_cnt : internal_address_cnt);
        for (auto k = 0; k < addr_cnt; k++) {
        }
        for (auto k = 0; k < addr_cnt; k++) {
        }
      }
  */
}

void CommandInterpreter::doTest() {
  throw std::domain_error(
      "verification-against-test-vectors command not implemented");
}

void CommandInterpreter::initJSON() {
  out_.clear();
  std::time_t t =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << std::put_time(std::localtime(&t), "%FT%T%Z");
  out_["_time"] = ss.str();
}

void CommandInterpreter::flushJSON() {
  result_ << std::setw(2) << out_ << std::endl;
  out_.clear();
}

void CommandInterpreter::addJSON(const UserInterface& ui) {
  out_["_user"]["command"] = ui.oper_;
  out_["_user"]["network"] = ui.cid_;
  if (ui.pwd_.size() > 0) out_["_user"]["password"] = ByteVect2String(ui.pwd_);
  if (ui.salt_.size() > 0) out_["_user"]["salt"] = ByteVect2String(ui.salt_);
}

void CommandInterpreter::addJSON(const UserInterface::WalletDTS& wallet) {
  out_["_user"]["wallet"]["_type"] = "deterministic-simple";
  out_["_user"]["wallet"]["keyCount"] = wallet.keys_;
  out_["_user"]["wallet"]["keyRoot"] = wallet.root_;
  out_["_user"]["wallet"]["magic"] = wallet.magic_;
  out_["_user"]["wallet"]["watchOnly"] = wallet.is_watch_only_;
}

void CommandInterpreter::addJSON(const UserInterface::WalletHD& wallet) {
  out_["_user"]["wallet"]["_type"] = "deterministic-hierarchical-bip-0032";
  out_["_user"]["wallet"]["extKeyCount"] = wallet.external_keys_;
  out_["_user"]["wallet"]["intKeyCount"] = wallet.internal_keys_;
  out_["_user"]["wallet"]["watchOnly"] = wallet.is_watch_only_;
}

void CommandInterpreter::addJSON(const UserInterface& ui,
                                 const UserInterface::Attach& attach,
                                 const ByteVect& pwd) {
  out_["_user"]["command"] = ui.oper_;
  out_["_user"]["network"] = ui.cid_;
  if (ui.salt_.size() > 0) {
    out_["_user"]["param"]["salt"] = ByteVect2String(ui.salt_);
  }
  out_["_user"]["param"]["passwordLenght"] = attach.pwd_len_;
  out_["_user"]["param"]["address"] = ByteVect2String(attach.address_);
  out_["attach"]["result"]["success"] = false;
  if (pwd.size() > 0) {
    out_["attach"]["result"]["success"] = true;
    out_["attach"]["result"]["password"] = ByteVect2String(pwd);
  }
}

void CommandInterpreter::addJSON(const CoinKeyPair& coin) {
  out_["key"]["address"] = ByteVect2String(coin.address());
  std::string s = ByteVect2String(coin.publicKey());
  for (auto& c : s) c = toupper(c);
  out_["key"]["publicKeyHex"] = s;
  out_["key"]["privateKeyWif"] = ByteVect2String(coin.privateKey());
}

void CommandInterpreter::addJSON(const KeyVect& coins,
                                 const OptionsOutput& options) {
  json j_coins;
  for (auto& i : coins) {
    json o;
    if (options.test(OptionsOutputEnum::kKeysAddress)) {
      o["key"]["address"] = ByteVect2String(i.address());
    }
    if (options.test(OptionsOutputEnum::kKeysPublicKey)) {
      std::string s = ByteVect2String(i.publicKey());
      for (auto& c : s) c = toupper(c);
      o["key"]["publicKeyHex"] = s;
    }
    if (options.test(OptionsOutputEnum::kKeysPrivKey)) {
      o["key"]["privateKeyWif"] = ByteVect2String(i.privateKey());
    }
    j_coins.push_back(o);
  }
  out_["keys"] = j_coins;
}

void CommandInterpreter::addJSON(const PassWordSaltKeyMap& coins) {
  json j_coins;
  for (auto& i : coins) {
    json o;
    o["_password"] = ByteVect2String(i.first.first);
    o["key"]["address"] = ByteVect2String(i.second.address());
    o["key"]["privateKeyWif"] = ByteVect2String(i.second.privateKey());
    std::string s = ByteVect2String(i.second.publicKey());
    for (auto& c : s) c = toupper(c);
    o["key"]["publicKeyHex"] = s;
    j_coins.push_back(o);
  }
  out_["keys"] = j_coins;
}

void CommandInterpreter::addJSON(const std::string& name, uint64_t combination,
                                 uint64_t cnt, uint64_t ms) {
  json stat;
  double sec(ms / 1000);
  stat["combination"] = combination;
  if (combination != 0)
    stat["coverage"] = double(cnt / double(combination)) * 100.0;
  stat["time"] = sec;
  if (ms != 0) stat["rate"] = double(cnt / sec);
  stat["trial"] = cnt;

  out_["attach"][name] = stat;
}
