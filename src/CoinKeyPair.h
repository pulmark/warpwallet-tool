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
#ifndef COINKEYPAIR_H
#define COINKEYPAIR_H

#include <array>
#include <cstdint>
#include <vector>

enum class CoinId { kBitCoin = 1, kBitCoinTest, kLiteCoin, kLiteCoinTest };

using SecretKey = std::array<uint8_t, 32>;
using ByteVect = std::vector<uint8_t>;

/// \class CoinKeyPair
/// \brief Represents coin key pair (address, public & private key).
class CoinKeyPair {
 public:
  CoinKeyPair(CoinId id, bool compressed = false, ByteVect addr = {'\0'},
              ByteVect pub = {'\0'}, ByteVect priv = {'\0'})
      : network_(id),
        compressed_(compressed),
        addr_(addr),
        pub_(pub),
        priv_(priv) {}

  /// \brief Compares keypair to other key pair.
  bool equals(CoinKeyPair& rhs) const {
    return (rhs.id() == network_ && rhs.address() == addr_);
  }

  /// \brief Creates a new coin keypair using given 'secret'
  void create(const uint8_t* secret, unsigned int secret_len);

  CoinId id() const { return network_; }
  ByteVect address() const { return addr_; }
  ByteVect publicKey() const { return pub_; }
  ByteVect privateKey() const { return priv_; }

 private:
  CoinId network_;   /// network
  bool compressed_;  /// public key compressed/uncompressed
  ByteVect addr_;    /// address, base58check
  ByteVect pub_;     /// public key, base58check
  ByteVect priv_;    /// private key, WIF, base58check

  bool isValid_;
};

#endif  // COINKEYPAIR_H
