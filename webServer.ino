void webServer(){
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        // send rest of HTTP header
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        setValues();
                        // send XML file containing input states
                        XML_response(client);
                    }
                    else {  // web page request
                        // send rest of HTTP header
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");
                        client.println();
                        // send web page
                        webFile = SD.open("arduino.htm");        // open web page file
                        if (webFile) {
                            while(webFile.available()) {
                                client.write(webFile.read()); // send web page to client
                            }
                            webFile.close();
                        }
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}


// checks if received HTTP request is switching on/off LEDs
// also saves the state of the LEDs
void setValues(void)
{
    // LED 1 (pin 6)
    if (StrContains(HTTP_req, "LED1=1")) {
        LED_state[0] = 1;  // save LED state
        digitalWrite(6, HIGH);
    }else if (StrContains(HTTP_req, "LED1=0")) {
        LED_state[0] = 0;  // save LED state
        digitalWrite(6, LOW);
    }
    // LED 2 (pin 8)
    if (StrContains(HTTP_req, "LED2=1")) {
        LED_state[1] = 1;  // save LED state
        digitalWrite(8, HIGH);
    }else if (StrContains(HTTP_req, "LED2=0")) {
        LED_state[1] = 0;  // save LED state
        digitalWrite(8, LOW);
    }
    // LED 3 intensidad
    if (StrContains(HTTP_req, "MIN")) {
        analogWrite(3, HIGH);
    }else if (StrContains(HTTP_req, "MAX")) {
        analogWrite(5, LOW);
    }
}

// send the XML file with analog values, switch status
//  and LED status
void XML_response(EthernetClient cl)
{
    int analog_val;            // stores value read from analog input
    analog_val = digitalRead(7);
    Serial.print(analog_val);
    
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    
    // sensores de movimiento
    cl.println("<movimiento>");
    cl.print(analog_val);
    cl.println("</movimiento>");
    
    //sensores de temperatura
    cl.print("<temperatura>");
    cl.print(analog_val);
    cl.println("</temperatura>");
    
    cl.print("<temperatura>");
    cl.print(analog_val+1);
    cl.println("</temperatura>");
    
    //rangos de temperatura
    cl.print("<rangos>");
      cl.print(analog_val+2);
    cl.println("</rangos>");
    cl.print("<rangos>");
      cl.print(analog_val+3);
    cl.println("</rangos>");
    
    //lecturas energia
    cl.print("<lecturas>");
      cl.print(analog_val+4);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+5);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+6);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+7);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+8);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+9);
    cl.println("</lecturas>");
    
    cl.print("<lecturas>");
      cl.print(analog_val+10);
    cl.println("</lecturas>");
    
    // sensores de humedad
    cl.print("<humedad>");
    cl.print(analog_val+11);
    cl.println("</humedad>");
    
    // checkbox LED states
    // LED1
    cl.print("<check>");
    if (LED_state[0]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
    cl.println("</check>");
    // LED2
    cl.print("<check>");
    if (LED_state[1]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
     cl.println("</check>");    
    cl.print("</inputs>");
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
