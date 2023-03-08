#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <SD.h>

// The TinyGPS++ object
TinyGPSPlus gps;
//sd card
File myFile;

static const byte RXPin1 = 27, TXPin1 = 26;//GPS
HardwareSerial serial1(1);
static const byte RXPin2 = 17, TXPin2 = 16;//GSM
//static const byte RXPin2 = 16, TXPin2 = 17;
HardwareSerial serial2(2);


String waktu;
String operators;
String sinyal;
char g;
byte kualitasSinyal, a;
float latitude, longitude, arah, kecepatan;
byte satelit;
char sz[32] = "";

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\nCEK SINYAL DAN GPS");

  serial2.begin(9600, SERIAL_8N1, RXPin2, TXPin2);
  Serial.println("serial2");

  serial1.begin(9600, SERIAL_8N1, RXPin1, TXPin1);
  Serial.println("serial1");

  //serial1.begin(115200, SERIAL_8N1, RXpin1, TXpin1);
  // serial2.begin(115200, SERIAL_8N1, RXpin2, TXpin2);

  //init SD Card
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  Serial.println("ALAT UKUR");
  delay(3000);
  
  //init GPS - wait until coordinate appear/occur
  Serial.println("cek gps");
  pinMode ( 2, OUTPUT );
  digitalWrite ( 2, HIGH );
  delay(1000);
  while (1) {
    //Serial.println("Ambil data GPS");
    while (serial1.available() > 0)
      if (gps.encode(serial1.read()))

        if (gps.charsProcessed() < 10) {
          Serial.println(F("No GPS detected: check wiring."));
        }
    if (gps.location.lng() > 0) break;
  }

  digitalWrite ( 2, 0 );
  //init GSM
  serial2.println("AT");
  unsigned long awal = millis();
  while (millis() - awal < 500) {
    g = serial2.read();
    if (isAscii(g)) {
      Serial.print(g);
    }
  }
}

void loop() {
  digitalWrite ( 2, HIGH );
  kualitaSinyal();
  digitalWrite ( 2, 0 );
  cekLokasi();

  Serial.print(waktu);
  Serial.print(",");
  Serial.print(longitude, 6);
  Serial.print(",");
  Serial.print(latitude, 6);
  Serial.print(",");
  Serial.println(kualitasSinyal);


  //simpan SD Card
  myFile = SD.open("/data.txt", FILE_APPEND);
  if (myFile) {
    myFile.print(waktu);
    myFile.print(",");
    myFile.print(longitude, 6);
    myFile.print(",");
    myFile.print(latitude, 6);
    myFile.print(",");
    myFile.println(kualitasSinyal);

    // close the file:
    myFile.close();
    //    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
}

void kualitaSinyal() {
  sinyal = "";
  //  Serial.println(F("AT+CSQ"));
  serial2.println(F("AT+CSQ"));
  //Serial.flush();
  serial2.flush();
  unsigned long awal = millis();
  while (millis() - awal < 500) {
    g = serial2.read();
    if (isAscii(g)) {
      //      Serial.print(g);
      sinyal += g;
    }
  }
  //Serial.println(" ");
  //  Serial.println(sinyal);
  //Serial.flush();
  serial2.flush();
  byte nilai = sinyal.indexOf(':');
  kualitasSinyal = (sinyal.substring(nilai + 1, sinyal.indexOf(',') + 1)).toInt();
  //  Serial.print(",");
  //  Serial.println(kualitasSinyal);
}
void cekLokasi() {
  //waktu
  while (serial1.available() > 0)
    if (gps.encode(serial1.read()))
      waktu = "";
  for (byte j = 0; j < sizeof(sz); j++) {
    sz[j] = '\0';
  }
  sprintf(sz, "%02d/%02d/%02d ", gps.date.month(), gps.date.day(), gps.date.year());
  waktu = String(sz);
  sprintf(sz, "%02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second());
  waktu = waktu + String(sz);

  //lokasi
  latitude = gps.location.lat();
  longitude = gps.location.lng();

}
