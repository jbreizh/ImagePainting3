//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//#define STA       // Decomment this to use STA mode instead of AP
//#define CORS      // Decomment this to support CORS
//#define DEBUG     // Decomment this to print some debug indication
#define BUTTON    // Decomment this to use BUTTON
#define FEATURE DotStarBgrFeature // Dotstars : DATA_PIN : MOSI / CLOCK_PIN :SCK (Wemos D1 mini DATA_PIN=D7(GREEN) CLOCK_PIN=D5 (Yellow))
#define METHOD DotStarSpiMethod // Dotstars
//#define FEATURE NeoGrbFeature // Neopixels : DATA_PIN : RDX0/GPIO3 (Wemos D1 mini DATA_PIN=RX)
//#define METHOD Neo800KbpsMethod // Neopixels
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <LittleFS.h>

// LED --------------
const int NUMPIXELS = 119;
NeoPixelBus<FEATURE, METHOD> STRIP(NUMPIXELS);
NeoPixelAnimator ANIMATIONS(1); // NeoPixel animation management object
NeoBitmapFile<FEATURE, fs::File> NEOBMPFILE;
// end LED -----------

// WEBSERVER --------------
ESP8266WebServer server;
struct t_httpAnswer
{
 int statusCode;
 String contentType;
 String contentData;
};
// end WEBSERVER -----------

// WIFI --------------
#ifdef STA // STA Mode
const char* ssid = "Redmi Note 10 Pro"; // your wifi ssid for STA mode
const char* password = "1122334455"; // your wifi password for AP mode
#else // AP Mode
const char* ssid = "imagePainting"; // wifi ssid for AP mode
IPAddress apIP(192, 168, 1, 1); // wifi IP for AP mode
#endif
// end WIFI -----------

// FS --------------
fs::File UPLOADFILE; // hold uploaded file
// end FS -----------

// PARAMETER --------------
struct t_parameter
{
  String bmppath;
  uint16_t indexstart;
  uint16_t indexstop;
  uint16_t indexmax;
  uint8_t delay;
  uint8_t brightness;
  bool isinvert;
  HtmlColor endcolor;
  bool isendoff;
  bool isendcolor;
  HtmlColor cutcolor;
  uint8_t vcut;
  uint8_t hcut;
  bool isalternate;
  uint8_t repeat;
  uint16_t wait;
  bool isbounce;
};
t_parameter PARAMETER; // Hold parameter
const t_parameter PARAMETERDEFAULT = {"",0,0,0,15,25,false,HtmlColor(0xffffff),true,false,HtmlColor(0),0,0,false,0,0,false}; // default parameter value (can be edit)
// end PARAMETER --------------

// PLAYLIST --------------
const uint8_t PLAYLISTMAX = 5; // Playlist max size
t_parameter PLAYLIST[PLAYLISTMAX]; // Hold playlist
uint8_t PLAYLISTSIZE; // Playlist size
// end PLAYLIST --------------

// ACTION --------------
struct t_action
{
  bool isplaylist; // Render playlist
  bool istrigger; // Trigger playlist
};
t_action ACTION = {false, false};
// end ACTION --------------

// RUNTIME --------------
t_parameter PARAMETERTEMP; // Temp for parameter
uint16_t INDEXCOUNTER; // Counter for index
uint16_t WAITCOUNTER; // Counter for wait
uint8_t REPEATCOUNTER; // Counter for repeat
uint8_t VCUTCOUNTER; // Counter for vcut
uint8_t HCUTCOUNTER; // Counter for hcut
uint8_t PLAYLISTCOUNTER; // Counter for playlist
// end RUNTIME --------------

// BUTTON --------------
#ifdef BUTTON
long DEBOUNCETIME = 50; //Debouncing Time in Milliseconds
long HOLDTIME = 500; // Hold Time in Milliseconds
const int BTNA_PIN = D3; //PIN for the button A
const int BTNB_PIN = D4; //PIN for the button B
long BTNATIMER = 0;
long BTNBTIMER = 0;
boolean ISBTNA = false;
boolean ISBTNB = false;
boolean ISBTNAHOLD = false;
boolean ISBTNBHOLD = false;
#endif
// end BUTTON-----------

// DEBUG --------------
#ifdef DEBUG
long ANIMATIONDURATION;
#endif
// end BUTTON-----------

//SHADER --------------
template<typename T_COLOR_OBJECT> class BrightnessShader : public NeoShaderBase
{
  public:
    BrightnessShader():
      NeoShaderBase()
    {}

    T_COLOR_OBJECT Apply(uint16_t index, const T_COLOR_OBJECT src)
    {
      //
      T_COLOR_OBJECT result;
      T_COLOR_OBJECT color;
      
      // Horizontal cut counter incrementation
      if (HCUTCOUNTER > 1) HCUTCOUNTER -= 1;
      else HCUTCOUNTER = 2*PARAMETERTEMP.hcut;
        
      //  Blank or color the strip during the horizontal cut
      if (HCUTCOUNTER <= PARAMETERTEMP.hcut) color = src;
      else color = PARAMETERTEMP.cutcolor;
    
      // below is a fast way to apply brightness to all elements (only 8bits) of the color
      const uint8_t* pColor = reinterpret_cast<const uint8_t*>(&color);
      uint8_t* pResult = reinterpret_cast<uint8_t*>(&result);
      const uint8_t* pColorEnd = pColor + sizeof(T_COLOR_OBJECT);
      while (pColor != pColorEnd) *pResult++ = (*pColor++ * (uint16_t(PARAMETERTEMP.brightness) + 1)) >> 8;
      
      //
      return result;
    }
};

typedef BrightnessShader<FEATURE::ColorObject> BrightShader;
BrightShader SHADER;
//end SHADER --------------

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void setup()
{
  // FS setup
  LittleFS.begin();

  // Serial setup
  Serial.begin(115200);

  // Wifi setup
#ifdef STA //STA Mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
#else //AP Mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
#endif

  // Webserver setup
  // delete file
  server.on("/delete", HTTP_DELETE, handleFileDelete);

  // handle file upload
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "UPLOAD SUCCESS");
  }, handleFileUpload);
    
  // handle Play
  server.on("/play", HTTP_GET, handlePlay);

  // handle Stop
  server.on("/stop", HTTP_GET, handleStop);

  // handle light
  server.on("/light", HTTP_GET, handleLight);

  // handle burn
  server.on("/burn", HTTP_GET, handleBurn);

  // handle parameter Read
  server.on("/parameterRead", HTTP_GET, handleParameterRead);

  // handle parameter Save
  server.on("/parameterSave", HTTP_GET, handleParameterSave);

  // handle parameter Write
  server.on("/parameterWrite", HTTP_POST, handleParameterWrite);

  // handle parameter Restore
  server.on("/parameterRestore", HTTP_POST, handleParameterRestore);

   // handle parameter Default
  server.on("/parameterDefault", HTTP_GET, handleParameterDefault);

  // handle system Read
  server.on("/systemRead", HTTP_GET, handleSystemRead);

    // handle playlist Read
  server.on("/playlistRead", HTTP_GET, handlePlaylistRead);

  // handle playlist Save
  server.on("/playlistSave", HTTP_GET, handlePlaylistSave);

  // handle playlist Write
  server.on("/playlistWrite", HTTP_POST, handlePlaylistWrite);

  // handle playlist Restore
  server.on("/playlistRestore", HTTP_POST, handlePlaylistRestore);
  
  // handle playlist Default
  server.on("/playlistDefault", HTTP_GET, handlePlaylistDefault);
  
  // handle action Read
  server.on("/actionRead", HTTP_GET, handleActionRead);
  
  // handle action Write
  server.on("/actionWrite", HTTP_POST, handleActiontWrite);
  
  // called when the url is not defined
  server.onNotFound([]() {
#ifdef CORS //CORS enable : respond to preflight
    if (server.method() == HTTP_OPTIONS)
    {
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE");
      server.sendHeader("Access-Control-Allow-Headers", "*");
      server.send(200);
      return;
    }
#endif
    handleFileRead(server.uri());
  });
  
#ifdef CORS //CORS enable : add allow origin *
  server.enableCORS(true);
#endif

  // Webserver start
  server.begin();

  // LED setup
  STRIP.Begin();
  STRIP.ClearTo(HtmlColor(0));

  // Parameter initialization
  parameterDefault(PARAMETER);

  // Playlist initialization
  playlistDefault();
  
  // Button setup
#ifdef BUTTON
  pinMode(BTNA_PIN, INPUT_PULLUP);
  pinMode(BTNB_PIN, INPUT_PULLUP);
#endif
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void loop()
{
  // To handle the webserver
  server.handleClient();

  // To handle the LED animation
  ANIMATIONS.UpdateAnimations();
  STRIP.Show();

  // To handle the buttons
#ifdef BUTTON
  // To handle the button A
  if (digitalRead(BTNA_PIN) == LOW)
  {
    if (!ISBTNA)
    {
      ISBTNA = true;
      BTNATIMER = millis();
    }
    if ((millis() - BTNATIMER > HOLDTIME) && (!ISBTNAHOLD))
    {
      ISBTNAHOLD = true;
      stopAnimation("BURN");
    }
  }
  else if (ISBTNA)
  {
    if ((millis() - BTNATIMER > DEBOUNCETIME) && (!ISBTNAHOLD))
    {
      playAnimation();
    }
    ISBTNA = false;
    ISBTNAHOLD = false;
  }

  // To handle the button B
  if (digitalRead(BTNB_PIN) == LOW)
  {
    if (!ISBTNB)
    {
      ISBTNB = true;
      BTNBTIMER = millis();
    }
    if ((millis() - BTNBTIMER > HOLDTIME) && (!ISBTNBHOLD))
    {
      ISBTNBHOLD = true;
      stopAnimation("LIGHT");
    }
  }
  else if (ISBTNB)
  {
    if ((millis() - BTNBTIMER > DEBOUNCETIME) && (!ISBTNBHOLD))
    {
      stopAnimation("STOP");
    }
    ISBTNB = false;
    ISBTNBHOLD = false;
  }
#endif
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
String getContentType(String filename)
{
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".bmp")) return "image/bmp";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".json")) return "application/json";
  return "text/plain";
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
String getFileBasename(String fileName)
{
    int dotLastIndex = fileName.lastIndexOf('.');
    return fileName.substring(0, dotLastIndex);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
String getFileExtension(String fileName)
{
    int dotLastIndex = fileName.lastIndexOf('.');
    return fileName.substring(dotLastIndex+1, fileName.length());
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
String findFileNewname(String basename, String extension)
{
  // Initialize newname = basename.extension
  String newname = basename + "." + extension;
  
  // Find the first newname = basename.i.extension available
  uint8_t i = 0;
  while (LittleFS.exists(newname))
  {
    newname = basename + "." + String(i) + "." + extension;
    i+=1;
  }

  //return newName
  return newname;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer actionRead()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  //
  jsonDoc["isplaylist"] = ACTION.isplaylist;
  jsonDoc["istrigger"] = ACTION.istrigger;

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "application/json";
  serializeJson(jsonDoc, httpAnswer.contentData);
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleActionRead()
{
  t_httpAnswer httpAnswer = actionRead();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer actionWrite(String stringAction)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  // Convert json String to json doc
  if (deserializeJson(jsonDoc, stringAction))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "ACTION WRITE ERROR : WRONG JSON";
    return httpAnswer;
  }

  // Running or paused animation ?
  if (ANIMATIONS.IsAnimationActive(0) || ANIMATIONS.IsPaused())
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 403;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "ACTION WRITE ERROR : NOT AVAILABLE";
    return httpAnswer;
  }

  //
  if (!jsonDoc["isplaylist"].isNull()) ACTION.isplaylist = jsonDoc["isplaylist"];
  if (!jsonDoc["istrigger"].isNull()) ACTION.istrigger = jsonDoc["istrigger"];
  
  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "ACTION WRITE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleActiontWrite()
{
  t_httpAnswer httpAnswer = actionWrite(server.arg("plain"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playlistRead()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  // Write playlist as jsonArray
  JsonArray jsonArrayPlaylist = jsonDoc.to<JsonArray>();
  
  for (uint8_t i=0; i<PLAYLISTSIZE; i++)
  {
    // Write each parameter of the playlist as jsonObject in jsonArray
    JsonObject jsonObjectParameter = jsonArrayPlaylist.add<JsonObject>();
    
    // Copy parameter to jsonObjectParameter
    parameterTojsonObject(PLAYLIST[i],jsonObjectParameter);
   }

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "application/json";
  serializeJson(jsonDoc, httpAnswer.contentData);
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlaylistRead()
{
  t_httpAnswer httpAnswer = playlistRead();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playlistSave(String playlistPath)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // Create or open playlistPath
  File playlistFile = LittleFS.open(playlistPath, "w");

  // Read and Save parameter in playlistFile
  playlistFile.print(playlistRead().contentData);

  // Close playlistFile
  playlistFile.close();

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PLAYLIST SAVE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlaylistSave()
{
  t_httpAnswer httpAnswer = playlistSave(findFileNewname("playlist", "json"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playlistWrite(String stringPlaylist)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  // Convert json String to json doc
  if (deserializeJson(jsonDoc, stringPlaylist))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PLAYLIST WRITE ERROR : WRONG JSON";
    return httpAnswer;
  }

  // Running or paused animation ?
  if (ANIMATIONS.IsAnimationActive(0) || ANIMATIONS.IsPaused())
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 403;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PLAYLIST WRITE ERROR : NOT AVAILABLE";
    return httpAnswer;
  }

  // Initialize PLAYLIST and PLAYLISTSIZE to 0
  playlistDefault();
  
  // Read playlist as jsonArray
  JsonArray jsonArrayPlaylist = jsonDoc.as<JsonArray>();

  // Read each parameter of the playslist as JsonObject in jsonArray
  for (JsonObject jsonObjectParameter : jsonArrayPlaylist)
  {
    // Playlist too long ?
    if (PLAYLISTSIZE >= PLAYLISTMAX)
    {
      // Build httpAnswer and return it
      httpAnswer.statusCode = 500;
      httpAnswer.contentType = "text/plain";
      httpAnswer.contentData = "PLAYLIST WRITE ERROR : MAXSIZE="+String(PLAYLISTMAX);
      return httpAnswer;
    }
    
    // Bmp not valid?
    if (!NEOBMPFILE.Begin(LittleFS.open(jsonObjectParameter["bmp"].as<String>(), "r")))
    {
      //Build httpAnswer and return it
      httpAnswer.statusCode = 500;
      httpAnswer.contentType = "text/plain";
      httpAnswer.contentData = "PLAYLIST WRITE ERROR : WRONG BITMAP "+jsonObjectParameter["bmp"].as<String>();
      return httpAnswer;
    }
     
    // Copy jsonObjectParameter to parameter
    jsonObjectToparameter(jsonObjectParameter, PLAYLIST[PLAYLISTSIZE]);

    // PLAYLISTSIZE incrementation
    PLAYLISTSIZE++;
  }
  
  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PLAYLIST WRITE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlaylistWrite()
{
  t_httpAnswer httpAnswer = playlistWrite(server.arg("plain"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playlistRestore(String playlistPath)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // check if file at playlistPath exists
  if (!LittleFS.exists(playlistPath))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 404;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "RESTORE ERROR : PLAYLISTFILE NOT FOUND";
    return httpAnswer;
  }
  
  // Open playlistPath
  File playlistFile = LittleFS.open(playlistPath, "r");

  // Read playlistFile
  String playlistString;
  while (playlistFile.available()) playlistString += char(playlistFile.read());
  
  // Close playlistFile
  playlistFile.close();

  // Write playlist
  return playlistWrite(playlistString);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlaylistRestore()
{
  t_httpAnswer httpAnswer = playlistRestore(server.arg("plain"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playlistDefault()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;

  // Set PLAYLIST to DEFAULT
  for (uint8_t i=0; i<PLAYLISTMAX; i++)
  {
    parameterDefault(PLAYLIST[i]);
  }

  // Set PLAYLISTSIZE to 0
  PLAYLISTSIZE = 0;
  
  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PLAYLIST DEFAULT SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlaylistDefault()
{
  t_httpAnswer httpAnswer = playlistDefault();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void parameterTojsonObject(t_parameter &parameter, JsonObject &jsonObject)
{
  // Copy parameter in jsonObject
  jsonObject["ist"] = parameter.indexstart;
  jsonObject["isp"] = parameter.indexstop;
  jsonObject["imx"] = parameter.indexmax;
  jsonObject["bmp"] = parameter.bmppath;
  //
  jsonObject["dly"] = parameter.delay;
  jsonObject["bts"] = parameter.brightness;
  jsonObject["iivt"] = parameter.isinvert;
  //
  char endcolor[9];
  parameter.endcolor.ToNumericalString(endcolor, 9);
  jsonObject["eclr"] = endcolor;
  jsonObject["iedo"] = parameter.isendoff;
  jsonObject["iedc"] = parameter.isendcolor;
  //
  char cutcolor[9];
  parameter.cutcolor.ToNumericalString(cutcolor, 9);
  jsonObject["cclr"] = cutcolor;
  jsonObject["vc"] = parameter.vcut;
  jsonObject["hc"] = parameter.hcut;
  jsonObject["ialt"] = parameter.isalternate;
  //
  jsonObject["rpt"] = parameter.repeat;
  jsonObject["wt"] = parameter.wait;
  jsonObject["ibnc"] = parameter.isbounce;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer parameterRead()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  // Write parameter as jsonObject
  JsonObject jsonObjectParameter = jsonDoc.to<JsonObject>();
  
  // Copy PARAMETER in jsonObjectParameter
  parameterTojsonObject(PARAMETER,jsonObjectParameter);

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "application/json";
  serializeJson(jsonDoc, httpAnswer.contentData);
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleParameterRead()
{
  t_httpAnswer httpAnswer = parameterRead();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer parameterSave(String parameterPath)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // Create or open parameterPath
  File parameterFile = LittleFS.open(parameterPath, "w");

  // Read and Save parameter in parameterFile
  parameterFile.print(parameterRead().contentData);

  // Close parameterFile
  parameterFile.close();

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PARAMETER SAVE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleParameterSave()
{
  t_httpAnswer httpAnswer = parameterSave(findFileNewname(PARAMETER.bmppath,"json"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void jsonObjectToparameter(JsonObject &jsonObject, t_parameter &parameter)
{
  if (!jsonObject["bmp"].isNull()) parameter.bmppath = jsonObject["bmp"].as<String>();
  if (!jsonObject["ist"].isNull()) parameter.indexstart = jsonObject["ist"];
  if (!jsonObject["isp"].isNull()) parameter.indexstop = jsonObject["isp"];
  if (!jsonObject["imx"].isNull()) parameter.indexmax = jsonObject["imx"];
  //
  if (!jsonObject["dly"].isNull()) parameter.delay = jsonObject["dly"];
  if (!jsonObject["bts"].isNull()) parameter.brightness = jsonObject["bts"];
  if (!jsonObject["iivt"].isNull()) parameter.isinvert = jsonObject["iivt"];
  //
  if (!jsonObject["eclr"].isNull()) parameter.endcolor.Parse<HtmlShortColorNames>(jsonObject["eclr"].as<String>());
  if (!jsonObject["iedo"].isNull()) parameter.isendoff = jsonObject["iedo"];
  if (!jsonObject["iedc"].isNull()) parameter.isendcolor = jsonObject["iedc"];
  //
  if (!jsonObject["cclr"].isNull()) parameter.cutcolor.Parse<HtmlShortColorNames>(jsonObject["cclr"].as<String>());
  if (!jsonObject["vc"].isNull()) parameter.vcut = jsonObject["vc"];
  if (!jsonObject["hc"].isNull()) parameter.hcut = jsonObject["hc"];
  if (!jsonObject["ialt"].isNull()) parameter.isalternate = jsonObject["ialt"];
  //
  if (!jsonObject["rpt"].isNull()) parameter.repeat = jsonObject["rpt"];
  if (!jsonObject["wt"].isNull()) parameter.wait = jsonObject["wt"];
  if (!jsonObject["ibnc"].isNull()) parameter.isbounce = jsonObject["ibnc"];
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer parameterWrite(String stringParameter)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // New json document
  JsonDocument jsonDoc;

  // Json not right : throw error
  if (deserializeJson(jsonDoc, stringParameter))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PARAMETER WRITE ERROR : WRONG JSON";
    return httpAnswer;
  }

  // Running or paused animation : throw error
  if (ANIMATIONS.IsAnimationActive(0) || ANIMATIONS.IsPaused())
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 403;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PARAMETER WRITE ERROR : NOT AVAILABLE";
    return httpAnswer;
  }

   // Store parameter in jsonObject
  JsonObject jsonObjectParameter = jsonDoc.as<JsonObject>();
  
  // New bitmap : test it
  if (jsonObjectParameter["bmp"].as<String>() != PARAMETER.bmppath)
  {
    // Bitmap not valid : throw error
    if (!NEOBMPFILE.Begin(LittleFS.open(jsonObjectParameter["bmp"].as<String>(), "r")))
    {
      // Build httpAnswer and return it
      httpAnswer.statusCode = 500;
      httpAnswer.contentType = "text/plain";
      httpAnswer.contentData = "PARAMETER WRITE ERROR : WRONG BITMAP"+jsonObjectParameter["bmp"].as<String>();
      return httpAnswer;
    }
    
    // New bitmap valid : change indexstart ; indexstop and indexmax
    jsonObjectParameter["ist"] = 0;
    jsonObjectParameter["isp"]= NEOBMPFILE.Height() - 1;
    jsonObjectParameter["imx"] = NEOBMPFILE.Height() - 1;
  }

  // Copy jsonObjectParameter in PARAMETER
  jsonObjectToparameter(jsonObjectParameter, PARAMETER);

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PARAMETER WRITE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleParameterWrite()
{
  t_httpAnswer httpAnswer = parameterWrite(server.arg("plain"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer parameterRestore(String parameterPath)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // check if file at parameterPath exists
  if (!LittleFS.exists(parameterPath))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 404;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "RESTORE ERROR : PARAMETERFILE NOT FOUND";
    return httpAnswer;
  }
  
  // Open parameterPath
  File parameterFile = LittleFS.open(parameterPath, "r");

  // Read parameterFile
  String parameterString;
  while (parameterFile.available()) parameterString += char(parameterFile.read());
  
  // Close parameterFile
  parameterFile.close();

  // Write parameter
  return parameterWrite(parameterString);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleParameterRestore()
{
  t_httpAnswer httpAnswer = parameterRestore(server.arg("plain"));
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer parameterDefault(t_parameter &parameter)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // set parameter to default value
  parameter = PARAMETERDEFAULT;
  
  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PARAMETER DEFAULT SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleParameterDefault()
{
  t_httpAnswer httpAnswer = parameterDefault(PARAMETER);
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer fileDelete()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // parse file from request
  String path = server.arg("plain");

  // check if the file is a system files
  if (path == "" || path == "/" || path == "index.html" || path == "css" || path == "js" || path == "title.png")
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "DELETE ERROR : SYSTEM FILE";
    return httpAnswer;
  }

  // check if the file exists
  if (!LittleFS.exists(path))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 404;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "DELETE ERROR : FILE NOT FOUND";
    return httpAnswer;
  }

  // check if the file is the current bitmap
  if (path == PARAMETER.bmppath)
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "DELETE ERROR : CURRENT BITMAP";
    return httpAnswer;
  }

  // Delete the file
  LittleFS.remove(path);

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "DELETE SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleFileDelete()
{
  t_httpAnswer httpAnswer = fileDelete();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleFileRead(String path)
{
  // Serve index file when top root path is accessed
  if (path.endsWith("/")) path += "index.html";

  // Check if the file exists
  if (!LittleFS.exists(path)) return server.send(404, "text/plain", "READ ERROR : FILE NOT FOUND");

  // Open the file
  fs::File file = LittleFS.open(path, "r");

  // Display the file on the client's browser
  server.streamFile(file, getContentType(path));

  // Close the file
  file.close();
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleFileUpload()
{
  //
  HTTPUpload& upload = server.upload();

  // Upload start
  if (upload.status == UPLOAD_FILE_START)
  {
    // Reject file too big
    FSInfo fs_info;
    LittleFS.info(fs_info);
    if (upload.contentLength > (fs_info.totalBytes-fs_info.usedBytes)) return;
    
    // Retrieve and correct filname
    String filename =  findFileNewname(getFileBasename(upload.filename), getFileExtension(upload.filename)); //String filename = upload.filename;
    //if (!filename.startsWith("/")) filename = "/" + filename; //not usefull for littlefs ???
    
    // Open the file for writing in LittleFS (create if it doesn't exist)
    UPLOADFILE = LittleFS.open(filename, "w");
  }

  // Upload in progress
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    //Write the received bytes to the file
    if (UPLOADFILE) UPLOADFILE.write(upload.buf, upload.currentSize);
  }

  // Upload end
  else if (upload.status == UPLOAD_FILE_END)
  {
    //Close the file
    if (UPLOADFILE) UPLOADFILE.close();
  }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer systemRead()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // Assuming there are no subdirectories
  fs::Dir dir = LittleFS.openDir("/");

  // New json document
  JsonDocument jsonDoc;
  
  // Store ledInfo in json nested object
  JsonObject ledInfo = jsonDoc["ldi"].to<JsonObject>();
  ledInfo["pxs"] = NUMPIXELS;

  // Store fsInfo in json nested object
  JsonObject fsInfo = jsonDoc["fsi"].to<JsonObject>();
  FSInfo fs_info;
  LittleFS.info(fs_info);
  fsInfo["ubs"] = fs_info.usedBytes;
  fsInfo["tbs"] = fs_info.totalBytes;
  fsInfo["fbs"] = fs_info.totalBytes-fs_info.usedBytes;

  // Store fileList in json nested object
  JsonObject fileList = jsonDoc["flt"].to<JsonObject>();
  while (dir.next())
  {
    if(dir.isFile())
    {
      // store file parameter in json nested object
      JsonObject file  = fileList[dir.fileName()].to<JsonObject>();
      file["fsz"] = dir.fileSize();
    }
  }

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "application/json";
  serializeJson(jsonDoc, httpAnswer.contentData);
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleSystemRead()
{
  t_httpAnswer httpAnswer = systemRead();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void clearToSHADER(HtmlColor color, uint8_t hcutcounter)
{
  // 
  HCUTCOUNTER = hcutcounter;
  
  // Apply color through SHADER to each pixel of the strip
  for (uint16_t index=0; index<NUMPIXELS; index++) STRIP.SetPixelColor(index,SHADER.Apply(index, color));
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void renderSHADER(uint16_t index, uint8_t hcutcounter)
{
  // 
  HCUTCOUNTER = hcutcounter;
  
  // Apply bitmap at index through SHADER to each pixel of the strip
  NEOBMPFILE.Render<BrightShader>(STRIP, SHADER, 0, 0, index, NEOBMPFILE.Width());
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer playAnimation()
{
  // New httpAnswer
  t_httpAnswer httpAnswer;

  // Animation is paused : resume it
  if (ANIMATIONS.IsPaused())
  {
    // Resume animation
    ANIMATIONS.Resume();

    // Build httpAnswer and return it
    httpAnswer.statusCode = 200;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "RESUME SUCCESS";
    return httpAnswer;
  }
  
  // Animation is active : pause it
  if (ANIMATIONS.IsAnimationActive(0))
  {
    // Pause animation
    ANIMATIONS.Pause();

    // Blank the strip if needed
    if (PARAMETERTEMP.isendoff) STRIP.ClearTo(HtmlColor(0));
    if (PARAMETERTEMP.isendcolor) clearToSHADER(PARAMETERTEMP.endcolor, PARAMETERTEMP.hcut);

    // Build httpAnswer and return it
    httpAnswer.statusCode = 200;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PAUSE SUCCESS";
    return httpAnswer;
  }
  
  // No animation : launch new animation
  // Load parameter from PLAYLIST[0] or from PARAMETER
  if (ACTION.isplaylist)
  {
    if (PLAYLISTSIZE <= 0)
    {
      // Build httpAnswer and return it
      httpAnswer.statusCode = 500;
      httpAnswer.contentType = "text/plain";
      httpAnswer.contentData = "PLAY ERROR : EMPTY PLAYLIST";
      return httpAnswer;
    }
    
    PARAMETERTEMP = PLAYLIST[0];
  }
  else
  {
    PARAMETERTEMP = PARAMETER;
  }

  // Load bmp from PARAMETERTEMP
  if(!NEOBMPFILE.Begin(LittleFS.open(PARAMETERTEMP.bmppath, "r")))
  {
    // Build httpAnswer and return it
    httpAnswer.statusCode = 500;
    httpAnswer.contentType = "text/plain";
    httpAnswer.contentData = "PLAY ERROR : WRONG BITMAP";
    return httpAnswer;
  }
  
  // Playlist counter initialization
  PLAYLISTCOUNTER = 0;

  // Repeat counter initialization
  REPEATCOUNTER = PARAMETERTEMP.repeat;
  
  // Wait counter initialization
  WAITCOUNTER = PARAMETERTEMP.wait;
  
  // Vertical cut counter initialization
  VCUTCOUNTER = PARAMETERTEMP.vcut;
  
  // Index initialization
  if (PARAMETERTEMP.isinvert) INDEXCOUNTER = PARAMETERTEMP.indexstop;
  else INDEXCOUNTER = PARAMETERTEMP.indexstart;

#ifdef DEBUG
  ANIMATIONDURATION = millis();
#endif
  
  // Launch a new animation
  ANIMATIONS.StartAnimation(0, PARAMETERTEMP.delay, updateAnimation);
  
  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = "PLAY SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handlePlay()
{
  t_httpAnswer httpAnswer = playAnimation();
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
t_httpAnswer stopAnimation(String action)
{
  // New httpAnswer
  t_httpAnswer httpAnswer;
  
  // Stop animation
  ANIMATIONS.StopAnimation(0);
  ANIMATIONS.Resume(); // remove the pause flag to stop paused animation

  // Turn the strip to blank
  if (action == "STOP") STRIP.ClearTo(HtmlColor(0));

  // Turn the strip to COLOR
  if (action == "LIGHT")
  { 
    // Load PARAMETERTEMP from PLAYLIST[0] or from PARAMETER
    if (ACTION.isplaylist)
    {
      if (PLAYLISTSIZE <= 0)
      {
        // Build httpAnswer and return it
        httpAnswer.statusCode = 500;
        httpAnswer.contentType = "text/plain";
        httpAnswer.contentData = "LIGHT ERROR : EMPTY PLAYLIST";
        return httpAnswer;
      }
      
      PARAMETERTEMP = PLAYLIST[0];
    }
    else
    {
      PARAMETERTEMP = PARAMETER;
    }
    
    // Render the color from PARAMETERTEMP
    clearToSHADER(PARAMETERTEMP.endcolor, PARAMETERTEMP.hcut);
  }

  // Turn the strip to the first column of BITMAP
  if (action == "BURN")
  {
    // Load PARAMETERTEMP from PLAYLIST[0] or from PARAMETER
    if (ACTION.isplaylist)
    {
      if (PLAYLISTSIZE <= 0)
      {
        // Build httpAnswer and return it
        httpAnswer.statusCode = 500;
        httpAnswer.contentType = "text/plain";
        httpAnswer.contentData = "BURN ERROR : EMPTY PLAYLIST";
        return httpAnswer;
      }
      
      PARAMETERTEMP = PLAYLIST[0];
    }
    else
    {
      PARAMETERTEMP = PARAMETER;
    }

    // Load bmp from PARAMETERTEMP
    if(!NEOBMPFILE.Begin(LittleFS.open(PARAMETERTEMP.bmppath, "r")))
    {
      // Build httpAnswer and return it
      httpAnswer.statusCode = 500;
      httpAnswer.contentType = "text/plain";
      httpAnswer.contentData = "BURN ERROR : WRONG BITMAP";
      return httpAnswer;
    }
    
    // Render the first(normal) or the last(invert) line of bmp from PARAMETERTEMP
    if (PARAMETERTEMP.isinvert) renderSHADER(PARAMETERTEMP.indexstop, PARAMETERTEMP.hcut);
    else renderSHADER(PARAMETERTEMP.indexstart, PARAMETERTEMP.hcut);
  }

  // Build httpAnswer and return it
  httpAnswer.statusCode = 200;
  httpAnswer.contentType = "text/plain";
  httpAnswer.contentData = action + " SUCCESS";
  return httpAnswer;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleStop()
{
  t_httpAnswer httpAnswer = stopAnimation("STOP");
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleLight()
{
  t_httpAnswer httpAnswer = stopAnimation("LIGHT");
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void handleBurn()
{
  t_httpAnswer httpAnswer = stopAnimation("BURN");
  server.send(httpAnswer.statusCode, httpAnswer.contentType, httpAnswer.contentData);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void updateAnimation(const AnimationParam & param)
{
  // Wait for this animation to complete,
  if (param.state == AnimationState_Completed)
  {
    // INDEXCOUNTER is in the limit
    if ((PARAMETERTEMP.indexstart <= INDEXCOUNTER) && (INDEXCOUNTER <= PARAMETERTEMP.indexstop))
    {
      // Restart the animation
      ANIMATIONS.RestartAnimation(param.index);

      // Vertical cut counter incrementation
      if (VCUTCOUNTER > 1) VCUTCOUNTER -= 1;
      else VCUTCOUNTER = 2*PARAMETERTEMP.vcut;

      // Blank or color the strip during the vertical cut
      if (VCUTCOUNTER <= PARAMETERTEMP.vcut) renderSHADER(INDEXCOUNTER, PARAMETERTEMP.hcut);
      else if (PARAMETERTEMP.isalternate) renderSHADER(INDEXCOUNTER, 2*PARAMETERTEMP.hcut);
      else clearToSHADER(PARAMETERTEMP.cutcolor, PARAMETERTEMP.hcut);

      // Index incrementation
      if (PARAMETERTEMP.isinvert) INDEXCOUNTER -= 1;
      else INDEXCOUNTER += 1; 
    }

    // Repeat or bounce to do
    else if (REPEATCOUNTER > 0)
    {
      // Restart the animation
      ANIMATIONS.RestartAnimation(param.index);

      // Wait to do
      if (WAITCOUNTER > 0)
      {
        // Wait counter incrementation
        WAITCOUNTER -= 1;
          
        // Blank or color the strip during the wait
        if (PARAMETERTEMP.isendoff) STRIP.ClearTo(HtmlColor(0));
        if (PARAMETERTEMP.isendcolor) clearToSHADER(PARAMETERTEMP.endcolor, PARAMETERTEMP.hcut);
      }
      // No wait to do? so let's repeat
      else
      {
        // Repeat counter incrementation
        REPEATCOUNTER -= 1;

        // Wait counter initialization
        WAITCOUNTER = PARAMETERTEMP.wait;
          
        //invert invertTemp to bounce
        if (PARAMETERTEMP.isbounce) PARAMETERTEMP.isinvert = !PARAMETERTEMP.isinvert;
  
        // Index initialization
        if (PARAMETERTEMP.isinvert) INDEXCOUNTER = PARAMETERTEMP.indexstop;
        else INDEXCOUNTER = PARAMETERTEMP.indexstart;
      }
    }
      
    // Playlist to do
    else if (ACTION.isplaylist && (PLAYLISTCOUNTER < PLAYLISTSIZE-1))
    {
      // Restart the animation
      ANIMATIONS.RestartAnimation(param.index);
   
      // Blank or color the strip if needed
      if (PARAMETERTEMP.isendoff) STRIP.ClearTo(HtmlColor(0));
      if (PARAMETERTEMP.isendcolor) clearToSHADER(PARAMETERTEMP.endcolor, PARAMETERTEMP.hcut);

      // Playlist counter incrementation
      PLAYLISTCOUNTER += 1;

      // Load new parameter from playlist
      PARAMETERTEMP = PLAYLIST[PLAYLISTCOUNTER];

      // Load new bmp from PARAMETERTEMP
      NEOBMPFILE.Begin(LittleFS.open(PARAMETERTEMP.bmppath, "r"));
        
      // Repeat counter initialization
      REPEATCOUNTER = PARAMETERTEMP.repeat;

      // Wait counter initialization
      WAITCOUNTER = PARAMETERTEMP.wait;
        
      // Vertical cut counter initialization
      VCUTCOUNTER = PARAMETERTEMP.vcut;
        
      // Index initialization
      if (PARAMETERTEMP.isinvert) INDEXCOUNTER = PARAMETERTEMP.indexstop;
      else INDEXCOUNTER = PARAMETERTEMP.indexstart;
        
      // Change animation delay
      ANIMATIONS.ChangeAnimationDuration(0, PARAMETERTEMP.delay);

      // Pause the new animation if playlist trigger by play
      if (ACTION.istrigger) ANIMATIONS.Pause();
    }
      
    // Nothing more to do
    else
    {
      // Stop the animation
      ANIMATIONS.StopAnimation(param.index);
        
      // Blank or color the strip if needed
      if (PARAMETERTEMP.isendoff) STRIP.ClearTo(HtmlColor(0));
      if (PARAMETERTEMP.isendcolor) clearToSHADER(PARAMETERTEMP.endcolor, PARAMETERTEMP.hcut);
        
#ifdef DEBUG
      Serial.print("Animation duration :");
      Serial.println(millis() - ANIMATIONDURATION);
#endif
    }
  }
}
