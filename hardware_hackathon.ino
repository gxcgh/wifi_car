#define         MQ_PIN                       (0)     
#define         RL_VALUE                     (5)     
#define         RO_CLEAN_AIR_FACTOR          (9.83)  
#define         CALIBARAION_SAMPLE_TIMES     (50)    
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   
#define         READ_SAMPLE_INTERVAL         (50)    
#define         READ_SAMPLE_TIMES            (5)     
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)
#define smois A5
#define ldr A3
#define mdr 10
float mois;
#define tmpe 9
int hum;
int tmp;
#include<DHT.h>
DHT dht(tmpe,DHT11);
/*#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp;*/
float           LPGCurve[3]  =  {2.3,0.21,-0.47};   
float           COCurve[3]  =  {2.3,0.72,-0.34};    
float           SmokeCurve[3] ={2.3,0.53,-0.44};    
float           Ro           =  10;
void setup()
{
  Serial.begin(115200); 
  /*  while ( !Serial ) delay(100); 
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);           
  }
    
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500);*/
  Ro = MQCalibration(MQ_PIN);       
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\
");
dht.begin();
pinMode(ldr,INPUT);
pinMode(smois,INPUT);
pinMode(mdr,OUTPUT);
pinMode(5,OUTPUT);
pinMode(6,OUTPUT);
}
  
void loop()
{
    /*Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); 
    Serial.println(" m");
    Serial.println();*/
   Serial.print("LPG:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG)  );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("CO:");  
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO) );
   Serial.print(  "ppm" );
   Serial.print("    ");   
   Serial.print("SMOKE:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE) );
   Serial.print(  "ppm" );
   Serial.println("\
");
  int sin=analogRead(ldr);
  sin=map(sin,0,1023,0,100);
  Serial.print("Light Intenstiy: ");
  Serial.print(sin);
  Serial.println("%");
  mois=analogRead(smois);
  mois=100-(mois/10.23);
  Serial.print("Soil Moisture: ");
  Serial.print(mois); 
  Serial.println("%");
  hum=dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(hum); 
  Serial.println("%");
  tmp=dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(tmp); 
  Serial.println("C");
  
  delay(3000);
 
}
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}  
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   
 
  val = val/RO_CLEAN_AIR_FACTOR;                         
 
  return val; 
}

float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++)  {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id  == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else  if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    
 
  return 0;
}

int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,(  ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
void watersupply()
{
  int i=0;
  
  analogWrite(mdr,255);
  digitalWrite(5,  HIGH);
  digitalWrite(6, LOW);
  
}
void waterstop()
{
  int i=0;

  analogWrite(mdr,0);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
}
