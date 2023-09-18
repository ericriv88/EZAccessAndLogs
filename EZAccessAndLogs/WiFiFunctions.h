////////////////////////////////////////////////////////////////////////////
//**********************WiFi Functions***********************************///
////////////////////////////////////////////////////////////////////////////

char def_ssid[] = secret_ssid;        //network SSID from secrets
char def_pass[] = secret_pass;       //network password from secrets
char def_user[] = secret_user;       //user name for enterprise from secrets
int keyIndex = 0;                   //network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;        //connection status

IPAddress savedIP = IPAddress(0,0,0,0); //initialize IP varriable for securing web connections

bool IPConnect = false;             //indicates when authorized client connected to server
bool CredChange = false;            //indicates if credential change is occuring
bool CardRegister = false;          //indicates that a card is being registered
bool CardManage = false;            //indicates card management mode
bool LogAccess = false;
bool DupeName = false;              //used to display different HTML when duplicate nickname entered at card register

void printWifiStatus(IPAddress* ip) {
  //print the SSID of the network attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  //print the board's IP address:
  *ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(*ip);

  //print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(*ip);
}

void enable_WiFi(LiquidCrystal_I2C lcd) {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    lcd.clear();      //Notify with LCD of failure
    lcd.setCursor(6,0);
    lcd.print("WiFi");
    lcd.setCursor(4,1); 
    lcd.print("Failed");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi(bool Enterprise, LiquidCrystal_I2C lcd) {
  lcd.clear();          //notify connecting by LCD 
  lcd.setCursor(6,0);
  lcd.print("WiFi");
  lcd.setCursor(3,1); 
  lcd.print("Connecting");
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(def_ssid);
    // Connect to network differently depending on Enterprise connection
    if (! Enterprise) status = WiFi.begin(def_ssid, def_pass);
    else status = WiFi.beginEnterprise(def_ssid, def_user, def_pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB(WiFiClient client, bool* IPSetup, LiquidCrystal_I2C lcd, IPAddress ip, MFRC522 mfrc522) {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    Serial.print("client ip: ");           // print clients ip address
    Serial.println(client.remoteIP());

    if(savedIP != IPAddress(0,0,0,0) && savedIP != client.remoteIP()){    //if no saved IP then save one
      IPConnect = false;
      savedIP = IPAddress(0,0,0,0);   //clear savedIP
    }

    String currentLine = "";                // make a String to hold incoming data from the client
    bool postData = false;
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            //if there is post data then serial print it
            if (postData) {
              String postBody = client.readString();
              if(!CredChange && !CardRegister && !CardManage && !LogAccess) {
                if (toHash(postBody) == readSDLine("LOGIN.txt", 1)) {    //activate system if POST contents matches the credentials in SD card
                  IPConnect = true;
                  *IPSetup = true;
                  savedIP = client.remoteIP();      //save ip of logged in client
                  overWriteSDBool("SET.txt", true); //overwrite file in SD card 
                  lcd.clear();
                  lcd.setCursor(4,0); 
                  lcd.print("EZ Access");
                  lcd.setCursor(4,1);
                  lcd.print("And Logs");
                }
              }
              else if(CredChange){      //indicates a credential change
                overWriteSDHash("LOGIN.txt", postBody); //overwrite hashed credential in SD
                CredChange = false;
              }
              else {        //indicates card register
                String nickName;
                for(int i = 5; i < postBody.length(); i++) {    //get only the nickname from the postBody
                  nickName += postBody[i];
                }
                if(!checkSDForString("NAME.txt",nickName)) {     //if nickName is not a repeat
                  writeSDLine("NAME.txt", nickName);  //write nickname to SD
                  CardRegister = false;
                  DupeName = false;
                }
                else DupeName = true;
              }
            }

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            if (IPConnect) {  //if the system is activated send either credential change or home page HTML code
              if (CredChange) {
                client.print(HTML_CredChangeA);   //send credential change HTML code
                client.print(ip);               //which requires ip address
                client.print(HTML_CredChangeB);
              }
              else if (CardRegister) {
                client.print(HTML_CardRegisterA);   //send card register HTML code
                client.print(ip);                  //which requires ip address
                if(!DupeName) client.print(HTML_CardRegisterB);
                else client.print(HTML_CardRegisterDupeB);
              }
              else if (CardManage) {
                client.print(HTML_CardManageA);   //send card manage HTML code
                for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {    //which requires all nicknames be printed between as anchors giving option to delete
                client.print("<li><a href=\"/");
                client.print(i);
                client.print("\">Delete ");
                client.print(readSDLine("NAME.txt", i));
                client.print("</a></li>");
                }
                client.print(HTML_CardManageB);
              }
              else if(LogAccess) {
                client.print(HTML_LogAccessA);   //send log access HTML code
                for(int i = 1; i <= SDLineCount("LOGS.txt"); i++) {    //which requires all logs to be printed as list items 
                client.print("<li>");
                client.print(readSDLine("LOGS.txt", i));
                client.print("</li>");
                }
                client.print(HTML_LogAccessB);
              }
              else {
                client.print(HTML_HomePage);  //send HTML of home page if logged in
              }
            }

            else {      //if system is not activated
              client.print(HTML_LoginA);   //send longin HTML code
              client.print(ip);               //which requires ip address
              client.print(HTML_LoginB);
            }
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else if (currentLine.startsWith("Content-Length:")) { // Check if the request contains POST data
            postData = true; 
          }
          currentLine = "";   //clear current line
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        //Check for all anchor text presses
        if (currentLine.endsWith("GET /Register") && IPConnect) {
          newCardRegister(mfrc522, lcd, &CardRegister);  //Register new card and make it the ValidUID      
        }
        if (currentLine.endsWith("GET /Manage") && IPConnect) {
          CardManage = true;    //indicates card management request
        }
        if (currentLine.endsWith("GET /Logs") && IPConnect) {
          LogAccess = true;    //indicates log access request
        }
        if (currentLine.endsWith("GET /Delete") && IPConnect) {
          wipeSDFile("UID.txt");  //Delete all UIDs and nicknames from the SD  
          wipeSDFile("NAME.txt");   
        }
        if (currentLine.endsWith("GET /LDelete") && IPConnect) {
          wipeSDFile("LOGS.txt"); //Delete all Logs from SD
        }
        if (currentLine.endsWith("GET /Reset") && IPConnect) {
          overWriteSDBool("SET.txt", false); //Reset all settings and logout
          overWriteSDHash("LOGIN.txt", default_login);
          wipeSDFile("UID.txt");
          wipeSDFile("NAME.txt");
          wipeSDFile("LOGS.txt");
          *IPSetup = false;
          IPConnect = false;
          savedIP = IPAddress(0,0,0,0);   //clear savedIP
          lcd.clear();          //show IP address on LCD
          lcd.setCursor(0,0);
          lcd.print(ip);    
        }
        if (currentLine.endsWith("GET /Logout") && IPConnect) {
          IPConnect = false;    //logout so that no changes can be made to access list
          savedIP = IPAddress(0,0,0,0);   //clear savedIP
        }
        if (currentLine.endsWith("GET /Change") && IPConnect) {
          CredChange = true;    //indicates credential change
        }
        if(IPConnect && CardManage) {
          for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {   //check if any of the cards are trying to be deleted
            String cardDelete = "GET /" + String(i);
            if(currentLine.endsWith(cardDelete)) {
              deleteSDLine("NAME.txt", i);    //delete nickanme and UID from files
              deleteSDLine("UID.txt", i);
            }
          }
        }
        if (currentLine.endsWith("GET /Menu") && IPConnect) {   //need this for going back to main page from card management
          CardManage = false;
          LogAccess = false;     
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}