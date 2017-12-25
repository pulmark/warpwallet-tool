## Introduction
A command-line tool to generate cryptocurrency addresses, keys and wallets by using WarpWallet algorithm.
More information about the algorithm can be found at [Keybase](https://keybase.io/warp).

The private key generation is done as described by the algorithm. At the moment tool support generation of addresses 
and keys for following cryptocurrency networks:

* bitcoin, bitcoin-test
* litecoin, litecoin-test

The tool can generate random keys, addresses if required by using random chars(alphabets) or passphrase from 
dictionary file. The tool supports passphrase generation for languages defined in [BIP-0039](https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki) 
proposal and a proprietary wordset for finnish language.

## Disclaimer

The code is experimental so use tool with care. Check that generated addresses & private keys are valid before using them. Understand the risks of using [brain wallets](https://en.bitcoin.it/wiki/Brainwallet) and benefits of using [deterministic wallets](https://en.bitcoin.it/wiki/Deterministic_wallet).

## Features

NOTE: To use language based random passphrase generation, the location of dictionary files must be the same as program directory.

The following operations are supported: 

#### 1. Generate Key
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

#### 2. Generate Keys Random
Generates a list of coin addresses, private keys using random passphrase generator and salt.
Mersenne-Twister engine (PRNG) is used for random passphrase generation.

* Command params: **-n {network id} -l {language code} -c 2  -p {random password length} {salt} {keys count}**
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

#### 3. Attach
Tries to find private key and passphrase for coin address.

* Command params: **-n {network id} -l {language code} -c 3 -p {passphrase length} {salt} {coin address}**
* Example Output:
```
{
  "_time": "2017-12-15T12:07:46EET",
  "_user": {
    "command": "attach-address",
    "network": 1,
    "param": {
      "address": "1MbZtq5vYiFBb9uWmM9t6nozCf1n4qb8yQ",
      "passwordLenght": 2,
      "salt": "let@me.out"
    }
  },
  "attach": {
    "performance": {
      "combination": 3844,
      "coverage": 129.39646201873,
      "rate": 1.37783933518006,
      "time": 3610.0,
      "trial": 4974
    },
    "result": {
      "password": "Dx",
      "success": true
    }
  },
  "key": {
    "address": "1MbZtq5vYiFBb9uWmM9t6nozCf1n4qb8yQ",
    "privateKeyWif": "5JD7KcrBdQZ3o8LZ6zxVTfrrURduwuoj2P9DiJr3dnjtRQpVKN6",
  }
}
```

#### 4. Generate Deterministic Wallet
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

#### 5. Generate Hierarchical Deterministic Wallet (BIP-0032)
Generates [BIP32](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki) wallet using passphrase, salt, external keys count, internal keys count.

* Command params : **-n {network id} - c 5 - p {passphrase} {salt} {ext keys count} {int keys count} {watch only}**
* Status: work in progress

#### 5. Generate Random Password/Passphrase
Just generates random passwords or passphrases.

* Command params : **-n {network id} -l {language code} - c 6 - p {passphrase length} {passphrase count} {custom charset} {mask}**
* Example Output:
```
{
    harsh inject rural dentist glow need eyebrow reason
    ankle faint spend pulp olive broom warm stick
    elbow street rely fever shrug month cross they
    mad climb describe hour card basic enemy dizzy
    good fork pencil radio armed curtain question depth
    garage various business adapt real satisfy grid tag
    leg seven pizza chair woman dynamic chunk lumber
    script rice divide rent teach nerve music exact
    kid sheriff hat alien expand gain wheel wide
    romance argue lawn fresh person zebra ready able
}
```

## Portability
The external [cppcrypto](https://sourceforge.net/projects/cppcrypto/files) library supports only x86 processors (32-bit or 64-bit).
The development and testing has been done on laptop running Debian based Linux x86_64. No other desktop platforms has been tested.

The source code uses standard C++ libraries. The compiler must support C++14 standard or never. The code is generic so it should 
be easy to build binaries for other platforms as well. The core logic and user interface is separated to allow integration 
of core functionality into other type of user interfaces.

## Dependencies
The distribution depends on the following externals:

### Header-only Libraries
* [json.hpp](https://github.com/nlohmann/json) - json formatted output, [MIT](https://github.com/nlohmann/json/blob/master/LICENSE.MIT)
* [CLI](https://github.com/CLIUtils/CLI11) - command-line parsing, [BSD-3](https://github.com/CLIUtils/CLI11/blob/master/LICENSE)

### Static Libraries
* [cppcrypto](https://sourceforge.net/projects/cppcrypto) - small, fast, cross-platform C++ crypto library [License](https://github.com/pulmark/warpwallet-tool/blob/master/externals/crypto/doc/license.txt)  
* [libbitcoin-tool](https://github.com/pulmark/bitcoin-tool) - generates cryptocurrency addresses and keys, [MIT](https://github.com/pulmark/bitcoin-tool/blob/master/lib/LICENSE.md)

The external libbitcoin-tool library depends on openSSL development libraries and headers, openSSL version supported is 1.1.0g. The code doesn't use any QT libraries or headers, only the build tool QT Creator.

## Build
The build is done by using QT Creator Community Edition, version 5.9.2 and qmake. The QT project file can be found at project root (warp-util.pro). 
The external headers (json, CLI) are already copied into include sub-directory.

### Build cppcrypto library

* follow [instructions](http://cppcrypto.sourceforge.net/) and download, extract library into ./externals/crypto/ sub-directory
* run make (requires yaml)

### Build libbitcoin-tool library
The library is derived from bitcoin-tool command line tool by adding a simple wrapper to use it as library

* git clone [project](https://github.com/pulmark/bitcoin-tool) into ./externals/bitcoin-tool/
* build library using QT project file (lib_bitcointool.pro) located in ./externals/bitcoin-tool/lib/

## Testing, Performance
During development the validity of generated bitcoin addresses and keys has been verified by using the bitaddress.org wallet tool.

When executing attach operation my laptop (Intel dual-core i7-2620M, 3.19GHz) runs hot, CPU usage 45-50%. 
The hash rate for the attach loop (generate random password, generate WarpWallet key, generate coin keys & address) 
is around 1.3 - 1.4 H/s. The performance of M-T engine PRNG varies. I have to investigate more PRNG algorithms, 
don't know of them. I suspect that the engine's initialization seed is not optimal.

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