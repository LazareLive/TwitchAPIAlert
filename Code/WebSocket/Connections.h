/* Project : Twitch/Arduino Connection
 * Version : 0.1 (dev) 
 * Developper : Lazare (twitch.tv/lazarelive)
 * With the help of Akanoa, ghostwake, Nesquiik69
 * May 2020
 * 
 * Connection file
 * Modify this file to connect to your API
 */

#ifndef _TWITCH_CONNECTIONS
#define _TWITCH_CONNECTIONS

#define _ESP_TIMEOUT          20

//Login for your Internet Access
#define ESP_LOGIN 		        "Your web connection here"
//Password for your Internet Acess
#define ESP_PASSWORD	        "Your web password here"

//Twitch Client ID : given on the dev.twitch.tv interface
#define TWITCH_CLIENT_ID      "client id"
//Twitch Client Secret : given on the dev.twitch.tv interface
//Generate new secret when needed
#define TWITCH_CLIENT_SECRET  "client secret"
//Your Twitch login name : lowercase only
#define TWITCH_LOGIN          "your login"

#define WEBHOOK_CALLBACK      "http://your ip:52810"

#endif
