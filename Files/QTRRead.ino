#include <Balboa32U4.h>
#include <EEPROM.h>

Balboa32U4LineSensors sensors; // Sensores de reflectividad
uint16_t sensorValues[5];      // Array para almacenar las lecturas de los sensores

const float fcl = 10;          // Frecuencia de corte del filtro siguelíneas
const float taul = 0.5/PI/fcl; // Constante de tiempo del filtro siguelíneas

const int h = 40;              // Paso de tiempo para salida por puerto serie (ms)

unsigned long t0 = 0;          // Tiempo anterior para medir 'dt'
unsigned long th = 0;          // Tiempo anterior para salida por puerto serie

float eFilt = 0;               // Error de posición lateral filtrado


void setup()
{
  Serial.begin(9600);

  // Configuramos los sensores
  sensors.setCenterAligned();

  // Llamamos una vez a 'calibrate' para inicializar los arrays
  sensors.calibrate(QTRReadMode::On);

  // Leemos la calibración de la EEPROM
  int eeAddress = 0;
  for (int i = 0; i < 5; i++)
  {
    EEPROM.get(eeAddress, sensors.calibrationOn.minimum[i]);
    eeAddress += sizeof(uint16_t);
    EEPROM.get(eeAddress, sensors.calibrationOn.maximum[i]);
    eeAddress += sizeof(uint16_t);
  }
}


void loop()
{
  // Medimos el paso de tiempo en s
  unsigned long t = micros();
  float dt = (t - t0)*1e-6;
  t0 = t;

  // Leemos posición ('readLineBlack' para línea oscura sobre fondo claro)
  uint16_t pos = sensors.readLineWhite(sensorValues, QTRReadMode::On);

  // El error es el valor deseado (2000) menos el medido, convertido a 'float'
  float yErr = 2000.0 - (float)pos;

  // Filtramos el error ya en posición, porque tiene bastante ruido
  // De este modo, ya no será necesario filtrar la derivada del error
  float al = dt/(taul + dt);
  eFilt = al*yErr + (1 - al)*eFilt;

  // Salida para verificar la medición con el Serial Plotter
  // El error debe ser cercano a 0 cuando el robot está centrado sobre la línea
  // Si se desplaza el robot a la derecha, el valor aumenta hacia 2000
  // Si se desplaza hacia la izquierda, el error disminuye hacia -2000
  if (t - th >= 1e3*h)
  {
    Serial.print(-2000);
    Serial.print(" ");
    Serial.print(2000);
    Serial.print(" ");
    Serial.println(eFilt);

    th = t;
  }
}