////////////////////////////////////////////////////////////////////////////
//**********************WiFi Functions***********************************///
////////////////////////////////////////////////////////////////////////////

char def_ssid[] = secret_ssid;        //network SSID from secrets
char def_pass[] = secret_pass;       //network password from secrets
char def_user[] = secret_user;       //user name for enterprise from secrets
int keyIndex = 0;                   //network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;        //connection status

String cardUID;
String cardPIN;

IPAddress savedIP = IPAddress(0,0,0,0); //initialize IP varriable for securing web connections

bool IPConnect = false;             //indicates when authorized client connected to server
bool CredChange = false;            //indicates if credential change is occuring
bool CardRegister = false;          //indicates that a card is being registered
bool ReaderRegister = false;        //indicates that a reader is being registered
bool CarDel = false;                //indicates card deletion mode
bool CardManage = false;            //indicates card management mode
bool UserManage = false;            //indicates user management mode
bool LogAccess = false;
bool DupeName = false;              //used to display different HTML when duplicate nickname entered at card register
bool DupeUser = false;              //used to display different HTML when duplicate credentials entered at user register
bool NewUser = false;               //indicates new user register function
bool Logout = false;                //indicates logout

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
  status = WL_IDLE_STATUS;
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

void printWEB(WiFiClient client, bool* IPSetup, LiquidCrystal_I2C lcd, IPAddress ip, MFRC522 mfrc522, bool* BLESetup) {

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
              if(!CredChange && !CardRegister && !CarDel && !LogAccess && !NewUser && !CardManage) {
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
                else if (checkSDForString("USERS.txt", toHash(postBody))) {
                  savedIP = client.remoteIP();
                  LogAccess = true;    //indicates log access request
                  client.print(HTML_LogAccessA);   //send log access HTML code
                  for(int i = 1; i <= SDLineCount("LOGS.txt"); i++) {    //which requires all logs to be printed as list items 
                    client.print("<li>");
                    client.print(readSDLine("LOGS.txt", i));
                    client.print("</li>");
                  }
                  client.print(HTML_UserLogAccessB);
                }
              }
              else if(CredChange){      //indicates a credential change
                overWriteSDHash("LOGIN.txt", postBody); //overwrite hashed credential in SD
                CredChange = false;
              }
              else if(NewUser) {      //indicates new user credientials
                String nickName;
                int i = 9;
                while(postBody[i] != '&') {    //get only the name from the postBody
                  nickName += postBody[i];
                  i++;
                }
                if(!checkSDForString("LIST.txt", nickName) && !checkSDForString("LOGIN.txt", toHash(postBody))) { //only add user if it does not conflict with other users and admin
                  writeSDLine("LIST.txt", nickName);                                                              //credentials
                  writeSDHashLine("USERS.txt", postBody);
                  NewUser = false;
                  DupeUser = false;
                }
                else DupeUser = true;
              }
              else if(CardRegister) {        //indicates card register
                String nickName;
                String keypadPin;
                int position = postBody.indexOf('&'); 
              
                for(int i = 5; i < position; i++) {    //get only the nickname from the postBody
                    nickName += postBody[i];
                  }
                for (int i = position + 5; i < position + 10; i++) {    //get only the pin from the postBody
                    keypadPin += postBody[i];
                  }
                String pinHash = toHash(keypadPin);

                int position2 = postBody.indexOf('&', position + 1);

                if(!checkSDForString("NAME.txt",nickName)) {     //if nickName is not a repeat
                  writeSDLine("NAME.txt", nickName);  //write nickname to SD
                  writeSDLine("KEYPAD.txt", pinHash);
                  CardRegister = false;
                  DupeName = false;
                  if(position2 != -1) { //if boxes were checked
                    String postBoxes = postBody;
                    postBoxes.remove(0, (position2));
                    String RCount = readSDLine("RCOUNT.txt", 1);
                    int ReaderCount = RCount.toInt();
                    for (int i = 0; i <= ReaderCount; i++) {  //check if any of the readers from reader count were checked starting from 0
                      int found = postBoxes.indexOf(String(i));
                      if(found != -1){
                       newCardRegister(cardUID, cardPIN, lcd, i); //Register new card to reader i
                      }
                    }
                  }
                }
                else DupeName = true;
              }
              else if(CardManage) {
                CardManage = false;
                String readerCount = readSDLine("RCOUNT.txt", 1); //wipe UID files for rewrite
                for(int i = 0; i <= readerCount.toInt(); i++) {
                  String fileName = "READERS/UID" + String(i) + ".txt";
                  wipeSDFile(fileName);
                }
                for(int i = 1; i <= SDLineCount("NAME.txt"); i++) { //for each registered card, write to UID file if box is checked
                  String name = readSDLine("NAME.txt", i);
                  name = name + "=";
                  String UIDHash = readSDLine("UID.txt", i);
                  String KEYPADHash = readSDLine("KEYPAD.txt", i);
                  int index = postBody.indexOf(name);
                  while(index != -1) {
                    int ampIndex = postBody.indexOf("&", index);
                    String readerNum = postBody.substring((index + name.length()), ampIndex); //get String representation of reader number
                    String fileName = "READERS/UID" + readerNum + ".txt"; //write UID to file
                    writeSDLine(fileName, UIDHash);
                    String fileName1 = "READERS/KEYPAD" + readerNum + ".txt"; //write keypad pin to file
                    writeSDLine(fileName1, KEYPADHash);
                    index = postBody.indexOf(name, ampIndex);
                  }
                }
              }
            }

            //create the buttons
            if (IPConnect) {  //if the system is activated send the right HTML code
              if (CredChange) {
                client.print(HTML_CredChangeA);   //send credential change HTML code
                client.print(ip);               //which requires ip address
                client.print(HTML_CredChangeB);
              }
              else if (NewUser) {
                client.print(HTML_NewUserA);
                client.print(ip);
                if(!DupeUser) client.print(HTML_NewUserB);
                else client.print(HTML_NewUserDupeB);
              }
              else if (CardRegister) {
                client.print(HTML_CardRegisterA);   //send card register HTML code
                client.print(ip);                  //which requires ip address
                client.print(HTML_CardRegisterB);
                client.print("<input type=\"checkbox\" id=\"Reader0\" name=\"Reader0\" value=\"0\">");  //create check box for hub module
                client.print("<label for=\"Reader0\"> Hub Reader</label><br>");
                //create check boxes for any other registered readers
                String RCount = readSDLine("RCOUNT.txt", 1);
                int ReaderCount = RCount.toInt();
                for(int i = 1; i <= ReaderCount; i++){
                  client.print("<input type=\"checkbox\" id=\"Reader");
                  client.print(String(i));
                  client.print("\" name=\"Reader");
                  client.print(String(i));
                  client.print("\" value=\"");
                  client.print(String(i));
                  client.print("\">");  //create check box for hub module
                  client.print("<label for=\"Reader");
                  client.print(String(i));
                  client.print("\"> Reader ");
                  client.print(String(i));
                  client.print("</label><br>");
                }
                if(!DupeName) client.print(HTML_CardRegisterC);
                else client.print(HTML_CardRegisterDupeC);
              }
              else if (CarDel) {
                client.print(HTML_CarDelA);   //send card manage HTML code
                for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {    //which requires all nicknames be printed between as anchors giving option to delete
                client.print("<li><a href=\"/CD");
                client.print(i);
                client.print("\">Delete ");
                client.print(readSDLine("NAME.txt", i));
                client.print("</a></li>");
                }
                client.print(HTML_CarDelB);
              }
              else if(CardManage) {
                client.print(HTML_CardManageA);
                client.print(ip);
                client.print("\" method=\"post\">");

                for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {
                  client.print("<p>");
                  String name = readSDLine("NAME.txt", i);
                  String UIDHash = readSDLine("UID.txt", i);
                  client.print(name);
                  client.print(": ");
                  client.print("<input type=\"checkbox\" id=\"");
                  client.print(name);
                  client.print("\" name=\"");
                  client.print(name);
                  client.print("\" value=\"0\"");  //create check box for hub module
                  if(checkSDForString("READERS/UID0.txt", UIDHash)) client.print(" checked>");  //checkbox is checked if already registered to hub
                  else client.print(">");
                  client.print("<label for=\"");
                  client.print(name);
                  client.print("\"> Reader Hub  </label>");
                  String readerCount = readSDLine("RCOUNT.txt", 1);
                  for(int j = 1; j <= readerCount.toInt(); j++) {
                    String FileName = "READERS/UID" + String(j) + ".txt";
                    client.print("<input type=\"checkbox\" id=\"");
                    client.print(name);
                    client.print("\" name=\"");
                    client.print(name);
                    client.print("\" value=\"");
                    client.print(String(j));
                    client.print("\"");  //create check box for reader module
                    if(checkSDForString(FileName, UIDHash)) client.print(" checked>");  //checkbox is checked if already registered to reader
                    else client.print(">");
                    client.print("<label for=\"");
                    client.print(name);
                    client.print("\"> Reader ");
                    client.print(String(j));
                    client.print("  ");
                    client.print("</label>");
                  }
                  client.print("<br>");
                }

                client.print(HTML_CardManageB);
              }
              else if (ReaderRegister) {
                client.print(HTML_ReaderRegisterA);
                client.print(readSDLine("RCOUNT.txt", 1));
                client.print(HTML_ReaderRegisterB);
              }
              else if (UserManage) {
                client.print(HTML_UserManageA);   //send card manage HTML code
                for(int i = 1; i <= SDLineCount("LIST.txt"); i++) {    //which requires all nicknames be printed between as anchors giving option to delete
                client.print("<li><a href=\"/UD");
                client.print(i);
                client.print("\">Delete ");
                client.print(readSDLine("LIST.txt", i));
                client.print("</a></li>");
                }
                client.print(HTML_UserManageB);
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

            else if(!LogAccess & !Logout) {      //if system is not activated and user not logged in 
              client.print(HTML_LoginA);   //send longin HTML code
              client.print(ip);               //which requires ip address
              client.print(HTML_LoginB);
            }

            else if(Logout) {
              Logout = false;
              *BLESetup = true;
              client.print(HTML_LogoutPage);  //send HTML of logout page
            }
            
            // The HTTP response ends with another blank line:
            client.println();
            delay(500); //delay to properly print to client
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
          cardUID = newCardRead(mfrc522, lcd, &CardRegister);  //Read new card
        }
        if (currentLine.endsWith("GET /ReaderAdd") && IPConnect) {
          BLE_Peripheral_Add_Reader();
          ReaderRegister = true;     
        }
        if (currentLine.endsWith("GET /CarDel") && IPConnect) {
          CarDel = true;    //indicates card delete request
        }
        if (currentLine.endsWith("GET /CardManage") && IPConnect) {
          CardManage = true;    //indicates card management request
        }
        if (currentLine.endsWith("GET /UserMan") && IPConnect) {
          UserManage = true;    //indicates user management request
        }
        if (currentLine.endsWith("GET /Logs") && IPConnect) {
          LogAccess = true;    //indicates log access request
        }
        if (currentLine.endsWith("GET /Delete") && IPConnect) {
          String readerCount = readSDLine("RCOUNT.txt", 1);
          wipeSDFile("NAME.txt");    //wipe nickname, UID, and keypad files
          wipeSDFile("UID.txt");
          wipeSDFile("KEYPAD.txt");
          for(int i = 0; i <= readerCount.toInt(); i++) {
            String fileName = "READERS/UID" + String(i) + ".txt";
            wipeSDFile(fileName);
          }   
        }
        if (currentLine.endsWith("GET /UserD") && IPConnect) {
          wipeSDFile("LIST.txt");  //Delete all usernames and user hashes from the SD  
          wipeSDFile("USERS.txt");   
        }
        if (currentLine.endsWith("GET /LDelete") && IPConnect) {
          wipeSDFile("LOGS.txt"); //Delete all Logs from SD
        }
        if (currentLine.endsWith("GET /Reset") && IPConnect) {
          overWriteSDBool("SET.txt", false); //Reset all settings and logout
          overWriteSDHash("LOGIN.txt", default_login);
          wipeSDFile("USERS.txt");
          wipeSDFile("LIST.txt");
          wipeSDFile("UID.txt");
          wipeSDFile("READERS/UID0.txt");
          wipeSDFile("READERS/KEYPAD0.txt");
          wipeSDFile("NAME.txt");
          wipeSDFile("LOGS.txt");
          wipeSDFile("KEYPAD.txt");
          String readerCount = readSDLine("RCOUNT.txt", 1);   //delete all reader UID files
          for(int i = 1; i <= readerCount.toInt(); i++) {
            String UIDFile = "READERS/UID" + String(i) + ".txt";
            deleteSDFile(UIDFile);
          }
          overWriteSD("RCOUNT.txt", "0");     //reset reader count
          *IPSetup = false;
          IPConnect = false;
          savedIP = IPAddress(0,0,0,0);   //clear savedIP
          Logout = true;
        }
        if (currentLine.endsWith("GET /Logout") && IPConnect) {
          IPConnect = false;    //logout so that no changes can be made to access list
          savedIP = IPAddress(0,0,0,0);   //clear savedIP
          Logout = true;
        }
        if (currentLine.endsWith("GET /Logout") && LogAccess) { //logout from a user login
          IPConnect = false;    //logout so that no changes can be made to access list
          savedIP = IPAddress(0,0,0,0);   //clear savedIP
          LogAccess = false;
          Logout = true;
        }
        if (currentLine.endsWith("GET /Change") && IPConnect) {
          CredChange = true;    //indicates credential change
        }
        if (currentLine.endsWith("GET /New") && IPConnect) {
          NewUser = true;       //indicates new user registration
        }
        if(IPConnect && CarDel) {
          for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {   //check if any of the cards are trying to be deleted
            String cardDelete = "GET /CD" + String(i);
            if(currentLine.endsWith(cardDelete)) {
              String DelUID = readSDLine("UID.txt", i);
              String DelKEYPAD = readSDLine("KEYPAD.txt", i);
              String readerCount = readSDLine("RCOUNT.txt", 1);
              deleteSDLine("NAME.txt", i);    //delete nickanme and UID from files
              deleteSDLine("UID.txt", i);
              deleteSDLine("KEYPAD.txt", i);
              for(int i = 0; i <= readerCount.toInt(); i++) {
                String fileName = "READERS/UID" + String(i) + ".txt";
                if(checkSDForString(fileName, DelUID))
                 {
                  int line = findSDStringLine(fileName, DelUID);
                  deleteSDLine(fileName, line);
                 }
                String fileName1 = "READERS/KEYPAD" + String(i) + ".txt";
                if(checkSDForString(fileName, DelKEYPAD)) 
                {
                  int line1 = findSDStringLine(fileName1, DelKEYPAD);
                  deleteSDLine(fileName1, line1);
                }
              }
            }
          }
        }
        if(IPConnect && UserManage) {
          for(int i = 1; i <= SDLineCount("LIST.txt"); i++) {   //check if any of the users are trying to be deleted
            String userDelete = "GET /UD" + String(i);
            if(currentLine.endsWith(userDelete)) {
              deleteSDLine("LIST.txt", i);    //delete user and user hash from files
              deleteSDLine("USERS.txt", i);
            }
          }
        }
        if (currentLine.endsWith("GET /Menu") && IPConnect) {   //need this for going back to main page from given pages
          CarDel = false;
          LogAccess = false;
          UserManage = false;
          ReaderRegister = false;  
          CardManage = false;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}