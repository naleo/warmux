/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <map>

#include <WORMUX_types.h>
#include <WORMUX_network.h>
#include <WORMUX_socket.h>
#include <WSERVER_config.h>
#include <WSERVER_clock.h>
#include <WSERVER_debug.h>
#include <WSERVER_env.h>
#include "config.h"
#include "server.h"

BasicClock wx_clock;

int main(int /*argc*/, char* /*argv*/[])
{
  DPRINT(INFO, "Wormux headless server version %i", VERSION);
  DPRINT(INFO, "%s", wx_clock.DateStr());
  Env::SetConfigClass(config);
  Env::SetWorkingDir();
  Env::SetChroot();
  Env::MaskSigPipe();
  Env::SetMaxConnection();

  int port = 0;
  if (!config.Get("port", port)) {
    DPRINT(INFO, "ERROR: No port specified");
    exit(EXIT_FAILURE);
  }

  std::string password = "";
  uint max_nb_clients = 4;
  GameServer server;
  if (!server.ServerStart(uint(port), max_nb_clients, "dedicated", password)) {
    DPRINT(INFO, "ERROR: Server not started");
    exit(EXIT_FAILURE);
  }

  server.RunLoop();
}

