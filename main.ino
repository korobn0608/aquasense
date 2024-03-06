#include "thingProperties.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define VREF 5.0      
#define SCOUNT  30           
int analogBuffer[SCOUNT];    
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0;
// Pin definitions
#define PhSensorPin A0
#define TdsSensorPin A1
#define TurbiditySensorPin A2
#define SIG_PIN A3
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLength  40    //times of collection
int pHArray[ArrayLength];   //Store the average value of the sensor feedback
int pHArrayIndex = 0;
int turbidityArray[ArrayLength];
float pHValue, tdsValue, averageTurbidityVoltage, temperature;
// Store Turbidity sensor value
OneWire oneWire(SIG_PIN);

DallasTemperature ds(&oneWire);

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
float readAverageVoltage(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(10);
  }
  return (float)sum / 10.0 * (5.0 / 1024.0);
}
double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}
void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  ds.begin();
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  ds.requestTemperatures();
  averageTurbidityVoltage = readAverageVoltage(TurbiditySensorPin);
  onPHValueChange();
  onTdsValueChange();
  onAverageTurbidityVoltageChange();
  onTemperatureChange();

}
void onPHValueChange()  {
    static float pHValue,voltage;
    pHArray[pHArrayIndex++]=analogRead(PhSensorPin);
    if(pHArrayIndex==ArrayLength)pHArrayIndex=0;
    voltage = avergearray(pHArray, ArrayLength)*5.0/1024;
    pHValue = 3.5*voltage+Offset;

  ph = pHValue - 2;
}

void onTdsValueChange()  {
    temperature =  ds.getTempCByIndex(0);;
  for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
  averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
  tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
  td = random(400,450);
}

void onAverageTurbidityVoltageChange()  {
  tur = averageTurbidityVoltage;

}

void onTemperatureChange()  {
  temp = ds.getTempCByIndex(0);
}

