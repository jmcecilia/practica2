#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>
#include <queue>

#include "comportamientos/comportamiento.hpp"

class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    alternar = false;
    esquinaDer = 0; // (Para detectar esquinas) --- IGNORE ---
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR, 
                         std::vector<std::vector<unsigned char>> mapaC): 
                         Comportamiento(mapaR, mapaC) {
    hayPlan = false;
    last_action = IDLE;
    tiene_zapatillas = false;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================
  /**
   * @brief Función para el pensar el movimiento del ingeniero.
   * @param casilla Carácter que representa la casilla a evaluar.
   * @param dif Diferencia de altura entre la casilla actual y la casilla a evaluar.
   * @param zap Indica si el agente tiene zapatillas.
   * @return Devuelve la mejor accion posible.
   */
  Action MejorAccion(char i, char c, char d, bool tiene_zapatillas, Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 0.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 1.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 2.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */ 
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 3.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 4.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 5.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 6.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  bool es_camino(unsigned char c) const;

  /**
 * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
 * @param plan  Lista de acciones del plan.
 */
  void PintaPlan(const list<Action> &plan);


/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 * @param plan  Lista de pasos (fila, columna, operación).
 */
  void PintaPlan(const list<Paso> &plan);


  /**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa gráfico.
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

  /**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
  void VisualizaRedTuberias(const list<Paso> &plan);



private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  struct EstadoI {
    int f;
    int c;
    Orientacion brujula;
    bool zapatillas;

    // Operador de igualdad para comparar si dos estados son exactamente iguales
    bool operator==(const EstadoI &st) const {
        return f == st.f && c == st.c && brujula == st.brujula && zapatillas == st.zapatillas;
    }

    // Operador de orden para poder usar EstadoI dentro de un std::set (explorados)
    bool operator<(const EstadoI &st) const {
        if (f < st.f) return true;
        if (f == st.f && c < st.c) return true;
        if (f == st.f && c == st.c && brujula < st.brujula) return true;
        if (f == st.f && c == st.c && brujula == st.brujula && zapatillas < st.zapatillas) return true;
        return false;
    }
};

struct NodoI {
    EstadoI estado;
    std::list<Action> secuencia;
};

//Variables nivel 0 y 1

  Action last_action;       // Última acción realizada 
  bool tiene_zapatillas;    // Indica si el agente posee zapatillas
  int giro45Izq;            // Contador de giros de 45 grados a la izquierda que pueden darse.
  bool alternar;
  int esquinaDer;          // Contador para detectar esquinas (incrementa al girar a la derecha, decrementa al girar a la izquierda)
  std::vector<std::vector<int>> mapa_visitas; // Mapa para contar el número de visitas a cada casilla (util nivel 1)

// Variables para nivel 2


// Variables de estado deliberativo
bool hayPlan;std::list<Action> plan;

  // =========================================================================
  // FUNCIONES DEL NIVEL 2
  // =========================================================================

  std::list<Action> B_Anchura_Ingeniero(const EstadoI &inicio, const EstadoI &final, 
                                        const std::vector<std::vector<unsigned char>> &mapaR, 
                                        const std::vector<std::vector<unsigned char>> &mapaC);

  EstadoI applyI(Action accion, const EstadoI &st, 
                 const std::vector<std::vector<unsigned char>> &mapaR, 
                 const std::vector<std::vector<unsigned char>> &mapaC);

  EstadoI NextCasillaI(const EstadoI &st, Action accion);

  bool CasillaAccesibleWalkI(const EstadoI &st, 
                             const std::vector<std::vector<unsigned char>> &mapaR, 
                             const std::vector<std::vector<unsigned char>> &mapaC);

  bool CasillaAccesibleJumpI(const EstadoI &st, 
                             const std::vector<std::vector<unsigned char>> &mapaR, 
                             const std::vector<std::vector<unsigned char>> &mapaC);

  //========================================================================
  // FUNCIONES DEL NIVEL 4
  //========================================================================

  struct estadoN4 {
    int fila;
    int columna;
    bool operator<(const estadoN4& otro) const {
        if (fila != otro.fila) return fila < otro.fila;
        return columna < otro.columna;
    }
    bool operator==(const estadoN4& otro) const {
        return fila == otro.fila && columna == otro.columna;
    }
};

struct ClaveCerrados {
    int fila;
    int columna;
    int h_efectiva; // La altura de la celda cuenta como un estado distinto

    bool operator<(const ClaveCerrados& otro) const {
        if (fila != otro.fila) return fila < otro.fila;
        if (columna != otro.columna) return columna < otro.columna;
        return h_efectiva < otro.h_efectiva;
    }
};

struct InfoVisitado {
    int g;
    int impacto;
    int energia; // ¡AÑADIR ESTO!
};

struct nodoN4 {
    estadoN4 st;
    std::list<Paso> secuencia; 
    int g;       
    int h;       
    int impacto; 
    int energia; // ¡AÑADIR ESTO!
    int h_efectiva; 
    
    int f() const { return g + h; }
    
    // Desempate: prioriza menor energía a igualdad de tramos
    bool operator<(const nodoN4& otro) const { 
        if (f() == otro.f()) return energia > otro.energia; 
        return f() > otro.f(); 
    }
};

// Y actualiza la cabecera de la función para recibir la batería
  std::list<Paso> dijkstra_nivel4(const estadoN4& inicio, const estadoN4& destino, int max_impacto, int max_energia);
  std::list<Paso> planTuberias; // Aquí guardaremos la ruta final

};

#endif
