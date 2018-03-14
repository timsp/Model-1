#define READSENSOR_TIMEOUT 1500

class IncuversSerialSensor {
  private:
    SoftwareSerial* ss;
    boolean useStreaming;


  public:
    void Initialize(int pinRx, int pinTx, boolean useStreamMode) {
      this->useStreaming = useStreamMode;
      this->ss = new SoftwareSerial(pinRx, pinTx); // Rx,Tx
      this->ss->begin(9600);
    }

    String GetSerialSensorReading(int minLen, int maxLen) {
      #ifdef DEBUG_SERIAL
          Serial.print(F("Call to GetSerialSensorReading("));
          Serial.print(minLen);
          Serial.print(F(","));
          Serial.print(maxLen);
          Serial.println(F(")"));
      #endif
    
      char inChar;
      String lastString = "";                   
      String currString = "";            // string to hold incoming data from a serial sensor
      boolean stringComplete = false;    // was all data from sensor received? Check
      boolean dataReadComplete = false;
      boolean prevReadAvailable = false;
      currString.reserve(maxLen+5); 
      int i = 0;
      int j = 0;
      long escapeAfter = millis() + READSENSOR_TIMEOUT;
      
      while (!dataReadComplete) {
        while(this->ss->available() && !stringComplete) {
          inChar = (char)this->ss->read();         // grab the next char
          currString += inChar;             // add the received char to CozirString
          i++;
          if (inChar == '\r') {             // if the incoming character is a newline - the end of a record, let's verify what we have
            if (i > minLen && i < maxLen) {
              // we have a full string, but is it the last string?
              if (this->ss->available() > minLen) {
                // there's another entry of data in the queue, lets get it.
                lastString = currString; // just in case there's an error with the next bit of data
                prevReadAvailable = true;
              } else {
                stringComplete = true;
                dataReadComplete = true;
              }
              
            } else {
              // we didn't get a complete or sane string..
              if (this->ss->available() < minLen && prevReadAvailable) {
                // there isn't another record in the queue and we have a previous record, let's use that.
                currString = lastString;
                stringComplete = true;
                dataReadComplete = true;
              } else {
                if (this->ss->available() < minLen) {
                  // there isn't another record in the queue but we don't have a previous record, so return a blank value
                  currString = "";
                  stringComplete = true;
                  dataReadComplete = true;
                } else {
                  currString = "";
                  i = 0;
                }
              }
            }
          }           
        }
        
        j++;
        
        #ifdef DEBUG_SERIAL
          if (this->ss->available()) {
            Serial.print(F("OuterWhileLoop "));
            Serial.print(this->ss->available());
            Serial.println(F(" bytes available "));
          }
        #endif
        
        if(escapeAfter < millis()){
          #ifdef DEBUG_SERIAL
            Serial.print(F("Escaping after "));
            Serial.print(j);
            Serial.println(F(" checks for data."));
          #endif
          dataReadComplete=true;
        }
    
      }
      #ifdef DEBUG_SERIAL
        Serial.print(F("    Returning string: "));
        Serial.println(currString);
      #endif
      return currString;
    }



};

boolean IsNumeric(String string) {
  boolean isNum = true;
  for (int i = 0; i < string.length(); i++) {
    if (!(isDigit(string.charAt(i)) || string.charAt(i) == '+' || string.charAt(i) == '-' || string.charAt(i) == '.')) {
      isNum = false;
    }
  }
  return isNum;
}


float GetDecimalSensorReading(char index, String sensorOutput, float invalidValue) {
  #ifdef DEBUG_SERIAL
    Serial.print(F("Call to GetDecimalSensorReading('"));
    Serial.print(index);
    Serial.print(F("','"));
    Serial.print(sensorOutput);
    Serial.print(F(","));
//    Serial.print(ínvalidValue);
    Serial.println(F(")"));
  #endif
  float value = invalidValue;

  int iIndex =   sensorOutput.lastIndexOf(index);
  String shortenedString = sensorOutput.substring(iIndex+2, sensorOutput.length());
  #ifdef DEBUG_SERIAL
    Serial.print(F("    Shortened string: "));
    Serial.println(shortenedString);
  #endif
  int iSpace = shortenedString.indexOf(" ");
  String readingString = shortenedString.substring(0, iSpace);
  readingString.trim();
  #ifdef DEBUG_SERIAL
    Serial.print(F("    Reading string: "));
    Serial.println(readingString);
  #endif
  if (IsNumeric(readingString)) {
    value = readingString.toFloat();
  }

  #ifdef DEBUG_SERIAL
    Serial.print(F("    Returning value: "));
    Serial.println(value);
  #endif
  return value;
}


int GetIntegerSensorReading(char index, String sensorOutput, int invalidValue) {
  #ifdef DEBUG_SERIAL
    Serial.print(F("Call to GetIntegerSensorReading('"));
    Serial.print(index);
    Serial.print(F("','"));
    Serial.print(sensorOutput);
    Serial.print(F("',"));
//    Serial.print(ínvalidValue.toString());
    Serial.println(F(")"));
  #endif
  int value = invalidValue;

  int iIndex = sensorOutput.lastIndexOf(index);
  String shortenedString = sensorOutput.substring(iIndex+2, sensorOutput.length());
  #ifdef DEBUG_SERIAL
    Serial.print(F("    Shortened string: "));
    Serial.println(shortenedString);
  #endif
  int iSpace = shortenedString.indexOf(" ");
  String readingString = shortenedString.substring(0, iSpace);
  readingString.trim();
  #ifdef DEBUG_SERIAL
    Serial.print(F("    Reading string: "));
    Serial.println(readingString);
  #endif
  if (IsNumeric(readingString)) {
    value = readingString.toInt();
  }

  #ifdef DEBUG_SERIAL
    Serial.print(F("    Returning value: "));
    Serial.println(value);
  #endif
  
  return value;
}


