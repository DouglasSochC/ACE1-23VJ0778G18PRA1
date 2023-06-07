#ifndef MATRIZ_SIN_DRIVER_H
#define MATRIZ_SIN_DRIVER_H

/*
  Se encarga de configurar y preparar el uso de la matriz sin driver.
*/
void inicializarMatrizSinDriver();

/*
  Se encarga de preparar el seteo de estados en una fila completa de la matriz. Por lo tanto, los estados de las demas filas son LOWs
*/
void seleccionarFila(int fila);

/*
  Se encarga de setear un estado en la columna indicada; Para poder utilizar este metodo es necesario utilizar previamente el metodo 'seleccionarFila'
*/
void setearEstadoEnColumna(int columna, bool estado);

#endif