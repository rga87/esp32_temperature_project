#include <SPI.h>                        // Подключение библиотеки SPI
#include <Wire.h>                       // Подключение библиотеки Wire
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#define PIN_DS18B20 14
#define pin_SDCardCS 13
OneWire oneWire(PIN_DS18B20);
// Создаем объект DallasTemperature для работы с сенсорами, передавая ему ссылку на объект для работы с 1-Wire.
DallasTemperature sensor(&oneWire);
unsigned long lastChangeTemperature=0;
float last_temperature = 0;
int numberFile=0;
String nameFile;
int checkFistLine=0;
void setup()
{
  Serial.begin(9600);   // Initialize serial communications with the PC
  // начинаем работу с датчиком
  sensor.begin();
  // устанавливаем разрешение датчика от 9 до 12 бит
  sensor.setResolution(12);
    pinMode(pin_SDCardCS, OUTPUT);
   // Пытаемся проинициализировать модуль
  if (!SD.begin(pin_SDCardCS)) 
  {
    // Если что-то пошло не так, завершаем работу:
    return;
  }
  else
  {
    while(SD.exists("datalog"+String(numberFile)+".csv"))
    { 
       numberFile++;
    }
    nameFile="datalog"+String(numberFile)+".csv";
    writeSD(("Температура;Время_cтарт;Время_финиш;Продолжительность;ПЕ;\n"));
  }
}

void loop()
{
   unsigned long currentMillis=millis();
   float pe=0;
   float exponent=0;
   float timeForPe=0;
   float forExponent=0;
  // переменная для хранения температуры
  float temperature;
  // отправляем запрос на измерение температуры
  sensor.requestTemperatures();
  // считываем данные из регистра датчика
  temperature = sensor.getTempCByIndex(0);
  if (((temperature - last_temperature) > 0.5) || ((last_temperature - temperature) > 0.5))
  {
    // выводим температуру в Serial-порт
    if (temperature > 0)
    {
      if (checkFistLine>0)
      {
          if(temperature>=60)
          {
            exponent=last_temperature-60;
            timeForPe=(currentMillis-lastChangeTemperature) ;
            forExponent=pow(1.393,exponent);
            pe=timeForPe/60000*forExponent;
          }
          writeSD(TimePrint(currentMillis)+";"+TimePrint(currentMillis-lastChangeTemperature)+";"+ String(pe)+";"+String(exponent)+";"+String(timeForPe)+";"+String(forExponent)+";\n"+String(temperature)+";"+TimePrint(currentMillis)+";");
          last_temperature = temperature;
          lastChangeTemperature=currentMillis;
      }
      else
      {
        writeSD(String(temperature)+";"+TimePrint(currentMillis)+";");
      }
      checkFistLine++;
    }
  }
  // Задержка для того, чтобы можно было что-то разобрать на экране
  delay(2000);
}
void writeSD(String st)
{
  // Открываем файл  для чтения и записи, начиная с конца файла, но помним, что одновременно можно работать только с одним файлом.
  // Если файла с таким именем не будет, ардуино создаст его.
  File dataFile = SD.open(nameFile, FILE_WRITE);
  // Если все хорошо, то записываем строку:
  if (dataFile) 
  {
    dataFile.print(st);
    dataFile.close();
  }
  else
  {
  // Сообщаем об ошибке, если все плохо
  }
}
String  TimePrint(unsigned long times)
{
  return (String((25 +times / 3600000)% 25)+":"+String((60 + times / 60000)% 60)+":"+String((60 + times / 1000)% 60));
}
