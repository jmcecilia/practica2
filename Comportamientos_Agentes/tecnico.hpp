#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <map>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoTecnico
 * 
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Técnico en el mundo Belkan.
 * El técnico colabora con el ingeniero para resolver el problema de instalación de tuberías
 */



class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
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
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, 
                       std::vector<std::vector<unsigned char>> mapaC): 
                       Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    hay_plan = false;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  /**
   * @brief Función para decidir la mejor acción a tomar según las casillas accesibles.
   * @param i Carácter que representa la casilla izquierda.
   * @param c Carácter que representa la casilla central.           
   * @param d Carácter que representa la casilla derecha.
   */
  Action MejorAccion(char i, char c, char d, Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
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

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  bool tiene_zapatillas;
  int giro45Izq;
  Action last_action;
  bool alternar;            // (Para evitar rebotes en diagonales)
  int esquinaDer;            // Contador para detectar esquinas a la derecha (no utilizado en esta práctica, pero puede ser útil para niveles avanzados)
  std::vector<std::vector<int>> mapa_visitas; // Mapa para contar el número de visitas a cada casilla (util nivel 1)


  // Nivel 3
  struct Estado {
    int fila;
    int columna;
    int orientacion;
    bool zapatillas;

    // Sobrecarga necesaria para usar std::set (lista de Cerrados)
    bool operator<(const Estado& otro) const {
        if (fila != otro.fila) return fila < otro.fila;
        if (columna != otro.columna) return columna < otro.columna;
        if (orientacion != otro.orientacion) return orientacion < otro.orientacion;
        return zapatillas < otro.zapatillas;
    }
    
    // Sobrecarga para comparar si llegamos a la meta
    bool operator==(const Estado& otro) const {
        return fila == otro.fila && columna == otro.columna;
    }
  };

// Representa un nodo en el árbol de búsqueda A*
  struct Nodo {
    Estado st;
    std::list<Action> plan;
    int g; // Coste real acumulado (Energía)
    int h; // Heurística (Estimación)

    int f() const { return g + h; }

    // Sobrecarga para que la priority_queue actúe como Min-Heap (menor f primero)
    bool operator>(const Nodo& otro) const {
        return f() > otro.f();
    }
  };

    // Variables de control de plan
    std::list<Action> plan_actual;
    bool hay_plan;

    // Métodos para A*
    std::list<Action> AEstrella(const Estado& origen, const Estado& destino);
    int HeuristicaChebyshev(int filaAct, int colAct, int filaMeta, int colMeta);
    int CosteEnergiaTecnico(Action accion, char terreno_origen, int cota_origen, int cota_destino);
    bool EsTransitableNivel3(int f, int c, bool tiene_zapatillas);


    ////////////////////////////////////////////////////
    // Nivel 5
    ////////////////////////////////////////////////////

    // Estructuras para el A* de movimiento (Copia del Ingeniero)
    struct estadoNav {
      int f; int c; int rumbo;
      bool operator<(const estadoNav& o) const {
          if (f != o.f) return f < o.f;
          if (c != o.c) return c < o.c;
          return rumbo < o.rumbo;
      }
    };

    struct nodoNav {
      estadoNav st;
      std::list<Action> secuencia;
      int g; int h;
      int f_val() const { return g + h; }
      bool operator<(const nodoNav& o) const { return f_val() > o.f_val(); }
    };

    // Función de navegación segura
    std::list<Action> a_estrella_navegacion(int orig_f, int orig_c, int orig_rumbo, int dest_f, int dest_c, bool zap);

struct estadoN4 { int fila; int columna; bool operator<(const estadoN4& otro) const { if (fila != otro.fila) return fila < otro.fila; return columna < otro.columna; } bool operator==(const estadoN4& otro) const { return fila == otro.fila && columna == otro.columna; } };
struct ClaveCerrados { int fila; int columna; int h_efectiva; bool operator<(const ClaveCerrados& otro) const { if (fila != otro.fila) return fila < otro.fila; if (columna != otro.columna) return columna < otro.columna; return h_efectiva < otro.h_efectiva; } };
struct InfoVisitado { int g; int impacto; int energia; };
struct nodoN4 { estadoN4 st; std::list<Paso> secuencia; int g; int h; int impacto; int energia; int h_efectiva; int f() const { return g + h; } bool operator<(const nodoN4& otro) const { if (f() == otro.f()) return energia > otro.energia; return f() > otro.f(); } };

std::list<Paso> dijkstra_nivel4(const estadoN4& inicio, const estadoN4& destino, int max_impacto, int max_energia);
std::list<Paso> planTuberias;
int paso_idx_t = 1;
enum EstadoTecnico { 
        CALCULANDO_T, APARCANDO_T, NAVEGANDO_T, ESPERANDO_I, INSTALANDO_T , ACERCANDOSE_ING, ESPERANDO_LLAMADA, ACUDIENDO_LLAMADA
    };
    EstadoTecnico estado_tec = ACERCANDOSE_ING;

};

#endif
