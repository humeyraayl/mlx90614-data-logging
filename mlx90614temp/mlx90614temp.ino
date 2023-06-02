/* 
Humeyra Yıldız
Real-time data logging with mlx90614 temperature sensor
 */
#include <Wire.h>
#include <Adafruit_MLX90614.h>  
#include "RTClib.h" 
#include <SD.h>
#include <SPI.h> 

          
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); 

RTC_DS3231 rtc;
char filen[16] = {0};
File dataFile; 

float okuma[50];
float toplam ;
float temp ;
const int led = 4; // Pin 4 to the led.(led sabitine 4 değerini atadık.)
const int buton = 5; // Pin 4 to the button .(buton sabitine 5 değerini atadık.)
int sistemDurumu = 0;//I have defined the systemStatus variable to have an initial value of 0.(sistemDurumu değişkenini başlangıç değeri 0 olacak şekilde tanımladık).


//char daysOfTheWeek[7][12] = {"Pazar", "Pazartesi", "Salı", "Carsamba", "Persembe", "Cuma", "Cumartesi"};


void setup() { 

  pinMode(led, OUTPUT);  // set led pin as output(led pinini çıkış olarak ayarladık.)
  pinMode(buton, INPUT);  // set button pin as input(buton pinini giriş olarak ayarladık.)
  Serial.begin(9600); 
  delay(1000);
  mlx.begin();  

  //emissivity value settings
  //mlx.writeEmissivityReg (62258 ); //0.95 emissivity
  //Serial.print("Current emissivity = "); Serial.println(mlx.readEmissivity());

  if (! rtc.begin()) {
    Serial.println("NOT FOUND RTC!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time.!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Initialize_RTC() ;
  filename();
  Initialize_SDcard();

}

//function to set date and time.
void Initialize_RTC() 
{ 
 rtc.begin();
 rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 Serial.println("Time is set.");
}


//creating the filename.
void filename()
{
 DateTime now = rtc.now();
    /*
    Serial.print("Tarih ve Saat:  ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);*/

    int year = now.year();
    int month= now.month();
    int day= now.day();
    //Serial.println(yil);
   
    String date = "";
    date =String(day)+String(month)+String(year)+".txt";
    date.toCharArray(filen, 16);

    Serial.println(date);
    delay(1000);
}

//initializing the sd card module
void Initialize_SDcard()
{
  //kart mevcut mu?
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
 
  //dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
  dataFile = SD.open(String(filen), FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    //dataFile.println(temp); 
    dataFile.println("   DATE    |   TIME   | TEMPERATURE | EKG");
    dataFile.close();
  }
}
void SDcard_data()
{//is card available?
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
 
  //dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
  dataFile = SD.open(String(filen), FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    DateTime now = rtc.now();

    int year = now.year();
    int month= now.month();
    int day= now.day();
    int hour =now.hour();
    int minute =now.minute();

    String date= String(day)+"/"+String(month)+"/"+String(year);
    String time = String(hour)+":"+String(minute);  

    dataFile.print(date);
    dataFile.print("  |  ");
    dataFile.print(time);
    dataFile.print("   |   ");
    dataFile.print(temp);
    dataFile.print("°C   | ");
    dataFile.println("");
    dataFile.close();
  }

}


void temp_data(){

  digitalWrite(led, HIGH); // The LED lights up as the system starts.(sistem başladığı için ledimizi yakıyoruz.)
  toplam=0;
for (int i=0; i<50; i++)
{
  float deger =mlx.readObjectTempC();
  okuma[i]= deger *1;
  toplam = toplam + okuma[i];
  delay (5);
}
  temp = toplam / 50.0;
  //if(temp>=30){
  Serial.print("temp: ");  
  Serial.println(temp);
  Serial.println("C");
 
  delay(3000);
  //}
 
}


void loop() {
  
if(digitalRead(buton)== HIGH) //if the button is pressed
{
 delay(10); // Waiting for a while to avoid small arcs when the button is pressed.(Butona basılınca oluşan küçük arkları önlemek için biraz beklettik.)
 if(sistemDurumu == LOW){ sistemDurumu = 1;}  
 else if(sistemDurumu == HIGH){ sistemDurumu = 0;} 
 while(digitalRead(buton)== HIGH){;} // As long as the button is pressed, we kept it busy with our own loop so that it does not enter the main loop.(Butona basılı kaldığı sürece ana döngüye girmemesi için  kendi döngümüzle oyaladık.)
 delay(10);  
}

//for the values ​​read on the sensor
if(sistemDurumu == 1) 
{
temp_data();
SDcard_data();
 
}
else if(sistemDurumu == 0) // If the sistemDurumu variable is 0, turn the LED off.(sistemDurumu değişkenini 0 ise ledi söndür.)
{
digitalWrite(led,LOW);
}


}
