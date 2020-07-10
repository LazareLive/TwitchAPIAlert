/* Project : Twitch/Arduino Connection
 * Version : 0.1 (dev) 
 * Developper : Lazare (twitch.tv/lazarelive)
 * With the help of Akanoa, ghostwake, Nesquiik69
 * May 2020
 * 
 * Class : TwitchHelix
 * Allow user to connect to the Twitch Helix API using a ESP8266
 * Webhooks tried but not working for now using this technology
 * Modify connexions.h to get your data working
 */

#ifndef _TWITCH_HELIX_CLASS
#define _TWITCH_HELIX_CLASS

#define _HELIX_NO_CHANGE      0x00
#define _HELIX_CHANGED        0x01
#define _HELIX_ERROR_WEB      0x02
#define _HELIX_ERROR_PARSING  0x04
#define _HELIX_ERROR_OAUTH    0x08

class TwitchHelix {
  public :
    //Constructor
    TwitchHelix(void);

    //Login/logout OAuth functions
    byte   requestOAuth(void);
    byte   revokeOAuth(void);
    byte   refreshOAuth(void);
    byte   validateOAuth(void);

    //Request functions
    byte   requestUserID(void);
    byte   requestStreamInformations(void);

    //Webhook request
    void   webhookSubscribeFollower(void);
    void   webhookSubscribeStream(void);

    //Get functions
    String getOAuthKey(void)        { return _OAuthKey; }
    String getUserID(void)          { return _TwitchUserID; }

    String getStreamTitle(void)     { return _TwitchStreamTitle; }
    bool   getIsStreamLive(void)    { return _TwitchStreamLive; }
    int    getViewerCount(void)     { return _TwitchViewerCount; }

    String getURIFollower(void)     { return _webhookURIFollow; }   
    String getURIStream(void)       { return _webhookURIStream; }   
    
  private : 
    //Variables
    String _OAuthKey;
    
    String _TwitchUserID;
        String _TwitchStreamTitle;
    bool   _TwitchStreamLive;
    int    _TwitchViewerCount;

    //Constantes
    const String _URLTwitchID  =      "id.twitch.tv";
    const String _URIOAuthToken =     "/oauth2/token";
    const String _URIOAuthRevoke =    "/oauth2/revoke";
    const String _URIOAuthValidate =  "/oauth2/validate";
    
    const String _URLTwitchAPI =      "api.twitch.tv";
    const String _URIUser =           "/helix/users";
    const String _URIStreams =        "/helix/streams";
    const String _URIWebhook =        "/helix/webhooks/hub";

    const String _webhookURIFollow =  "/follow";
    const String _webhookURIStream =  "/stream";

    const String _fingerprintID =     "1F:97:3B:C1:77:C3:F3:07:23:59:4D:8E:5B:45:27:AC:E4:AF:CA:07";
    const String _fingerprintAPI =    "EB:D3:54:43:95:57:A4:0D:D0:B7:10:19:65:D5:7A:9B:E3:5E:0D:69";
};

#endif
