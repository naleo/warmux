/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 ******************************************************************************
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/


#include "network_connection_menu.h"
#include "network_menu.h"
#include "internet_menu.h"

#include "game/game.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "map/maps_list.h"
#include "network/net_error_msg.h"
#include "network/index_server.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/string_tools.h"

NetworkConnectionMenu::NetworkConnectionMenu() :
  Menu("menu/bg_network", vOkCancel)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  
  Rectanglei stdRect(0, 0, 360, 64);

  uint x_button = AppWormux::GetInstance()->video.window.GetWidth()/2 - stdRect.GetSizeX()/2;
  uint y_box = AppWormux::GetInstance()->video.window.GetHeight()/2 - 200;

  // Connection related widgets
  connection_box = new VBox(Rectanglei( x_button, y_box, stdRect.GetSizeX(), 1), false);
  connection_box->SetBorder(Point2i(0,0));

  // What do we want to do ?
  Box* action_box = new HBox(stdRect, false);

  previous_action_bt = new Button(Point2i(0, 0), res, "menu/really_big_minus", false);
  next_action_bt = new Button(Point2i(0, 0), res, "menu/really_big_plus", false);
  action_label = new Label(_("Connect to an internet game"), 
			   Rectanglei(0,0,250,0), 
			   Font::FONT_BIG, Font::FONT_NORMAL, white_color, true);
  action_box->AddWidget(previous_action_bt);
  action_box->AddWidget(action_label);
  action_box->AddWidget(next_action_bt);

  connection_box->AddWidget(action_box);

  // Server address
  server_address_label = new Label(_("Server address:"), rectZero, Font::FONT_MEDIUM, Font::FONT_NORMAL);
  connection_box->AddWidget(server_address_label);
  server_address = new TextBox("localhost", rectZero, Font::FONT_MEDIUM, Font::FONT_NORMAL);
  connection_box->AddWidget(server_address);

  // Server port
  port_number_label = new Label(_("Port:"), rectZero, Font::FONT_MEDIUM, Font::FONT_NORMAL);
  connection_box->AddWidget(port_number_label);
  port_number = new TextBox(WORMUX_NETWORK_PORT, rectZero, Font::FONT_MEDIUM, Font::FONT_NORMAL);
  connection_box->AddWidget(port_number);

  // Available on internet ?
  internet_server = new CheckBox(_("Server available on Internet"),
				 Rectanglei(0,0,0,0),
				 true);
  connection_box->AddWidget(internet_server);

  widgets.AddWidget(connection_box);

  SetAction(NET_BROWSE_INTERNET);

  // Warning about experimental networking
  msg_box = new MsgBox(Rectanglei( AppWormux::GetInstance()->video.window.GetWidth()/2 - 300,
				   y_box+connection_box->GetSizeY() + 30, 
				   600, 200), 
		       Font::FONT_SMALL, Font::FONT_NORMAL);
  widgets.AddWidget(msg_box);

  msg_box->NewMessage(_("WARNING!! Network is still under developement and therefore a little experimental."), c_red);
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Some weapons are disabled, because of known bugs (ninjarope, airhammer, blowtorch, submachine gun) and surely many other things don't work either!"));
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find some opponents."));
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Have a good game!"));
  msg_box->NewMessage(""); // Skip a line

  resource_manager.UnLoadXMLProfile(res);
}

NetworkConnectionMenu::~NetworkConnectionMenu()
{
}

void NetworkConnectionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  if ((w == next_action_bt) ||
      (w == action_label && (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_WHEELDOWN)))
    {
      switch (current_action) {
      case NET_HOST:
	SetAction(NET_CONNECT_LOCAL);
	break;
      case NET_CONNECT_LOCAL:
	SetAction(NET_BROWSE_INTERNET);
	break;
      case NET_BROWSE_INTERNET:
	SetAction(NET_HOST);
	break;
      }
      return;
    } 

  if ((w == previous_action_bt) ||
      (w == action_label && (button == SDL_BUTTON_RIGHT || button == SDL_BUTTON_WHEELUP)))
    {
      switch (current_action) {
      case NET_HOST:
	SetAction(NET_BROWSE_INTERNET);
	break;
      case NET_CONNECT_LOCAL:
	SetAction(NET_HOST);
	break;
      case NET_BROWSE_INTERNET:
	SetAction(NET_CONNECT_LOCAL);
	break;
      }
      return;
    }
}

void NetworkConnectionMenu::OnClick(const Point2i &mousePosition, int button)
{
}

void NetworkConnectionMenu::SetAction(network_menu_action_t action)
{
  current_action = action;

  switch (current_action) {
  case NET_HOST:
    action_label->SetText(_("Host a game"));
    server_address_label->SetVisible(false);
    server_address->SetVisible(false);
    port_number_label->SetVisible(true);
    port_number->SetVisible(true);
    internet_server->SetVisible(true);
    break;
  case NET_CONNECT_LOCAL:
    action_label->SetText(_("Connect to game"));
    server_address_label->SetVisible(true);
    server_address->SetVisible(true);
    port_number_label->SetVisible(true);
    port_number->SetVisible(true);
    internet_server->SetVisible(false);
    break;
  case NET_BROWSE_INTERNET:
    action_label->SetText(_("Connect to an internet game"));
    server_address_label->SetVisible(false);
    server_address->SetVisible(false);
    port_number_label->SetVisible(false);
    port_number->SetVisible(false);
    internet_server->SetVisible(false);
    break;
  } 
}

void NetworkConnectionMenu::Draw(const Point2i &mousePosition){}

void NetworkConnectionMenu::DisplayError(Network::connection_state_t conn)
{
  DispNetworkError(conn);
  Menu::RedrawMenu();
}

bool NetworkConnectionMenu::signal_ok()
{
  Network::connection_state_t conn;
  bool r = false;

  switch (current_action) {
  case NET_HOST: // Hosting your own server
    if( !internet_server->GetValue() )
      index_server.SetHiddenServer();

    conn = index_server.Connect();
    if(conn != Network::CONNECTED)
    {     
      DisplayError(conn);
      msg_box->NewMessage(_("Error: Unable to contact index server to host a game"), c_red);
      goto out;
    }

    conn = Network::GetInstance()->ServerStart(port_number->GetText());
    if( conn != Network::CONNECTED)
    {      
      DisplayError(conn);
      goto out;
    }

    index_server.SendServerStatus();

    if (!Network::GetInstance()->IsConnected()) {
      msg_box->NewMessage(_("Error: Unable to start server"), c_red);
      goto out;
    }
    break;

  case NET_CONNECT_LOCAL: // Direct connexion to a server
    conn = Network::ClientStart(server_address->GetText(), port_number->GetText());
    if (!Network::IsConnected() || conn != Network::CONNECTED) {
      DisplayError(conn);

      // translators: %s:%s will expand to something like "example.org:9999"
      msg_box->NewMessage(Format(_("Error: Unable to connect to %s:%s"),
				 (server_address->GetText()).c_str(), (port_number->GetText()).c_str()),
			  c_red);
      goto out;
    }
    break;

  case NET_BROWSE_INTERNET: // Search an internet game!
    conn = index_server.Connect();
    if (conn != Network::CONNECTED) {
      DisplayError(conn);
      msg_box->NewMessage(_("Error: Unable to contact index server to search an internet game"), c_red);
      goto out;
    }

    InternetMenu im;
    im.Run();

    index_server.Disconnect();

    // we don't go back into the main menu!
    // -> im.Run() may have connected to a host so the 
    // if(Network::GetInstance()->IsConnected()) just below will be catched and close the menu
    break;
  }

  if (Network::GetInstance()->IsConnected()) {
    // run the network menu ! :-)
    NetworkMenu nm;
    Network::GetInstance()->network_menu = &nm;
    nm.Run();
    Network::GetInstance()->network_menu = NULL;
    index_server.Disconnect();

    // back to main menu after playing
    Network::Disconnect();
    return true;
  } else {
    // if InternetMenu was closed without making a connection menu has to be redrawn
    Menu::RedrawMenu();
    Network::Disconnect();
    goto out;
    // TODO : add error sound and dialog
  }

  r = true;
 out:
  Network::Disconnect();
  return r;
}

bool NetworkConnectionMenu::signal_cancel()
{
  Network::Disconnect();
  return true;
}

