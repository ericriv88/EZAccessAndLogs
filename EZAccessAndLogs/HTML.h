#define HTML_HomePage "<!DOCTYPE html>\
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
      <li><a href=\"/ReaderAdd\">Register Additional Reader</a></li>\
      <li><a href=\"/Register\">Register New Card</a></li>\
      <li><a href=\"/CarDel\">Delete Registered Cards</a></li>\
      <li><a href=\"/CardManage\">Manage Registered Cards</a></li>\
      <li><a href=\"/Logs\">Access Logs</a></li>\
      <li><a href=\"/New\">Register New User</a></li>\
      <li><a href=\"/UserMan\">Manage Registered Users</a></li>\
      <li><a href=\"/Change\">Change Username and Password</a></li>\
    </ul>\
  </nav>\
  <section>\
    <p><a href=\"/Logout\">Logout</a></p>\
    <p><a href=\"/Reset\">Reset and Logout</a> - This will reset ALL settings</p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_LogoutPage "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
  </header>\
  <p>You have logged out and turned off wifi connection</p>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_ReaderRegisterA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
  </header>\
  <p>Reader "
  
  
#define HTML_ReaderRegisterB " has been added to the system</p>\
<p><a href=\"/Menu\">Main Menu</a></p>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"




#define HTML_LoginA "<!DOCTYPE html>\
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

#define HTML_LoginB "\" method=\"post\">\
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

#define HTML_CredChangeA "<!DOCTYPE html>\
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
  <h2>Update Credentials</h2>\
  <form action=\"http://" //cuts off and inserts ip address here in main program

#define HTML_CredChangeB "\" method=\"post\">\
    <hr>\
    <p>\
      <label for=\"userName\">New User Name:</label>\
      <input type=\"text\" name=\"userName\" id=\"userName\" placeholder=\"User\" autocomplete=\"off\" required autofocus>\
    </p>\
    <p>\
      <label for=\"password\">New Password:</label>\
      <input type=\"password\" name=\"password\" id=\"password\" placeholder=\"Secret\" required>\
    </p>\
    <br>\
    <button type=\"submit\">Update</button>\
  </form>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_NewUserA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
  <style>\
    .center {\
    text-align: center;\
    color: red;\
    }\
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
  <h2>Register New User</h2>\
  <form action=\"http://" //cuts off and inserts ip address here in main program

#define HTML_NewUserB "\" method=\"post\">\
    <hr>\
    <p>\
      <label for=\"userName\">New User Name:</label>\
      <input type=\"text\" name=\"userName\" id=\"userName\" placeholder=\"User\" autocomplete=\"off\" required autofocus>\
    </p>\
    <p>\
      <label for=\"password\">New Password:</label>\
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

#define HTML_NewUserDupeB "\" method=\"post\">\
    <hr>\
    <p>\
      <label for=\"userName\">New User Name:</label>\
      <input type=\"text\" name=\"userName\" id=\"userName\" placeholder=\"User\" autocomplete=\"off\" required autofocus>\
    </p>\
    <p>\
      <label for=\"password\">New Password:</label>\
      <input type=\"password\" name=\"password\" id=\"password\" placeholder=\"Secret\" required>\
    </p>\
    <br>\
    <button type=\"submit\">Submit</button>\
  </form>\
  <p class=\"center\">Invalid User Credentials</p>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_CardRegisterA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
  <style>\
    .center {\
    text-align: center;\
    color: red;\
    }\
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
  <h2>Register Card</h2>\
  <form action=\"http://" //cuts off and inserts ip address here in main program

#define HTML_CardRegisterB "\" method=\"post\">\
    <hr>\
    <p>\
      <label for=\"name\">NickName:</label>\
      <input type=\"text\" name=\"name\" id=\"name\" placeholder=\"Bob\" autocomplete=\"off\" required autofocus>\
      <br>\
      <h3>Select readers:</h3>"

#define HTML_CardRegisterC "</p>\
    <br>\
    <button type=\"submit\">Register</button>\
  </form>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_CardRegisterDupeC "</p?\
  <br>\
    <button type=\"submit\">Register</button>\
  </form>\
  <p class=\"center\">Enter Unique Nick Name</p>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_CarDelA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
    <h2>Delete Cards</h2>\
  </header>\
  <nav>\
    <ul>"
      
#define HTML_CarDelB "</ul>\
  </nav>\
  <section>\
    <p><a href=\"/Delete\">Delete All Cards</a></p>\
    <p><a href=\"/Menu\">Main Menu</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_CardManageA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
    <h2>Card Management</h2>\
  </header>\
  <form action=\"http://" //cuts off and inserts ip address here in main program
      
#define HTML_CardManageB "</p><br><br> <button type=\"submit\">Submit</button>\
  </form>\
  <section>\
    <p><a href=\"/Menu\">Main Menu</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_UserManageA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
    <h2>User Management</h2>\
  </header>\
  <nav>\
    <ul>"
      
#define HTML_UserManageB "</ul>\
  </nav>\
  <section>\
    <p><a href=\"/UserD\">Delete All Users</a></p>\
    <p><a href=\"/Menu\">Main Menu</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_LogAccessA "<!DOCTYPE html>\
<html>\
<head>\
  <title>EZ Access and Logs</title>\
</head>\
<body>\
  <header>\
    <h1>EZ Access and Logs</h1>\
    <h2>Log Access</h2>\
  </header>\
  <nav>\
    <ul>"
      
#define HTML_LogAccessB "</ul>\
  </nav>\
  <section>\
    <p><a href=\"/LDelete\">Delete All Logs</a></p>\
    <p><a href=\"/Menu\">Main Menu</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"

#define HTML_UserLogAccessB "</ul>\
  </nav>\
  <section>\
    <p><a href=\"/Logout\">Logout</a></p>\
  </section>\
  <footer>\
    <hr>\
    <p>&lt;&lt;&lt; &copy; EZ Access and Logs &gt;&gt;&gt;</p>\
  </footer>\
</body>\
</html>"


