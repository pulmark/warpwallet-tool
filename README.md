## Introduction
A command-line tool to generate cryptocurrency addresses, keys and wallets by using WarpWallet algorithm.
More information about the algorithm can be found at [Keybase](https://keybase.io/warp).

The private key generation has been done as described by the algorithm. At the moment tool support generation of addresses 
and keys for following cryptocurrency networks:

* bitcoin, bitcoin-test
* litecoin, litecoin-test

## Disclaimer
Use with care. Check that generated addresses & private keys are valid before using them. 

Never use system that is connected to network for address & private key generation or to store private keys. Sign transactions in isolated, [air-gapped](https://www.techopedia.com/definition/17037/air-gap) system before sending transaction into network.

Understand the risks of using [brain wallets](https://en.bitcoin.it/wiki/Brainwallet) and benefits of using [deterministic wallets](https://en.bitcoin.it/wiki/Deterministic_wallet).

## Features
The following operations are supported: 

### 1. Generate Key
Generates a coin address, private key using passphrase and salt.

* Command params: **-n {network id} -c 1 -p {passphrase} {salt}**
* Example Output:
```
{
  "_time": "2017-12-14T13:54:21EET",
  "_user": {
    "command": "generate-key",
    "network": 1,
    "password": "password",
    "salt": "let@me.out"
  },
  "key": {
    "address": "1Ds1WyW4GHa2V9ZqXfTohrZMwUuZhXkodv",
    "privateKeyWif": "5HwxMHzPvebezLnqUPaFvTNgw1EHho2Ch5wGtZvBmkhH5aLpE8r"
  }
}
```

### 2. Generate Keys Random
Generates a list of coin addresses, private keys using random passphrase generator and salt.
Mersenne-Twister engine (PRNG) is used for random passphrase generation.

* Command params: **-n {network id} -c 2 -p {random password length} {salt} {keys count}**
* Example Output:
```
{
  "_time": "2017-12-14T14:01:07EET",
  "_user": {
    "command": "generate-key-random",
    "network": 1,
    "passwordLength": 12,
    "keysCount": 2,
    "salt": "let@me.out"
  },
  "keys": [
    {
      "_password": "fSoaffmVso07",
      "key": {
        "address": "1LHy86GqgUuHCFE3uK7FVAxyVn7YqH16D4",
        "privateKeyWif": "5KkyfWF5vWEh1SxjAvDzFAyJkXyZm7ZLmsJvKN8KfrBoKY7hFX1"
      }
    },
    {
      "_password": "vCBjzMFiHhk3",
      "key": {
        "address": "129AZmYwYguEgXBKijsoxpCiCeweBdQ6Bs",
        "privateKeyWif": "5KZAxPV7gmTkDNz4cJTE3b66fbU6f39xqoV95DJPzeGxa3NRWkp"
      }
    }
  ]
}
```

### 3. Attach
Tries to find private key and passphrase for given coin address.

* Command params: **-n {network id} -c 3 -p {passphrase length} {salt} {coin address}**

### 4. Generate Deterministic Wallet
Generates a list of coin addresses, private keys using passphrase, salt, magic number. The magic number adds more 
randomness and security against attachers trying to find private keys for coin addresses. The number can be birth date, 
shoe size, phone number, apartment number, street address number etc. that is easy to remember.

Coin private key derivation process:
```
Master Key = WarpKeyGenerator(Passphrase, Salt) 
Child Key = Master Key + Magic Number
Coin Private Key = WarpKeyGenerator(Child Key, Salt)
```
* Command params: **-n {network id} -c 4 -p {passphrase} {salt} {magic number} {keys count} {watch only}**
* Example Output:
```
{
  "_time": "2017-12-14T14:07:12EET",
  "_user": {
    "command": "generate-wallet-deterministic-dts",
    "network": 1,
    "password": "password",
    "salt": "let@me.out",
    "wallet": {
      "_type": "deterministic-dts",
      "keyCount": 2,
      "keyRoot": "1169809122274da2467cdc89c4d2ccb635216df180f6773389972284c2bcc4cd",
      "magic": 12345,
      "watchOnly": false
    }
  },
  "keys": [
    {
      "key": {
        "address": "1EGkqRyiNcMs7JNdDPig3XcqXSd3cRxGsf",
        "privateKeyWif": "5Jw5chS7n6xSYCC6EMgnN3foB1c2v592e96pf23H2HRoydcHLZp"
      }
    },
    {
      "key": {
        "address": "1AgFGzJhdjzg3obmcygaiA5KnQn7JZJHQP",
        "privateKeyWif": "5KQZ9MQ1CXDMrrWRs7dZWjrFMBF3qumfSvxwpNU2FXZBwNApgdC"
      }
    }
  ]
}
```

### 5. Generate Hierarchical Deterministic Wallet (BIP-0032)
Generates [BIP32](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki) wallet using passphrase, salt, external keys count, internal keys count.

* Command params : **-n {network id} - c 5 - p {passphrase} {salt} {ext keys count} {int keys count} {watch only}**
* Status: work in progress

## Portability

The external [cppcrypto](https://sourceforge.net/projects/cppcrypto/files) library supports only x86 processors(32-bit or 64-bit).
The development and testing has been done on laptop running Linux x86_64. No other desktop platforms has been tested.

The distribution source code is generic so it should be easy to build binaries for other platforms as well.
The source code is implemented so that core logic and user interface is separated to allow integration of 
core functionality into other type of user interfaces.

## Dependencies
The source code uses standard C++ libraries. The compiler must support C++14 standard or never.
The external libbitcoin-tool library depends on OpenSSL development libraries and headers. OpenSSL version supported is 1.1.0g.      

The package depends on the following externals:

### Header-only Libraries
* [json.hpp](https://github.com/nlohmann/json) - json formatted output, [MIT](https://github.com/nlohmann/json/blob/master/LICENSE.MIT)
* [CLI](https://github.com/CLIUtils/CLI11) - command-line parsing, [BSD-3](https://github.com/CLIUtils/CLI11/blob/master/LICENSE)

### Static Libraries
* [cppcrypto](https://sourceforge.net/projects/cppcrypto/files) - small, fast, cross-platform BSD licensed C++ crypto library  
* [libbitcoin-tool](https://github.com/pulmark/bitcoin-tool) - generates cryptocurrency addresses and keys, [MIT](https://github.com/pulmark/bitcoin-tool/blob/master/lib/LICENSE.md)

The distribution doesn't use any QT libraries or headers, only the build tool QT Creator.

## Build
The build is done by using QT Creator Community Edition, version 5.9.2. 
The project file can be found at the project root: [warp-util.pro](https://github.com/pulmark/warpwallet-tool/blob/master/warp-util.pro). 

The external headers (JSON, CLI) are already copied into include sub-directory. 

Fetching the sources for external libraries from repositories and build them for distribution is not yet supported.
This must be done separately.

### Build cppcrypto library

* View [instructions](https://github.com/pulmark/warpwallet-tool/blob/master/externals/crypto/cppcrypto/doc/readme.html) for details where to get it and how-to build. 
* The path into target library location is ./externals/crypto/cppcrypto.

### Build libbitcoin-tool library

* Clone [project](https://github.com/pulmark/bitcoin-tool) into .externals/bitcoin-tool/ directory. 
* The library is derived from bitcoin-tool command line tool by adding a simple wrapper to use it as library. 
* For building use QT project file included in it's lib sub-directory.

Currently distribution contains only one executable. Later the purpose is to build separate packages for core 
functionality and user interface. Some tests has been done by using Google's Bazel for build. Bazel might 
be the tool that is utilized later to build the distribution packages.

## Testing
During development the validity of generated bitcoin addresses and keys has been verified by using the bitaddress.org wallet tool.

Test cases for operations implemented has been created. Next step is to build test suite that 
verifies program output against these cases.

## Credits
* Max Krohn and Chris Coyne, authors of WarpWallet algorithm,
* Colin Percival, author of Scrypt algorithm,
* Developers and contributors of libraries & headers used in the distribution

## Support, Questions

Contact, chat or donate via my [keybase-id](https://keybase.io/pulmark).

I'm currently between jobs, searching for work opportunities that utilize block chain technologies. 
Do not hesitate to contact me also in case you require more information or if you are interested in 
hiring me.

## License
Copyright &#169; [Markku Pulkkinen](http://www.markkupulkkinen.com). 
Released under the [GNU Lesser General Public License v3.0](https://www.gnu.org/licenses/lgpl-3.0.md).  