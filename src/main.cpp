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

#include <iostream>

#include "CoinKeyPair.h"
#include "CommandInterpreter.h"
#include "RandomSeedGenerator.h"
#include "WarpKeyGenerator.h"

int main(int argc, char* argv[]) {
  try {
    UserInterface ui(std::cout);
    if (ui.parse(argc, argv)) {
      CommandInterpreter cmd(ui);
      cmd.execute();
      ui.show(cmd.result());
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
