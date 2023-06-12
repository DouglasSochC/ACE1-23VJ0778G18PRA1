#include <Arduino.h>
#include "matriz_sin_driver.h"

// Pines que se utilizan en el arduino para manejar la matriz sin driver
short columnas[] = { 37, 36, 35, 34, 33, 32, 31, 30 };
short filas[] = { 29, 28, 27, 26, 25, 24, 23, 22 };

void inicializarMatrizSinDriver() {
  //Se definen las salidas
  for (int i = 0; i < 8; i++) {
    pinMode(filas[i], OUTPUT);
    pinMode(columnas[i], OUTPUT);
  }
}

void seleccionarFila(int fila) {
  if (fila == 0) digitalWrite(filas[0], LOW);
  else digitalWrite(filas[0], HIGH);
  if (fila == 1) digitalWrite(filas[1], LOW);
  else digitalWrite(filas[1], HIGH);
  if (fila == 2) digitalWrite(filas[2], LOW);
  else digitalWrite(filas[2], HIGH);
  if (fila == 3) digitalWrite(filas[3], LOW);
  else digitalWrite(filas[3], HIGH);
  if (fila == 4) digitalWrite(filas[4], LOW);
  else digitalWrite(filas[4], HIGH);
  if (fila == 5) digitalWrite(filas[5], LOW);
  else digitalWrite(filas[5], HIGH);
  if (fila == 6) digitalWrite(filas[6], LOW);
  else digitalWrite(filas[6], HIGH);
  if (fila == 7) digitalWrite(filas[7], LOW);
  else digitalWrite(filas[7], HIGH);
}

void setearEstadoEnColumna(int columna, bool estado) {
  if (columna == 0) digitalWrite(columnas[0], estado);
  if (columna == 1) digitalWrite(columnas[1], estado);
  if (columna == 2) digitalWrite(columnas[2], estado);
  if (columna == 3) digitalWrite(columnas[3], estado);
  if (columna == 4) digitalWrite(columnas[4], estado);
  if (columna == 5) digitalWrite(columnas[5], estado);
  if (columna == 6) digitalWrite(columnas[6], estado);
  if (columna == 7) digitalWrite(columnas[7], estado);
}
