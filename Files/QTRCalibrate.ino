#include <Balboa32U4.h>
#include <EEPROM.h>

Balboa32U4LineSensors sensors; // Sensores de reflectividad
uint16_t sensorValues[5];      // Array para almacenar las lecturas de los sensores

const float fcl = 10;          // Frecuencia de corte del filtro siguelíneas
const float taul = 0.5/PI/fcl; // Constante de tiempo del filtro siguelíneas

const int h = 40;              // Paso de tiempo para salida por puerto serie (ms)

unsigned long t0 = 0;          // Tiempo anterior para medir 'dt'
unsigned long th = 0;          // Tiempo anterior para salida por puerto serie

float pFilt = 0;               // Posición lateral filtrada


void setup()
{
  Serial.begin(9600);

  // Configuramos los sensores
  sensors.setCenterAligned();

  // Encendemos LED amarillo durante la calibración
  ledYellow(1);
  Serial.print("Calibrando... ");

  // Hay que mover el robot lateralmente sobre la línea, pasando por todos los sensores
  // 250 muestras tardan unos 10 segundos, la calibración termina cuando se apaga el led amarillo
  for (int i = 0; i < 250; i++)
    sensors.calibrate(QTRReadMode::On);

  // Apagamos el LED cuando termina la calibración
  Serial.println("listo");
  ledYellow(0);

  // Mostramos los valores mínimos
  for (int i = 0; i < 5; i++)
  {
    Serial.print(sensors.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // Mostramos los valores máximos
  for (int i = 0; i < 5; i++)
  {
    Serial.print(sensors.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();

  // Guardamos resultados en la EEPROM
  int eeAddress = 0;
  for (int i = 0; i < 5; i++)
  {
    EEPROM.put(eeAddress, sensors.calibrationOn.minimum[i]);
    eeAddress += sizeof(uint16_t);
    EEPROM.put(eeAddress, sensors.calibrationOn.maximum[i]);
    eeAddress += sizeof(uint16_t);
  }

  // Dejamos un tiempo para visualizar los valores
  delay(5000);
}


void loop()
{
  // Medimos el paso de tiempo en s
  unsigned long t = micros();
  float dt = (t - t0)*1e-6;
  t0 = t;

  // Leemos posición ('readLineBlack' para línea oscura sobre fondo claro)
  uint16_t pos = sensors.readLineWhite(sensorValues, QTRReadMode::On);

  // Filtramos la posición, porque tiene bastante ruido
  float al = dt/(taul + dt);
  pFilt = al*pos + (1 - al)*pFilt;

  // Salida para verificar la calibración con el Serial Plotter
  // Debe salir 2000 cuando el robot está centrado sobre la línea
  // Si se desplaza el robot a la derecha, el valor baja hacia 0
  // Si se desplaza hacia la izquierda, la lectura sube hacia 4000
  if (t - th >= 1e3*h)
  {
    Serial.print(0);
    Serial.print(" ");
    Serial.print(4000);
    Serial.print(" ");
    Serial.println(pFilt);

    th = t;
  }
}