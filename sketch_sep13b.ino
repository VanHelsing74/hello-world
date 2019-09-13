#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
long nextTime = 100000;
Adafruit_ADS1115 ads;
#define SSID "ROSTELECOM_10A8"              
#define PASS "AYKGE4GK"                               
String IP = "api.thingspeak.com"; 

#define esp_bitrate 9600  
#define rxpin 5      
#define txpin 6                                  
long interval = 12000; 
SoftwareSerial espSerial( rxpin, txpin );
String GET = "GET /update?key=4BPVWS4EV6ITZKPI&field1="; 
String HOST = "Host: " + IP + "\r\n\r\n";


 
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
 
void setup(void)
{  
  
  
  Serial.begin(9600);
Serial.begin(esp_bitrate);
  espSerial.begin(esp_bitrate);
   sendespSerial("AT");
  delay(2000);
  if (espSerial.find("OK"))
  {
    Serial.println("RECEIVED ESP: OK\nData ready to sent!");
    connectWiFi();
  }  
lcd.begin(16, 2);
ads.setGain(GAIN_SIXTEEN); 
ads.begin();
analogReference(INTERNAL);// опорное 1,1 В
pinMode(A0,INPUT_PULLUP);
pinMode(5,INPUT);
 
}
long u_digital,u_an,u_sum;
const float multiplier = 0.0078125;
float u,u_iz,t,tx_sum,t_sum,t_iz,t_iz_analog,t_analog;
float a[]={0,2.5173462*10,-1.1662878,-1.08336338,-8.9773540/10,-3.7342377/10,-8.6632643/100,-1.0450598/100,-5.1920577/10000};
float b[]{0,2.508355*10,7.860106/100,-2.503131/10,8.315270/100,-1.228034/100,9.804036/10000,-4.413030/100000,1.057734/1000000,-1.052755/100000000};
float c[]{-1.318058*100,4.830222*10,-1.646031,5.464731/100,-9.6550715/10000,8.802193/1000000,-3.110810/100000000};
float e[]={1.1573067/10000,1.5884573*10,4.0458554/100,0.3170064,0.1666128,5.146958/100,9.5288883/1000,1.0301283/1000,6.0654431/100000,1.5131878/1000000};
float f[]{7.2069422/1000,1.5775525*10,-0.2261183,9.4286756/1000,-3.5394655/10000,1.0050886/100000,-1.9323678/10000000,2.3816891/1000000000,-1.7130654/100000000000,5.4857331/100000000000000};
int tx,tip,kn,kn1;
const float t0 = 26.0; 
const float t0_analog = 545;
void loop(void){ if(digitalRead(5)==HIGH){kn1++;delay(100);}if(kn1==2){kn1=0;String T = String(t_iz); 
  
 Serial.println(millis());
   if ( millis() - nextTime > (interval - 4215) ) {
    update_param(T);
    
      nextTime = millis();
      }
 
 }
lcd.clear();
u_sum=0;tx_sum = 0;
u_iz=u_an * multiplier;
lcd.setCursor(0,0);lcd.print("T = ");lcd.setCursor(4,0);lcd.print(t,1);lcd.print(" C");lcd.setCursor(12,0);
if(tip==0){lcd.print(" TXA ");}if(tip==1){lcd.print(" TXK ");}
lcd.setCursor(0,1);lcd.print(u_iz,2); lcd.print(" mV");lcd.setCursor(11,1);lcd.print(t_iz,1);lcd.print("C");
if(kn1==0){txa();}
if(kn1==1){txk();}
for(int i=0;i<100;i++){
u_digital = ads.readADC_Differential_0_1();
tx = analogRead(A0);
Serial.println(tx);
tx_sum = tx_sum + tx;
u_sum = u_sum + u_digital;
delay(1);
}
u_an=u_sum/100;
t_analog = tx_sum / 100;
t_iz_analog = (t_analog-t0_analog); 
t_iz = t0 - t_iz_analog;

 



}
 
void txa(){tip=0;u=u_iz+t_iz*0.0403;
if(u<0){t=(a[0]*pow(u,0))+(a[1]*pow(u,1))+(a[2]*pow(u,2))+(a[3]*pow(u,3))+(a[4]*pow(u,4))+(a[5]*pow(u,5))+(a[6]*pow(u,6))+(a[7]*pow(u,7))+(a[8]*pow(u,8));}
if(u>=0&&u<=20.64){t=(b[0]*pow(u,0))+(b[1]*pow(u,1))+(b[2]*pow(u,2))+(b[3]*pow(u,3))+(b[4]*pow(u,4))+(b[5]*pow(u,5))+(b[6]*pow(u,6))+(b[7]*pow(u,7))+(b[8]*pow(u,8))+(b[9]*pow(u,9));}
if(u>20.64){t=(c[0]*pow(u,0))+(c[1]*pow(u,1))+(c[2]*pow(u,2))+(c[3]*pow(u,3))+(c[4]*pow(u,4))+(c[5]*pow(u,5))+(c[6]*pow(u,6));}
if(u<-6.35){t=-240;} if(u>54.9){t=1372;}
}
void txk(){tip=1;u=u_iz+t_iz*0.06555;
if(u<0){t=(e[0]*pow(u,0))+(e[1]*pow(u,1))+(e[2]*pow(u,2))+(e[3]*pow(u,3))+(e[4]*pow(u,4))+(e[5]*pow(u,5))+(e[6]*pow(u,6))+(e[7]*pow(u,7))+(e[8]*pow(u,8))+(e[9]*pow(u,9));}
if(u>=0){t=(f[0]*pow(u,0))+(f[1]*pow(u,1))+(f[2]*pow(u,2))+(f[3]*pow(u,3))+(f[4]*pow(u,4))+(f[5]*pow(u,5))+(f[6]*pow(u,6))+(f[7]*pow(u,7))+(f[8]*pow(u,8))+(f[9]*pow(u,9));}
if(u<-9.49){t=-200;} if(u>66.47){t=800;}

}


void sendespSerial(String cmd) {
  Serial.print("SEND: ");
  Serial.println(cmd);
  espSerial.println(cmd);
}


//*-- Соединение с точкой доступа
boolean connectWiFi() {
  espSerial.println("AT+CWMODE=1"); 
  delay(2000);
  String cmd = "AT+CWJAP=\"";       
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  sendespSerial(cmd);
  delay(5000);
  if (espSerial.find("OK"))
  {
    Serial.println("Connect: OK");
    return true;
  }
  else
  {
    Serial.println("Connect: Error");
    return false;
  }

  cmd = "AT+CIPMUX=0";           
  sendespSerial( cmd );
  if ( espSerial.find( "Error") )
  {
    Serial.print( "RECEIVED: Error" );
    return false;
  }
}


void update_param( String T) {

  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  sendespSerial(cmd);
  delay(1000);

  if ( espSerial.find( "Error" ) )
  {
    Serial.print( "RECEIVED: Error\nExit1" );
    return;
  }

  // Отправка строки на сервер
  cmd = GET + T  ; //

  cmd += " HTTP/1.1\r\n"  + HOST;
  espSerial.print( "AT+CIPSEND=" );
  espSerial.println( cmd.length() );
  if (espSerial.find( ">" ) )
  {
    Serial.print(">");
    Serial.print(cmd);
    espSerial.print(cmd);
    delay(30);
  }
  else
  {
    sendespSerial( "AT+CIPCLOSE" );
  }
  if ( espSerial.find("OK") )
  {
    Serial.println( "RECEIVED: OK" );
  }
  else
  {
    Serial.println( "RECEIVED: Error\nExit2" );
  }
 
}










