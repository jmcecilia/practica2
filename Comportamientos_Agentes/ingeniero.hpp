#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>

#include "comportamientos/comportamiento.hpp"

struct estadoI {
  int fila;
  int columna;
  int orientacion;
  bool zap;
  bool operator<(const estadoI &n) const {
    /* Redefinir el operador menor. Importante para distinguir entre estados */
    if (fila < n.fila) 
      return true;
    else
      return false;
  }
  bool operator==(const estadoI &n) const {
    if (fila == n.fila and columna == n.columna and
        orientacion == n.orientacion and zap == n.zap)
      return true;
    else
      return false;
  }
};

struct ComparaEstadosI {
  bool operator()(const estadoI &a, const estadoI &n) const {
    /* CURSOR: Definición alternativa a la redifinición del operador menor.*/
    if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
        (a.fila == n.fila and a.columna == n.columna and
         a.orientacion > n.orientacion) or
        (a.fila == n.fila and a.columna == n.columna and
         a.orientacion == n.orientacion and a.zap > n.zap))
      return true;
    else
      return false;
  }
};



class ComportamientoIngeniero : public Comportamiento {
public:
  // Constructor de la clase para los niveles 0, 1 y 6
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  // Constructor de la clase para los niveles 2, 3, 4 y 5
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC): Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  /**
   * @brief Calcula el coste de realizar una acción desde un estado determinado.
   * @param origen Estado actual del agente.
   * @param accion Acción que se pretende realizar.
   * @return Coste de la acción (p.e. basado en el tipo de terreno).
   */
  int CosteDeLaAccion(const estadoI &origen, Action accion);

  /**
   * @brief Algoritmo de búsqueda de caminos.
   * @param origen Estado inicial.
   * @param destino Estado objetivo.
   * @param plan Lista de acciones resultante.
   * @return true si se encontró un camino, false en caso contrario.
   */
  bool pathFinding(const estadoI &origen, const estadoI &destino, std::list<Action> &plan);

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)
  /**
   * @brief Implementación del comportamiento reactivo para el Nivel 0.
   * El objetivo es alcanzar una casilla 'U' (objetivo) priorizando caminos 
   * y evitando bucles mediante el uso de la matriz de tiempo.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // PRIMITIVAS SENSORIALES-MOTORAS (PROPORCIONADAS - NO MODIFICAR)
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   */
  void ActualizarMapa(Sensores sensores);



  /**
   * @brief Comprueba si una casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const estadoI &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  estadoI Delante(const estadoI &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto (camino, sendero, etc).
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es una casilla de tipo camino ('C', 'S', 'D' o 'U').
   */
  bool es_camino(unsigned char c) const;

  /**
   * @brief Traduce un índice del vector de sensores a coordenadas (fila, columna) reales.
   * Esta función es fundamental para poder indexar correctamente en mTiempo y mapaResultado.
   * Soporta las 8 orientaciones posibles del agente.
   * @param sensor_idx Índice en el vector de sensores (0-15).
   * @param sensores Referencia a los sensores actuales (para obtener posF, posC y rumbo).
   * @param[out] f Fila resultante.
   * @param[out] c Columna resultante.
   */
  void get_pos(int sensor_idx, const Sensores &sensores, int &f, int &c) const;

private:
  // Variables de Estado (pueden ser extendidas por el alumno)
  //******* NIVEL 0 **************/
  /// Última acción realizada con éxito por el agente.
  Action last_action;
  /// Indica si el ingeniero posee actualmente las zapatillas.
  bool tiene_zapatillas;
  /// Contador global de instantes de la simulación.
  int instante;
};

#endif
