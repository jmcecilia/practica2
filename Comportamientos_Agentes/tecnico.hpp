#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>

#include "comportamientos/comportamiento.hpp"

struct estadoT {
  int fila;
  int columna;
  int orientacion;
  bool zap;
  bool operator<(const estadoT &n) const {
    /* Redefinir el operador menor. Importante para distinguir entre estados */
    if (fila < n.fila) 
      return true;
    else
      return false;
  }
  bool operator==(const estadoT &n) const {
    if (fila == n.fila and columna == n.columna and
        orientacion == n.orientacion and zap == n.zap)
      return true;
    else
      return false;
  }
};

struct ComparaEstadosT {
  bool operator()(const estadoT &a, const estadoT &n) const {
    /* CURSOR: Definición alternativa a la redifinición del operador menor.*/
    if (a.fila > n.fila)
      return true;
    else
      return false;
  }
};


class ComportamientoTecnico : public Comportamiento {
public:
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC): Comportamiento(mapaR, mapaC) {
    instante = 0;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   */
  Action think(Sensores sensores);



  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  // Funciones específicas para cada nivel del técnico
  /**
   * @brief Implementación del comportamiento reactivo para el Nivel 0 (Técnico).
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // PRIMITIVAS SENSORIALES-MOTORAS (PROPORCIONADAS - NO MODIFICAR)
  // =========================================================================

  void ActualizarMapa(Sensores sensores);
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const estadoT &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  estadoT Delante(const estadoT &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto (camino, sendero, etc).
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es una casilla de tipo camino.
   */
  bool es_camino(unsigned char c) const;

  /**
   * @brief Traduce un índice del vector de sensores a coordenadas (fila, columna) reales.
   * Soporta las 8 orientaciones posibles.
   * @param sensor_idx Índice en el vector de sensores (0-15).
   * @param sensores Referencia a los sensores actuales.
   * @param[out] f Fila resultante.
   * @param[out] c Columna resultante.
   */
  void get_pos(int sensor_idx, const Sensores &sensores, int &f, int &c) const;

private:
  /// Última acción realizada con éxito por el técnico.
  Action last_action;
 /// Indica si el ingeniero posee actualmente las zapatillas.
  bool tiene_zapatillas; 
  /// Contador de instantes del técnico.
  int instante;
};


#endif
