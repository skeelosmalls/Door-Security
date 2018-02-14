void WebConfig() {
//  EthernetClient client = ethClient.available();
   if(ethClient.available()) {
     boolean currentLineIsBlank = true;
     boolean currentLineIsGet = true;
     int tCount = 0;
     char tBuf[64];
     int rate;
     char *pch;
     
     Serial.print("Client request: ");
     
     while (ethClient.connected()) {
       while(ethClient.available()) {
         char c = ethClient.read();
  
         if(currentLineIsGet && tCount < 63)
         {
           tBuf[tCount] = c;
           tCount++;
           tBuf[tCount] = 0;          
         }
  
         if (c == '\n' && currentLineIsBlank) {
  
           while(ethClient.available()) ethClient.read();
           Serial.println(tBuf);
           pch = strtok(tBuf,"?");
  
           while(pch != NULL)
           {
             if(strcmp(pch,"mode=blink") == 0)
             {
//                 Serial.println("mode=blink");
                 // do mode = blink stuff
             }
             else if(strcmp(pch,"mode=steady") == 0)
             {
//                 Serial.println("mode=steady");
                 // do mode = steady stuff
             }
             else if(strcmp(pch,"color=yellow") == 0)
             {
//                 Serial.println("color=yellow");
                 // do color=yellow stuff
             }
//             else if(strcmp(pch,"color=red") == 0)
//             {
//                 Serial.println("color=red");
//                 // do color=red stuff
//             }
//             else if(strncmp(pch,"rate=",5) == 0)
//             {
//               rate = atoi(pch+5);
//               Serial.print("rate = ");              
//               Serial.println(rate,DEC);
//             }
  
             pch = strtok(NULL,"& ");
           }
  
           Serial.println("Sending response");
           ethClient.write("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><H1>TEST</H1>");
  
           ethClient.write("<form method=GET>Mode: <input type=text name=mode><br>");
           ethClient.write("Rate: <input type=text name=rate><br>");
           ethClient.write("Color: <input type=text name=color><br><input type=submit></form>");
           
           ethClient.write("</body></html>\r\n\r\n");
           ethClient.stop();
         }
         else if (c == '\n') {
           currentLineIsBlank = true;
           currentLineIsGet = false;
         } 
         else if (c != '\r') {
           currentLineIsBlank = false;
         }
       }
     }
     Serial.println("done");
   }
}
