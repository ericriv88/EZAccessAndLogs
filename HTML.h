#define HTML_active "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
  </header>\
  <nav>\
    <ul>\
      <li><a href=\"/H\">Make blue tag valid</a></li>\
      <li><a href=\"/L\">Make white tag valid</a></li>\
    </ul>\
  </nav>\
  <section>\
    <p><a href=\"/S\">Logout</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"




#define HTML_inactiveA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
  <style>\
    body {\
      display: flex;\
      flex-direction: column;\
      align-items: center;\
      justify-content: center;\
      height: 100vh;\
      margin: 0;\
      padding: 0;\
      font-family: Arial, sans-serif;\
    }\
    form {\
      max-width: 300px;\
      width: 100%;\
      padding: 20px;\
      border: 1px solid #ccc;\
      border-radius: 4px;\
      background-color: #f5f5f5;\
    }\
    h1, p, label, input, button {\
      text-align: center;\
    }\
    footer {\
      margin-top: 20px;\
      text-align: center;\
    }\
  </style>\
</head>\
<body>\
  <h1>EZ Access and Logs</h1>\
  <form action=\"http://" //cuts off and inserts ip address here in main program

#define HTML_inactiveB "\" method=\"post\">\
    <hr>\
    <p>\
      <label for=\"userName\">User Name:</label>\
      <input type=\"text\" name=\"userName\" id=\"userName\" placeholder=\"User\" autocomplete=\"on\" required autofocus>\
    </p>\
    <p>\
      <label for=\"password\">Password:</label>\
      <input type=\"password\" name=\"password\" id=\"password\" placeholder=\"Secret\" required>\
    </p>\
    <br>\
    <button type=\"submit\">Submit</button>\
  </form>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"


