#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores) {
  Action accion = IDLE;


  // Decisión del agente según el nivel
  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}

/** 
 * @brief Función para detectar casillas accesibles.
 * @param casilla Carácter que representa la casilla a evaluar.
 * @param dif Diferencia de altura entre la casilla actual y la casilla a evaluar.
 * @return Un carácter que indica si la casilla es accesible o no.
 */
char CasillaAccesibleT(char casilla, int dif){
  if (!(casilla == 'C' || casilla == 'D' || casilla == 'U' || casilla == 'S'))
    return 'P';

  if (abs(dif) <= 1 )
    return casilla;

  return 'P';
}

Action ComportamientoTecnico::MejorAccion(char i, char c, char d , Sensores sensores)
{
// Variable estática para recordar cuántos giros seguidos llevamos (memoria Nivel 0)
    static int consec_turns = 0;

    //  Maniobras evasivas
    if (giro45Izq > 0) { 
        giro45Izq--; 
        esquinaDer--; 
        consec_turns = 0; // Al esquivar reseteamos
        return TURN_SL; 
    }
    // Si el otro agente esta enfrente, giramos para esquivarlo
    if (sensores.agentes[2] == 'i') { 
        giro45Izq = 1; 
        esquinaDer--; 
        consec_turns = 0;
        return  TURN_SL; 
    }

    //  Prioridad: Meta
    if (c == 'U') { consec_turns = 0; return WALK; }
    if (i == 'U') { esquinaDer--; consec_turns++; return TURN_SL; }
    if (d == 'U') { esquinaDer++; consec_turns++; return TURN_SR; }

    //  Detecta giros de 360 grados para no quedarse en bucles grandes
    if (esquinaDer >= 8) {
        alternar = true;   
        esquinaDer = 0;
    } else if (esquinaDer <= -8) {
        alternar = false;  
        esquinaDer = 0;
    }

    bool es_diagonal = (sensores.rumbo % 2 != 0);  // Detecta si se esta mirando a una direccion diagonal
    bool embudo = es_diagonal && (i == 'P' && d == 'P'); // Detecta si se esta en un embudo (pared a ambos lados)

    Action accion_elegida = IDLE;

    // Movimiento siguiendo los muros dependiendo de la variable alternar, ademas de tener en cuenta los giros de 90 grados para no quedarse en bucles pequeños
     if (!alternar) {
        
        // Da prioridad de movimiento siguiendo el muro derecho
        if (d != 'P' && consec_turns < 2) {
            esquinaDer++;
            accion_elegida = TURN_SR;
        } else if (c != 'P' && !embudo) {
            accion_elegida = WALK;
        } else if (i != 'P') {
            esquinaDer--;
            accion_elegida = TURN_SL;
        } else {
            esquinaDer--;
            accion_elegida = TURN_SL; 
        }
    } else {
        // Da prioridad de movimiento siguiendo el muro izquierdo
        if (i != 'P' && consec_turns < 2) {
            esquinaDer--;
            accion_elegida = TURN_SL;
        } else if (c != 'P' && !embudo) {
            accion_elegida = WALK;
        } else if (d != 'P') {
            esquinaDer++;
            accion_elegida = TURN_SR;
        } else {
            esquinaDer++;
            accion_elegida = TURN_SR; 
        }
    }
    

    // Actualizar el contador de giros consecutivos
    if (accion_elegida == WALK) {
        consec_turns = 0; // Si caminamos, la cuenta vuelve a cero
    } else {
        consec_turns++;   // Si giramos, aumentamos la cuenta
    }

    return accion_elegida;
}


///////////////////////////////////////////////
// Nivel 3
///////////////////////////////////////////////

#include <cmath> // Necesario para la heurística euclídea

/**
 * QUÉ: Calcula la Distancia de Chebyshev.
 * CÓMO: Obteniendo el valor máximo entre la diferencia de filas y columnas.
 * POR QUÉ: En un mapa de 8 direcciones donde el coste diagonal es 1, 
 * Chebyshev es la única heurística matemática que garantiza admisibilidad.
 */
int ComportamientoTecnico::HeuristicaChebyshev(int filaAct, int colAct, int filaMeta, int colMeta) {
    int dFila = std::abs(filaMeta - filaAct);
    int dCol = std::abs(colMeta - colAct);
    return std::max(dFila, dCol);
}

int ComportamientoTecnico::CosteEnergiaTecnico(Action accion, char terreno_origen, int cota_origen, int cota_destino) {
    int coste = 0;
    
    if (accion == WALK) {
        if (terreno_origen == 'A') {
            coste = 60;
            if (cota_destino > cota_origen) coste += 5;
            else if (cota_destino < cota_origen) coste -= 2;
        }
        else if (terreno_origen == 'H') {
            coste = 6;
            if (cota_destino > cota_origen) coste += 5;
            else if (cota_destino < cota_origen) coste -= 2;
        }
        else if (terreno_origen == 'S') {
            coste = 3;
            if (cota_destino > cota_origen) coste += 5;
            else if (cota_destino < cota_origen) coste -= 2;
        }
        else {
            // "Resto de casillas" ('C', 'U', 'D', 'B' con zapatillas)
            // Según la tabla, el incremento/decremento es +0 / -0
            coste = 1; 
        }
        
        // Seguridad: El coste de avanzar nunca debe ser menor o igual a 0.
        if (coste < 1) coste = 1;
        
    } else if (accion == TURN_SL || accion == TURN_SR) {
        // Los giros no sufren cambios por altura, solo depende del terreno de origen
        if (terreno_origen == 'A') coste = 5;
        else if (terreno_origen == 'H') coste = 2;
        else if (terreno_origen == 'S') coste = 1;
        else coste = 1;
    }
    
    return coste;
}

/**
 * QUÉ: Verifica si una casilla puede ser pisada.
 * CÓMO: Descarta precipicios ('P'), muros ('M') y valida la restricción del bosque.
 * POR QUÉ: Previene que el algoritmo genere ramas inútiles hacia obstáculos o zonas letales.
 */
bool ComportamientoTecnico::EsTransitableNivel3(int f, int c, bool tiene_zapatillas) {
    if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
    char terreno = mapaResultado[f][c];
    
    if (terreno == 'P' || terreno == 'M') return false;
    // Si es bosque y no tiene zapatillas, no es transitable
    if (terreno == 'B' && !tiene_zapatillas) return false;
    
    return true;
}


/**
 * QUÉ: Algoritmo de búsqueda A* para encontrar la ruta óptima de energía.
 * CÓMO: Expandiendo nodos mediante una cola de prioridad, evaluando WALK, TURN_SL y TURN_SR.
 * POR QUÉ: Es el requisito principal del Nivel 3. Garantiza encontrar el camino que consume menos batería.
 */
std::list<Action> ComportamientoTecnico::AEstrella(const Estado& origen, const Estado& destino) {
    std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> ABIERTA;
    std::set<Estado> CERRADOS;

    Nodo inicial;
    inicial.st = origen;
    inicial.g = 0;
    inicial.h = HeuristicaChebyshev(origen.fila, origen.columna, destino.fila, destino.columna);
    ABIERTA.push(inicial);

    while (!ABIERTA.empty()) {
        Nodo actual = ABIERTA.top();
        ABIERTA.pop();

        // 1. ¿Hemos llegado a la meta? (Compara coordenadas, no orientación ni ítems)
        if (actual.st == destino) {
            return actual.plan;
        }

        // 2. ¿Ya evaluamos este estado exacto antes?
        if (CERRADOS.find(actual.st) != CERRADOS.end()) {
            continue;
        }
        CERRADOS.insert(actual.st);

        // 3. Generar Hijos
        char terreno_actual = mapaResultado[actual.st.fila][actual.st.columna];
        int cota_actual = mapaCotas[actual.st.fila][actual.st.columna];

        // --- ACCIÓN: AVANZAR (WALK) ---
        Estado estado_walk = actual.st;
        // Calculamos la coordenada frontal dependiendo de la brújula
        ubicacion ubi_actual; 
        ubi_actual.f = actual.st.fila; 
        ubi_actual.c = actual.st.columna; 
        // Hacemos un "cast" explícito de int a Orientacion
        ubi_actual.brujula = static_cast<Orientacion>(actual.st.orientacion);
        ubicacion ubi_delante = Delante(ubi_actual);
        
        estado_walk.fila = ubi_delante.f;
        estado_walk.columna = ubi_delante.c;

        if (EsTransitableNivel3(estado_walk.fila, estado_walk.columna, estado_walk.zapatillas)) {
            int cota_destino = mapaCotas[estado_walk.fila][estado_walk.columna];
            
            // Restricción Técnica: Desnivel máximo absoluto de 1
            if (std::abs(cota_destino - cota_actual) <= 1) {
                // Si llegamos a una casilla 'D', recogemos zapatillas
                if (mapaResultado[estado_walk.fila][estado_walk.columna] == 'D') {
                    estado_walk.zapatillas = true;
                }

                if (CERRADOS.find(estado_walk) == CERRADOS.end()) {
                    Nodo hijo_walk;
                    hijo_walk.st = estado_walk;
                    hijo_walk.plan = actual.plan;
                    hijo_walk.plan.push_back(WALK);
                    hijo_walk.g = actual.g + CosteEnergiaTecnico(WALK, terreno_actual, cota_actual, cota_destino);
                    hijo_walk.h = HeuristicaChebyshev(estado_walk.fila, estado_walk.columna, destino.fila, destino.columna);
                    ABIERTA.push(hijo_walk);
                }
            }
        }

        // --- ACCIÓN: GIRAR IZQUIERDA (TURN_SL) ---
        Estado estado_sl = actual.st;
        estado_sl.orientacion = (estado_sl.orientacion + 7) % 8; // Girar -45 grados
        if (CERRADOS.find(estado_sl) == CERRADOS.end()) {
            Nodo hijo_sl;
            hijo_sl.st = estado_sl;
            hijo_sl.plan = actual.plan;
            hijo_sl.plan.push_back(TURN_SL);
            hijo_sl.g = actual.g + CosteEnergiaTecnico(TURN_SL, terreno_actual, cota_actual, cota_actual);
            hijo_sl.h = actual.h; // La heurística no cambia porque no me moví de casilla
            ABIERTA.push(hijo_sl);
        }

        // --- ACCIÓN: GIRAR DERECHA (TURN_SR) ---
        Estado estado_sr = actual.st;
        estado_sr.orientacion = (estado_sr.orientacion + 1) % 8; // Girar +45 grados
        if (CERRADOS.find(estado_sr) == CERRADOS.end()) {
            Nodo hijo_sr;
            hijo_sr.st = estado_sr;
            hijo_sr.plan = actual.plan;
            hijo_sr.plan.push_back(TURN_SR);
            hijo_sr.g = actual.g + CosteEnergiaTecnico(TURN_SR, terreno_actual, cota_actual, cota_actual);
            hijo_sr.h = actual.h;
            ABIERTA.push(hijo_sr);
        }
    }

    // Retorna una lista vacía si no hay solución
    return std::list<Action>();
}


Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {
   // Actualizar el mapa interno con la información de los sensores
  ActualizarMapa(sensores);
  
  //  actualiza sus zapatillas aquí
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // Definicion del comportamiento del agente 
  if (sensores.superficie[0] == 'U'){ 
    return IDLE; // Llegamos a la meta
  }
  
  // Calculamos la accesibilidad REAL usando la cota y las zapatillas
  char i = CasillaAccesibleT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = CasillaAccesibleT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = CasillaAccesibleT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  // Llamar al motor de exploración pasándole el estado de las zapatillas
  Action accion = MejorAccion(i, c, d, sensores);

  last_action = accion;
  return accion;
}
/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
 if (mapa_visitas.empty()) {
        mapa_visitas.assign(mapaResultado.size(), std::vector<int>(mapaResultado[0].size(), 0));
    }

    ActualizarMapa(sensores);
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    static int f_ant_t = -1, c_ant_t = -1;
    if (sensores.posF != f_ant_t || sensores.posC != c_ant_t) {
        mapa_visitas[sensores.posF][sensores.posC]++;
        f_ant_t = sensores.posF;
        c_ant_t = sensores.posC;
    }

    char i = CasillaAccesibleT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
    char c = CasillaAccesibleT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
    char d = CasillaAccesibleT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

    ubicacion actual;
    actual.f = sensores.posF; actual.c = sensores.posC; actual.brujula = sensores.rumbo;

    ubicacion u_izq = actual; u_izq.brujula = (Orientacion)(((int)actual.brujula + 7) % 8); u_izq = Delante(u_izq);
    ubicacion u_cen = Delante(actual);
    ubicacion u_der = actual; u_der.brujula = (Orientacion)(((int)actual.brujula + 1) % 8); u_der = Delante(u_der);

    int v_izq = (i != 'P' && sensores.agentes[1] == '_') ? mapa_visitas[u_izq.f][u_izq.c] : 999999;
    int v_cen = (c != 'P' && sensores.agentes[2] == '_') ? mapa_visitas[u_cen.f][u_cen.c] : 999999;
    int v_der = (d != 'P' && sensores.agentes[3] == '_') ? mapa_visitas[u_der.f][u_der.c] : 999999;

    int score_izq = (v_izq == 999999) ? 999999 : (v_izq * 1000);
    int score_cen = (v_cen == 999999) ? 999999 : (v_cen * 1000);
    int score_der = (v_der == 999999) ? 999999 : (v_der * 1000);

    if (score_izq != 999999) {
        if (i == 'C') score_izq -= 30;
        else if (i == 'S') score_izq -= 20;
        // El Técnico no recibe bonificación a la izquierda
    }
    if (score_cen != 999999) {
        if (c == 'C') score_cen -= 30;
        else if (c == 'S') score_cen -= 20;
        score_cen -= 5;
    }
    if (score_der != 999999) {
        if (d == 'C') score_der -= 30;
        else if (d == 'S') score_der -= 20;
        score_der -= 2; // Personalidad de abrazo a la derecha
    }

    Action accion = TURN_SR; 
    int min_score = 999999;

    if (score_cen < min_score) { min_score = score_cen; accion = WALK; }
    if (score_izq < min_score) { min_score = score_izq; accion = TURN_SL; }
    if (score_der < min_score) { min_score = score_der; accion = TURN_SR; }

    last_action = accion;
    return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  // Si acabamos de encontrar zapatillas pisándolas, actualizamos el estado global
    if (sensores.superficie[0] == 'D') {
        tiene_zapatillas = true;
    }

    // Si ya estamos en la meta, nos quedamos quietos
    if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC) {
        return IDLE;
    }

    // Si no tenemos un plan, lo calculamos mediante A*
    if (!hay_plan) {
        Estado origen;
        origen.fila = sensores.posF;
        origen.columna = sensores.posC;
        origen.orientacion = sensores.rumbo;
        origen.zapatillas = tiene_zapatillas;

        Estado destino;
        destino.fila = sensores.BelPosF;
        destino.columna = sensores.BelPosC;
        // La orientación y zapatillas en el destino no importan para el == sobrecargado

        plan_actual = AEstrella(origen, destino);
        
        if (!plan_actual.empty()) {
            hay_plan = true;
            // Opcional: Puedes descomentar para visualizar el plan en el entorno gráfico
            // ubicacion st_origen; st_origen.f = sensores.posF; st_origen.c = sensores.posC; st_origen.brujula = sensores.rumbo;
            // VisualizaPlan(st_origen, plan_actual);
        } else {
            // Si A* devuelve vacío, no hay camino posible
            return IDLE; 
        }
    }

    // Si tenemos un plan, ejecutamos la siguiente acción
    if (hay_plan && !plan_actual.empty()) {
        Action sig_accion = plan_actual.front();
        plan_actual.pop_front();
        
        // Si el plan se acaba, hay_plan vuelve a false
        if (plan_actual.empty()) {
            hay_plan = false;
        }
        
        last_action = sig_accion;
        return sig_accion;
    }

    return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}




// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
          mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
        }
      break;
    case noreste:
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
      mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
      mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
      mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
      mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
      mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
      mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
      mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
      mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
          mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
        }
      break;
    case sureste:
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
      mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
      mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
      mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
      mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
      mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
      mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
      mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
      mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
      mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
      mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
          mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
        }
      break;
    case suroeste:
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
      mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
      mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
      mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
      mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
      mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
      mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
      mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
      mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
      break;
    case oeste:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
          mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
        }
      break;
    case noroeste:
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
      mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
      mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
      mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
      mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
      mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
      mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
      mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
      mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
      mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
      mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
      break;
  }
}



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
   listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}


