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

#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>

#include "CoinKeyPair.h"
#include "lib_bitcointool.h"

std::string byte2HexString(const uint8_t* data, int len) {
  std::stringstream ss;
  ss << std::hex;
  for (int i = 0; i < len; ++i)
    ss << std::setw(2) << std::setfill('0') << (unsigned)data[i];
  return ss.str();
}

std::vector<uint8_t> string2ByteArray(char* s, unsigned int s_len) {
  std::vector<uint8_t> v;
  v.resize(s_len);
  for (auto i = 0U; i < s_len; i++) {
    v.push_back(static_cast<uint8_t>(*(s + s_len)));
  }
  return v;
}

namespace {
const std::array<const char*, 13> CMD_GENERATE_KEYS_ALL = {
    "bitcoin-tools", "--network",      "bitcoin", "--public-key-compression",
    "uncompressed",  "--output-type",  "all",     "--input-type",
    "private-key",   "--input-format", "hex",     "--input",
    nullptr};

const std::map<CoinId, std::string> NETWORK_TYPE = {
    {CoinId::kBitCoin, "bitcoin"},
    {CoinId::kBitCoinTest, "bitcoin-testnet"},
    {CoinId::kLiteCoin, "litecoin"},
    {CoinId::kLiteCoinTest, "litecoin-testnet"}};
}

void CoinKeyPair::create(const uint8_t* secret, unsigned int secret_len) {
  if (secret == nullptr || secret_len < 2)
    throw std::invalid_argument("CoinKeyPair::invalid secret key");

  // init argument vector
  std::array<const char*, 13> argv = CMD_GENERATE_KEYS_ALL;
  std::string s = byte2HexString(secret, secret_len);
  argv.at(2) = NETWORK_TYPE.at(network_).c_str();
  argv.at(4) = (compressed_ ? "compressed" : "uncompressed");
  argv.back() = s.c_str();

  // run command using bitcoin-tool
  LibBitcoinTool tool;
  int err = tool.run(argv.size(), argv.data());
  if (err == -1)
    throw std::domain_error(
        "CoinKeyPair::LibBitcoinTool::parseOptions() failed");
  if (err == -2)
    throw std::domain_error("CoinKeyPair::LibBitcoinTool::run() failed");

  // process results
  if (!tool.getValue("address.base58check:", addr_))
    throw std::domain_error("CoinKeyPair::invalid address");

  //  if (!tool.getValue("public-key.base58check:", pub_))
  //    throw std::domain_error("CoinKeyPair::invalid public key");

  if (!tool.getValue("public-key.hex:", pub_))
    throw std::domain_error("CoinKeyPair::invalid public key");

  if (!tool.getValue("private-key-wif.base58check:", priv_))
    throw std::domain_error("CoinKeyPair::invalid private key");
}
