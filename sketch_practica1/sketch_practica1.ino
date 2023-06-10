#include <LedControl.h>
#include "estructuras.h"
#include "matriz_sin_driver.h"

// Mensaje
int offset = 0;                    // Indica la posicion actual del movimiento del mensaje
bool msg_hacia_izquierda = false;  // Indica la direccion de recorrido que dara el mensaje inicial
int potenciometro = 0;             // Indica la velocidad del movimiento del mensaje

// Pines generales
short pinPotenciometroVelocidad = A0;                  // Pin para manejar el potenciometro que representa la velocidad del mensaje y del avion
short pinPotenciometroVidas = A1;                      // Pin para manejar el potenciometro que representa la cantidad de vidas que se tendra en una partida nueva
short pinDisparo = 50;                                 // Pin para manejar el boton de disparo
short pinDerecha = 49;                                 // Pin para manejar el boton con movimiento hacia la derecha
short pinIzquierda = 48;                               // Pin para manejar el boton con movimiento hacia la izquierda
short pinIni = 47;                                     // Pin para manejar el boton de inicio
LedControl matriz_driver = LedControl(51, 53, 52, 1);  // Matriz con driver

// Juego
short vidas = 3;                            // Indica la cantidad de vidas que tiene en el juego actual
short vidasConfiguracion = 3;        
short nivel = 1;                            // Indica el nivel actual de juego
short velocidadInicial = 1;                 // Indica el nivel de velocidad del juego
short edificios_a_destruir = 0;             // Indica la cantidad de edificios que hay que destruir en el nivel actual de juego
short edificios_destruidos = 0;             // Indica la cantidad de edificios que el usuario a destruido desde que ha iniciado el juego
short pos_x_avion = 0;                      // Indica la posicion del avion en el eje x de la matriz
short pos_y_avion = -3;                     // Indica la posicion del avion en el eje y de la matriz
bool avion_hacia_izquierda = false;         // Indica la direccion de movimiento del avion
String estado_app = "MENSAJE";              // Indica el estado actual en el que esta el programa; Estos pueden ser: MENSAJE, MENU, JUGAR, PAUSA, ESTADISTICA, CONFIGURACION
short estadisticas[5] = { 0, 0, 0, 0, 0 };  // Sirve para almacenar los 5 mejores puntajes

void setup() {
  inicializarMatrizSinDriver();  // Inicializando la matriz sin driver
  Serial.begin(9600);            // Inicializando la comunicacion serial

  // Inicializando la matriz con driver
  matriz_driver.shutdown(0, false);
  matriz_driver.setIntensity(0, 8);
  matriz_driver.clearDisplay(0);
}

long t0 = millis();
long t1 = millis();
void loop() {

  if (estado_app == "MENSAJE") {

    // Definiendo la velocidad de recorrido del mensaje
    t1 = millis();
    if (t1 - t0 >= 300 / velocidadInicial) {
      offset++;
      t0 = millis();
    }

    // Imprimiendo el mensaje en ambas matrices
    imprimirMensajeMatrizSinDriver();
    imprimirMensajeMatrizConDriver();

  } else if (estado_app == "MENU") {
    imprimirMenuPrincipal();
  } else if (estado_app == "JUGAR") {

    // Muestra el nivel y redibuja los edificios en el tablero en el caso que no halla mas edificios por destruir
    if (edificios_a_destruir == 0) {

      // Obtiene el tiempo actual en milisegundos
      unsigned long tiempoInicio = millis();

      // Muestra el mensaje del nivel durante 2 segundos
      while (millis() - tiempoInicio < 2000) {
        imprimirMensajeNivel();
      }

      // Dibuja los edificios la matriz 'tablero'
      dibujarEdificios();
    }

    // Definiendo la velocidad de recorrido del avion
    t1 = millis();
    if (t1 - t0 >= 300 / velocidadInicial) {
      offset++;
      t0 = millis();

      movimientoBomba();
      movimientoAvion();
    }

    // Dibujando el avion
    dibujarAvion();
    dibujarBomba();
    imprimirTablero();
  } else if (estado_app == "PAUSA") {
    imprimirMenuPausa(vidas);
  } else if (estado_app == "CONFIGURACION") {
    imprimirMenuDeConfiguracion(vidas, velocidadInicial);
  }

  // potenciometro = map(analogRead(A0), 0, 1024, 200, 800);

  // Escuchando los botones
  botonK();
  botonDerecho();
  botonIzquierdo();
  botonDisparo();
}


/***************************/
/********** JUEGO **********/
/***************************/

// Imprime todos los estados de la variable 'tablero'
// en la matriz sin driver y en la matriz con driver
void imprimirTablero() {

  // Matriz sin driver
  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      setearEstadoEnColumna(j, tablero[i][j]);
    }
    delay(1);
  }

  // Matriz con driver
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      matriz_driver.setLed(0, i, 15 - j, tablero[i][j]);
    }
  }
}

// Dibuja el avion en el tablero
void dibujarAvion() {
  if (avion_hacia_izquierda) {

    // Imprime la parte alta del avion
    if (pos_y_avion >= 0 && pos_y_avion < 16) {
      tablero[pos_x_avion][pos_y_avion] = 1;
    }

    // Imprime la parte baja del avion
    for (int i = 0; i < 3; i++) {
      if (pos_y_avion - i >= 0 && pos_y_avion - i < 16) {
        tablero[pos_x_avion + 1][pos_y_avion - i] = 1;
      }
    }

    // Si el avion colisiona contra un edificio se sube 2 unidades hacia arriba
    if (pos_y_avion - 3 >= 0 && pos_y_avion - 3 < 16 && tablero[pos_x_avion + 1][pos_y_avion - 3] == 1) {
      vidas--;
      if (vidas > 0) {
        movimientoAvion();
        pos_x_avion = pos_x_avion - 3;
      } else {
        reiniciarJuego();
        estado_app = "MENSAJE";
      }
    }

    // Si el avion llega a la parte final del tablero se baja un nivel y se reaparece del lado contrario
    if (pos_y_avion < 0) {
      pos_y_avion = 18;
      pos_x_avion++;
    }
  } else {

    // Imprime la parte alta del avion
    if (pos_y_avion >= 0 && pos_y_avion < 16) {
      tablero[pos_x_avion][pos_y_avion] = 1;
    }

    // Imprime la parte baja del avion
    for (int i = 0; i < 3; i++) {
      if (pos_y_avion + i >= 0 && pos_y_avion + i < 16) {
        tablero[pos_x_avion + 1][pos_y_avion + i] = 1;
      }
    }

    // Si el avion colisiona contra un edificio se sube 2 unidades hacia arriba
    if (pos_y_avion + 3 >= 0 && pos_y_avion + 3 < 16 && tablero[pos_x_avion + 1][pos_y_avion + 3] == 1) {
      vidas--;
      if (vidas > 0) {
        movimientoAvion();
        pos_x_avion = pos_x_avion - 3;
      } else {
        reiniciarJuego();
        estado_app = "MENSAJE";
      }
    }

    // Si el avion llega a la parte final del tablero se baja un nivel y se reaparece del lado contrario
    if (pos_y_avion > 16) {
      pos_y_avion = -3;
      pos_x_avion++;
    }
  }
}

// Desplaza el avion en el tablero
void movimientoAvion() {
  if (avion_hacia_izquierda) {
    if (pos_y_avion >= 0 && pos_y_avion < 16) {
      tablero[pos_x_avion][pos_y_avion] = 0;
    }
    for (int i = 0; i < 3; i++) {
      if (pos_y_avion - i >= 0 && pos_y_avion - i < 16) {
        tablero[pos_x_avion + 1][pos_y_avion - i] = 0;
      }
    }
    pos_y_avion--;
  } else {
    if (pos_y_avion >= 0 && pos_y_avion < 16) {
      tablero[pos_x_avion][pos_y_avion] = 0;
    }
    for (int i = 0; i < 3; i++) {
      if (pos_y_avion + i >= 0) {
        tablero[pos_x_avion + 1][pos_y_avion + i] = 0;
      }
    }
    pos_y_avion++;
  }
}

// Dibuja la bomba en el tablero
void dibujarBomba() {
  for (int i = 0; i < 16; i++) {
    if (bombas[i].x != -1 && bombas[i].y != -1) {
      tablero[bombas[i].x + 1][bombas[i].y] = 1;
    }
  }
}

// Desplaza la bomba en el tablero
void movimientoBomba() {
  for (int i = 0; i < 16; i++) {
    if (bombas[i].x != -1 && bombas[i].y != -1) {

      // Colosiona contra un edificio
      if (tablero[bombas[i].x + 2][bombas[i].y] == 1) {

        // Se acumula el edificio destruido
        edificios_destruidos++;

        // Se agrega la cantidad de edificios que se ha destruido durante el juego en las estadisticas
        for (int i = 0; i < sizeof(estadisticas) / sizeof(estadisticas[0]); i++) {
          if (edificios_destruidos > estadisticas[i]) {
            estadisticas[i] = edificios_destruidos;
            break;
          }
        }

        // En el caso que halla destruido 5 edificios se le agregara una vida extra
        if (edificios_destruidos % 5 == 0) {
          vidas++;
        }

        // Se borra el edificio
        for (int j = bombas[i].x + 1; j < 8; j++) {
          tablero[j][bombas[i].y] = 0;
        }

        // Se borra la bala
        tablero[bombas[i].x + 1][bombas[i].y] = 0;
        bombas[i].x = -1;
        bombas[i].y = -1;

        // Se disminuye la cantidad de edificios a destruir
        edificios_a_destruir--;

        // Se pasa al siguiente nivel una vez halla destruido todos los edificios y en el caso que sobrepase el nivel 10,
        // se va a la pantalla principal finalizando asi el juego
        if (edificios_a_destruir == 0) {
          nivel++;
          if (nivel > 10) {
            reiniciarJuego();
            estado_app = "MENSAJE";
          } else {
            movimientoAvion();
            pos_y_avion = avion_hacia_izquierda ? 18 : -3;
            pos_x_avion = 0;
          }
        }

      }
      // Colisiona con la parte baja de la matriz
      else if (bombas[i].x >= 6) {
        tablero[bombas[i].x + 1][bombas[i].y] = 0;
        bombas[i].x = -1;
        bombas[i].y = -1;
      }
      // Movimiento de la bomba
      else {
        tablero[bombas[i].x + 1][bombas[i].y] = 0;
        bombas[i].x = bombas[i].x + 1;
      }
    }
  }
}

// Dibuja los edificios que hay segun el nivel actual del juego
void dibujarEdificios() {
  short indice = 0;
  short largo = 0;
  short cantidad_edificios = nivel + 2;
  edificios_a_destruir = cantidad_edificios;
  while (cantidad_edificios > 0) {
    indice = random(16);
    largo = random(1, 5);
    if (!tablero[7][indice]) {
      for (int i = 0; i < largo; i++) {
        tablero[7 - i][indice] = 1;
      }
      cantidad_edificios--;
    }
  }
}

/***************************/
/********* BOTONES *********/
/***************************/

// Reconoce el boton ini presionado
unsigned long tiempo_boton_presionado;
void botonK() {
  int btnK = digitalRead(pinIni);

  if (btnK == HIGH) {
    if (tiempo_boton_presionado == 0) {
      tiempo_boton_presionado = millis();
    }
  } else {
    if (tiempo_boton_presionado != 0) {
      unsigned long lapso_tiempo = millis() - tiempo_boton_presionado;
      tiempo_boton_presionado = 0;

      // Entra al juego
      if (estado_app == "MENSAJE" && lapso_tiempo >= 2000) {
        estado_app = "MENU";
      }
      // Entra a la pausa del juego
      else if (estado_app == "JUGAR" && lapso_tiempo >= 2000) {
        estado_app = "PAUSA";
      }
      // Regresar al menu principal
      else if (estado_app == "PAUSA" && lapso_tiempo >= 3000) {
        reiniciarJuego();
        estado_app = "MENU";
      }
      // Entra a la pausa del juego
      else if (estado_app == "PAUSA" && lapso_tiempo >= 2000) {
        estado_app = "JUGAR";
      }
      // Regresar al menu principal si esta en configuracion
      else if(estado_app == "CONFIGURACION" && lapso_tiempo >= 3000){
        estado_app = "MENU";
      }else if(estado_app == "MENU" && lapso_tiempo > 2000){
        estado_app = "MENSAJE";
      }
    }
  }
}

// Reconoce el boton de disparo presionado
bool estado_boton_dis = false;
bool ultimo_estado_boton_dis = false;
unsigned long ultimo_tiempo_rebote_boton_dis = 0;
const unsigned long delay_rebote_boton_dis = 50;
void botonDisparo() {
  int btnDisparo = digitalRead(pinDisparo);

  if (btnDisparo != ultimo_estado_boton_dis) {
    ultimo_tiempo_rebote_boton_dis = millis();
  }

  if ((millis() - ultimo_tiempo_rebote_boton_dis) > delay_rebote_boton_dis) {
    if (btnDisparo != estado_boton_dis) {
      estado_boton_dis = btnDisparo;

      if (estado_boton_dis == LOW) {
        if (estado_app == "MENU") {
          estado_app = "ESTADISTICA";
        } else if (estado_app == "JUGAR") {
          for (int i = 0; i < sizeof(bombas) / sizeof(bombas[0]); i++) {
            if (bombas[i].x == -1 && bombas[i].y == -1) {
              if (avion_hacia_izquierda) {
                bombas[i].y = pos_y_avion - 1;
              } else {
                bombas[i].y = pos_y_avion + 1;
              }
              bombas[i].x = pos_x_avion + 1;
              break;
            }
          }
        }
      }
    }
  }

  ultimo_estado_boton_dis = btnDisparo;
}

// Reconoce el boton derecho presionado
bool estado_boton_der = false;
bool ultimo_estado_boton_der = false;
unsigned long ultimo_tiempo_rebote_boton_der = 0;
const unsigned long delay_rebote_boton_der = 50;
void botonDerecho() {
  int btnDerecha = digitalRead(pinDerecha);

  if (btnDerecha != ultimo_estado_boton_der) {
    ultimo_tiempo_rebote_boton_der = millis();
  }

  if ((millis() - ultimo_tiempo_rebote_boton_der) > delay_rebote_boton_der) {
    if (btnDerecha != estado_boton_der) {
      estado_boton_der = btnDerecha;

      if (estado_boton_der == LOW) {
        if (estado_app == "MENSAJE") {
          msg_hacia_izquierda = false;
        } else if (estado_app == "MENU") {
          estado_app = "CONFIGURACION";
        } else if (estado_app == "JUGAR") {
          movimientoAvion();
          avion_hacia_izquierda = false;
        }
      }
    }
  }

  ultimo_estado_boton_der = btnDerecha;
}

// Reconoce el boton izquierdo presionado
bool estado_boton_izq = false;
bool ultimo_estado_boton_izq = false;
unsigned long ultimo_tiempo_rebote_boton_izq = 0;
const unsigned long delay_rebote_boton_izq = 50;
void botonIzquierdo() {
  int btnIzquierda = digitalRead(pinIzquierda);

  if (btnIzquierda != ultimo_estado_boton_izq) {
    ultimo_tiempo_rebote_boton_izq = millis();
  }

  if ((millis() - ultimo_tiempo_rebote_boton_izq) > delay_rebote_boton_izq) {
    if (btnIzquierda != estado_boton_izq) {
      estado_boton_izq = btnIzquierda;

      if (estado_boton_izq == LOW) {
        if (estado_app == "MENSAJE") {
          msg_hacia_izquierda = true;
        } else if (estado_app == "MENU") {
          estado_app = "JUGAR";
        } else if (estado_app == "JUGAR") {
          movimientoAvion();
          avion_hacia_izquierda = true;
        }
      }
    }
  }

  ultimo_estado_boton_izq = btnIzquierda;
}

/***************************/
/********** NIVEL ***********/
/***************************/

void imprimirMensajeNivel() {

  // Matriz sin driver
  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      setearEstadoEnColumna(j, caracter_nivel[i][j]);
    }
    delay(1);
  }


  // Matriz con driver - Primer digito
  int primer_digito = nivel / 10;
  for (int fila = 0; fila < 8; fila++) {
    for (int columna = 0; columna < 4; columna++) {
      matriz_driver.setLed(0, fila, 7 - columna, numeros[primer_digito][fila][columna]);
    }
  }

  // Matriz con driver - Segundo digito
  int segundo_digito = nivel - primer_digito * 10;
  for (int fila = 0; fila < 8; fila++) {
    for (int columna = 0; columna < 4; columna++) {
      matriz_driver.setLed(0, fila, 3 - columna, numeros[segundo_digito][fila][columna]);
    }
  }
}

void reiniciarJuego() {
  nivel = 1;
  edificios_a_destruir = 0;
  pos_x_avion = 0;
  pos_y_avion = -3;
  edificios_destruidos = 0;
  vidas = vidasConfiguracion;
  for(int i = 0; i < 8; i++){
      for(int j = 0; j < 16; j++){
        tablero[i][j] = 0;
     }
  }
}

/***************************/
/********** MENU ***********/
/***************************/

void imprimirMenuPrincipal() {

  // Matriz sin driver
  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      setearEstadoEnColumna(j, menu_principal[i][j]);
    }
    delay(1);
  }

  // Matriz con driver
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      matriz_driver.setLed(0, i, 15 - j, menu_principal[i][j]);
    }
  }
}

/******IMPRIMIR MENU CONFIGURACION******/
void imprimirMenuDeConfiguracion(int velocidadInicialJuego, int vidasInicial) {
  int newVelocidad = map(analogRead(A0), 0, 1023, 1, 3);
  Serial.print("velocidad: ");
  Serial.println(newVelocidad);
  if (vidasInicial <= 10) {

    int newVidas = map(analogRead(A1), 0, 1023, 3, 10);
    Serial.print("vidas: ");
    Serial.println(newVidas);

    //Imprime barra inferior
    for (int fila = 4; fila < 8; fila++) {
      for (int columna = 0; columna < 8; columna++) {
        matriz_driver.setLed(0, fila, 7 - columna, barra[newVidas - 4][fila][columna]);
      }
    }
    vidasConfiguracion = newVidas;
    vidas = newVidas;
  }





  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      setearEstadoEnColumna(j, caracter_config[i][j]);
    }
    delay(1);
  }

  //Imprime barra superior
  for (int fila = 0; fila < 4; fila++) {
    for (int columna = 0; columna < 8; columna++) {
      matriz_driver.setLed(0, fila, 7 - columna, barra[newVelocidad][fila][columna]);
    }
  }
  velocidadInicial = newVelocidad;
}


/******IMPRIMIR MENU PAUSA******/
void imprimirMenuPausa(int vidas) {
  Digitos digitos = obtenerDigitos(vidas);
  int digito1 = digitos.digito1;
  int digito2 = digitos.digito2;
  Serial.println(digito1);
  Serial.println(digito2);
  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      setearEstadoEnColumna(j, caracter_vidas[i][j]);
    }
    delay(1);
  }

  for (int fila = 0; fila < 8; fila++) {
    for (int columna = 0; columna < 4; columna++) {
      matriz_driver.setLed(0, fila, 7 - columna, numeros[digito1][fila][columna]);
    }
  }

  // Matriz con driver - Segundo digito
  for (int fila = 0; fila < 8; fila++) {
    for (int columna = 0; columna < 4; columna++) {
      matriz_driver.setLed(0, fila, 3 - columna, numeros[digito2][fila][columna]);
    }
  }

}


Digitos obtenerDigitos(int numero) {
  Digitos digitos;
  digitos.digito1 = numero / 10;
  digitos.digito2 = numero % 10;
  return digitos;
}

/***************************/
/********* CADENA **********/
/***************************/

void imprimirMensajeMatrizSinDriver() {
  for (int i = 0; i < 9; i++) {
    seleccionarFila(i);
    for (int j = 0; j < 8; j++) {
      offset = offset >= LONGITUD_TEXTO ? 0 : offset;
      if (msg_hacia_izquierda) {
        setearEstadoEnColumna(j, cadena[i][(j + offset) % LONGITUD_TEXTO]);
      } else {
        setearEstadoEnColumna(j, cadena[i][(j - offset < 0 ? LONGITUD_TEXTO + j - offset : j - offset)]);
      }
    }
    delay(1);
  }
}

void imprimirMensajeMatrizConDriver() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (msg_hacia_izquierda) {
        short offset_aux = 8 + offset + j;
        matriz_driver.setLed(0, i, 7 - j, cadena[i][offset_aux >= LONGITUD_TEXTO ? offset_aux - LONGITUD_TEXTO : offset_aux]);
      } else {
        short offset_aux = 8 - offset + j;
        matriz_driver.setLed(0, i, 7 - j, cadena[i][offset_aux < 0 ? LONGITUD_TEXTO + offset_aux : offset_aux]);
      }
    }
  }
}
