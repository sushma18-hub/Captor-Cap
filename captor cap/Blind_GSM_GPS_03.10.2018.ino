// GSM Tx goes to RX pin of Arduino, Rx pin goes to TX pin of Arduino
#include <SoftwareSerial.h>
SoftwareSerial gps(10,11); //(Rx,Tx) Tx of GPS, Rx of GPS
//String str="";
char str[70];
String gpsString="";
char *test="$GPGGA";
String latitude="No Range      ";
String longitude="No Range     ";
int temp=0,i;
int button = 8;
int value = 0;

boolean gps_status=0;
void setup() 
{
  pinMode(button,INPUT);
  Serial.begin(9600);
  gps.begin(9600);
  delay(2000);
  gsm_init();
  Serial.println("AT+CNMI=2,2,0,0,0");
  Serial.println("GPS Initializing");
  Serial.println("  No GPS Range  ");
  get_gps();
  delay(2000);
  Serial.println("GPS Range Found");
  Serial.println("GPS is Ready");
  Serial.print("System Ready");
  temp=0;
}
void loop()
{
  serialEvent();
  if(temp)
  {
    get_gps();
    tracking();
  }
  value=digitalRead(button);
  if (value == HIGH)
  {
    get_gps();
    tracking();
  }
}
void serialEvent()
{
  while(Serial.available())
  {
    if(Serial.find("Track"))
    {
      temp=1;
      break;
    }
    else
    temp=0;
  }
}
void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (gps.available()>0)            //checking serial data from GPS
   {
    char inChar = (char)gps.read();
     gpsString+= inChar;                    //store data from GPS into gpsString
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //checking for $GPGGA sentence
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}
void gsm_init()
{
  Serial.println("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
  Serial.println("Module Connected..");
  delay(1000);
  Serial.println("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  Serial.println("Network Found..");
}
void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    longitude="";
    int comma=0;
    while(x<str_lenth)
    {
      if(gpsString[x]==',')
      comma++;
      if(comma==2)        //extract latitude from string
      latitude+=gpsString[x+1];     
      else if(comma==4)        //extract longitude from string
      longitude+=gpsString[x+1];
      x++;
    }
    int l1=latitude.length();
    latitude[l1-1]=' ';
    l1=longitude.length();
    longitude[l1-1]=' ';
    
    Serial.print(latitude);
    Serial.println(" N");
    Serial.print(longitude);
    Serial.println(" E");
    i=0;x=0;
    str_lenth=0;
    delay(2000);
   }
}
void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+919980099900\"");
  delay(400);
}
void send_data(String message)
{
  Serial.println(message);
  delay(200);
}
void send_sms()
{
  Serial.write(26);
}
void lcd_status()
{
  Serial.println("Message Sent");
  delay(2000);
  Serial.println("System Ready");
  return;
}
void tracking()
{
    init_sms();
    send_data("Location of Emergency:");
    Serial.print(" LATITUDE :");
    send_data(latitude);
    Serial.println(" N");
    Serial.print(" LONGITUDE:");
    send_data(longitude);
    Serial.println(" E");
    send_data(" Please send HELP!");
    send_sms();
    delay(2000);
    lcd_status();
}



