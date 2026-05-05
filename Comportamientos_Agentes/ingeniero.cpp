#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

// =========================================================
// Nivel 0 y 1
// =========================================================



/** 
 * @brief Función para detectar casillas accesibles.
 * @param casilla Carácter que representa la casilla a evaluar.
 * @param dif Diferencia de altura entre la casilla actual y la casilla a evaluar.
 * @param zap Indica si el agente tiene zapatillas.
 * @return Un carácter que indica si la casilla es accesible o no.
 */
char CasillaAccesibleI(char casilla, int dif, bool zap){
  if (!(casilla == 'C' || casilla == 'D' || casilla == 'U' || casilla == 'S'))
    return 'P';

  if (abs(dif) <= 1 || (zap && abs(dif) <= 2))
    return casilla;

  return 'P';
}

/**
 * @brief Función para decidir la mejor acción a tomar según las casillas accesibles.
 * @param i Carácter que representa la casilla izquierda.
 * @param c Carácter que representa la casilla central.
 * @param d Carácter que representa la casilla derecha.
 * @param tiene_zapatillas Indica si el agente posee zapatillas.
 * @return La acción recomendada para el agente.
 */
Action ComportamientoIngeniero::MejorAccion(char i, char c, char d, bool tiene_zapatillas , Sensores sensores)
{
  // Variable estática para recordar cuántos giros seguidos llevamos (memoria Nivel 0)
    static int consec_turns = 0;

    // Maniobras evasivas
    if (giro45Izq > 0) { 
        giro45Izq--; 
        esquinaDer--; 
        consec_turns = 0; // Al esquivar reseteamos
        return TURN_SL; 
    }
    // Si el otro agente esta enfrente, giramos para esquivarlo
    if (sensores.agentes[2] == 't') { 
        giro45Izq = 1; 
        esquinaDer--; 
        consec_turns = 0;
        return TURN_SL; 
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

    bool es_diagonalI = (sensores.rumbo % 2 != 0); // Detecta si se esta mirando a una direccion diagonal
    bool embudoI = es_diagonalI && (i == 'P' && d == 'P'); // Detecta si se esta en un embudo (pared a ambos lados)
    Action accion_elegida = IDLE;

    // Movimiento siguiendo los muros dependiendo de la variable alternar, ademas de tener en cuenta los giros de 90 grados para no quedarse en bucles pequeños
    if (!alternar) {
        
        // Da prioridad de movimiento siguiendo el muro derecho
        if (d != 'P' && consec_turns < 2) {
            esquinaDer++;
            accion_elegida = TURN_SR;
        } else if (c != 'P' && !embudoI) {
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
        } else if (c != 'P' && !embudoI) {
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

// =========================================================
// Nivel 2
// =========================================================

// --- 1. Funciones de Transición de Estado ---
ComportamientoIngeniero::EstadoI ComportamientoIngeniero::NextCasillaI(const EstadoI &st, Action accion) {
    EstadoI siguiente = st;

    if (accion == TURN_SR) {
        siguiente.brujula = static_cast<Orientacion>((siguiente.brujula + 1) % 8);
    } else if (accion == TURN_SL) {
        siguiente.brujula = static_cast<Orientacion>((siguiente.brujula + 7) % 8);
    } else if (accion == WALK || accion == JUMP) {
        int pasos = (accion == WALK) ? 1 : 2;
        switch (st.brujula) {
            case norte:    siguiente.f -= pasos; break;
            case noreste:  siguiente.f -= pasos; siguiente.c += pasos; break;
            case este:     siguiente.c += pasos; break;
            case sureste:  siguiente.f += pasos; siguiente.c += pasos; break;
            case sur:      siguiente.f += pasos; break;
            case suroeste: siguiente.f += pasos; siguiente.c -= pasos; break;
            case oeste:    siguiente.c -= pasos; break;
            case noroeste: siguiente.f -= pasos; siguiente.c -= pasos; break;
        }
    }
    return siguiente;
}

bool ComportamientoIngeniero::CasillaAccesibleWalkI(const EstadoI &st, const std::vector<std::vector<unsigned char>> &mapaR, const std::vector<std::vector<unsigned char>> &mapaC) {
    EstadoI next = NextCasillaI(st, WALK);
    
    // 1. Comprobar límites
    if (next.f < 0 || next.f >= mapaR.size() || next.c < 0 || next.c >= mapaR[0].size()) return false;
    
    unsigned char terreno = mapaR[next.f][next.c];
    
    // No puede pisar Precipicios ('P'), Muros ('M') NI Bosque ('B').
    if (terreno == 'P' || terreno == 'M' || terreno == 'B') return false; 
    
    // 3. Diferencia de altura (1 sin zapatillas, 2 con zapatillas)
    int dif = mapaC[next.f][next.c] - mapaC[st.f][st.c];
    if (abs(dif) <= 1 || (st.zapatillas && abs(dif) <= 2)) return true;
    
    return false;
}

bool ComportamientoIngeniero::CasillaAccesibleJumpI(const EstadoI &st, const std::vector<std::vector<unsigned char>> &mapaR, const std::vector<std::vector<unsigned char>> &mapaC) {
    EstadoI mid = NextCasillaI(st, WALK);
    EstadoI fin = NextCasillaI(st, JUMP);

    if (fin.f < 0 || fin.f >= mapaR.size() || fin.c < 0 || fin.c >= mapaR[0].size()) return false;

    unsigned char terrMid = mapaR[mid.f][mid.c];
    unsigned char terrFin = mapaR[fin.f][fin.c];

    // 1. La casilla intermedia DEBE ser transitable. 
    // No miramos su cota porque "La altura de la Casilla Intermedia no afecta".
    if (terrMid == 'P' || terrMid == 'M' || terrMid == 'B') return false;

    // 2. La casilla destino DEBE ser transitable.
    if (terrFin == 'P' || terrFin == 'M' || terrFin == 'B') return false;
    
    // 3. La diferencia de altura es solo entre Inicial y Destino.
    int dif = mapaC[fin.f][fin.c] - mapaC[st.f][st.c];
    if (abs(dif) <= 1 || (st.zapatillas && abs(dif) <= 2)) return true;

    return false;
}

ComportamientoIngeniero::EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st, const std::vector<std::vector<unsigned char>> &mapaR, const std::vector<std::vector<unsigned char>> &mapaC) {
    EstadoI next = st;

    if (accion == WALK && CasillaAccesibleWalkI(st, mapaR, mapaC)) {
        next = NextCasillaI(st, WALK);
    } else if (accion == JUMP && CasillaAccesibleJumpI(st, mapaR, mapaC)) {
        next = NextCasillaI(st, JUMP);
    } else if (accion == TURN_SL || accion == TURN_SR) {
        next = NextCasillaI(st, accion);
    }

    // Actualizar si recogimos zapatillas en el nuevo estado
    if (mapaR[next.f][next.c] == 'D') {
        next.zapatillas = true;
    }

    return next;
}

// --- 2. Algoritmo de Búsqueda en Anchura ---
std::list<Action> ComportamientoIngeniero::B_Anchura_Ingeniero(const EstadoI &inicio, const EstadoI &final, const std::vector<std::vector<unsigned char>> &mapaR, const std::vector<std::vector<unsigned char>> &mapaC) {
    
    std::queue<NodoI> frontier; // Cola de nodos por explorar
    std::set<EstadoI> explored; // Conjunto eficiente para estados visitados
    std::list<Action> path;

    NodoI current_node;
    current_node.estado = inicio;
    frontier.push(current_node);
    explored.insert(inicio);

    bool SolutionFound = (inicio.f == final.f && inicio.c == final.c);

    Action posibles_acciones[] = {WALK, JUMP, TURN_SR, TURN_SL}; // Priorizamos avanzar frente a girar

    while (!SolutionFound && !frontier.empty()) {
        current_node = frontier.front();
        frontier.pop();

        for (Action accion : posibles_acciones) {
            EstadoI estado_hijo = applyI(accion, current_node.estado, mapaR, mapaC);

            // Verificamos si el estado generado es diferente al actual (acción válida) y no ha sido explorado
            if (!(estado_hijo == current_node.estado) && explored.find(estado_hijo) == explored.end()) {
                
                NodoI hijo;
                hijo.estado = estado_hijo;
                hijo.secuencia = current_node.secuencia;
                hijo.secuencia.push_back(accion);

                if (estado_hijo.f == final.f && estado_hijo.c == final.c) {
                    SolutionFound = true;
                    path = hijo.secuencia;
                    break;
                }

                explored.insert(estado_hijo);
                frontier.push(hijo);
            }
        }
    }

    return path;
}


// =========================================================
// Funciones Nivel 4
// =========================================================

std::list<Paso> ComportamientoIngeniero::dijkstra_nivel4(const estadoN4& inicio, const estadoN4& destino, int max_impacto, int max_energia) {
    std::priority_queue<nodoN4> Abiertos;
    std::map<ClaveCerrados, std::vector<InfoVisitado>> Cerrados;

    auto getCosteEcoLocal = [](int accion, unsigned char celda) -> int {
        switch (accion) {
            case 0: // INSTALL
                if (celda == 'A') return 50;
                if (celda == 'H') return 45;
                if (celda == 'S') return 25;
                if (celda == 'C' || celda == 'U') return 15;
                return 30;
            case 1: // RAISE
                if (celda == 'A') return 1000000;
                if (celda == 'H') return 55;
                if (celda == 'S') return 30;
                if (celda == 'C' || celda == 'U') return 10;
                return 40;
            case -1: // DIG
                if (celda == 'A') return 1000000;
                if (celda == 'H') return 65;
                if (celda == 'S') return 40;
                if (celda == 'C' || celda == 'U') return 25;
                return 50;
            default: return 0;
        }
    };

    auto getCosteEnergiaLocal = [](int accion, unsigned char celda) -> int {
        switch (accion) {
            case 0: // INSTALL
                if (celda == 'A') return 60;
                if (celda == 'H') return 45;
                if (celda == 'S') return 25;
                if (celda == 'C' || celda == 'U') return 15;
                return 30;
            case 1: // RAISE
                if (celda == 'A') return 1000000;
                if (celda == 'H') return 55;
                if (celda == 'S') return 30;
                if (celda == 'C' || celda == 'U') return 10;
                return 40;
            case -1: // DIG
                if (celda == 'A') return 1000000;
                if (celda == 'H') return 65;
                if (celda == 'S') return 40;
                if (celda == 'C' || celda == 'U') return 25;
                return 50;
            default: return 0;
        }
    };

    char terrOrigen = mapaResultado[inicio.fila][inicio.columna];
    int h_origen = mapaCotas[inicio.fila][inicio.columna];

    int operaciones_inicio[3] = {0, -1, 1};
    for (int op : operaciones_inicio) {
        if (terrOrigen == 'A' && op != 0) continue; 
        if (terrOrigen == 'P' || terrOrigen == 'M' || terrOrigen == 'B') continue;

        int impacto_init = 0;
        int energia_init = 0;
        if (op != 0) {
            impacto_init = getCosteEcoLocal(op, terrOrigen);
            energia_init = getCosteEnergiaLocal(op, terrOrigen);
        }
        
        // PRIMERA DOBLE PODA (En la casilla de salida)
        if (impacto_init > max_impacto || energia_init > max_energia) continue;

        nodoN4 inicial;
        inicial.st = inicio;
        inicial.g = 0; 
        inicial.impacto = impacto_init;
        inicial.energia = energia_init; // Asignamos la energía
        inicial.h_efectiva = h_origen + op;
        inicial.h = 0; 
        inicial.secuencia.push_back({inicio.fila, inicio.columna, op});
        
        Abiertos.push(inicial);
    }

    int dF[] = {-1, 0, 1, 0};
    int dC[] = {0, 1, 0, -1};

    while (!Abiertos.empty()) {
        nodoN4 actual = Abiertos.top();
        Abiertos.pop();

        if (mapaResultado[actual.st.fila][actual.st.columna] == 'U') {
            std::cout << "--- PLAN ENCONTRADO (DIJKSTRA) ---" << std::endl;
            std::cout << "Longitud red: " << actual.g << " tramos." << std::endl;
            std::cout << "Impacto: " << actual.impacto << " / " << max_impacto << std::endl;
            std::cout << "Energia: " << actual.energia << " / " << max_energia << std::endl;
            return actual.secuencia; 
        }

        ClaveCerrados clave = {actual.st.fila, actual.st.columna, actual.h_efectiva};
        bool dominado = false;
        for (const auto& visitado : Cerrados[clave]) {
            // Un nodo está dominado SÓLO si empeora o iguala en LAS TRES variables
            if (visitado.g <= actual.g && visitado.impacto <= actual.impacto && visitado.energia <= actual.energia) {
                dominado = true;
                break;
            }
        }
        if (dominado) continue;
        
        // Importante: Guardar las 3 variables en InfoVisitado
        Cerrados[clave].push_back({actual.g, actual.impacto, actual.energia});

        for (int i = 0; i < 4; ++i) {
            estadoN4 sig = {actual.st.fila + dF[i], actual.st.columna + dC[i]};

            if (sig.fila < 0 || sig.fila >= mapaResultado.size() ||
                sig.columna < 0 || sig.columna >= mapaResultado[0].size()) continue;

            char terrSig = mapaResultado[sig.fila][sig.columna];
            if (terrSig == 'P' || terrSig == 'M' || terrSig == 'B') continue; 
            
            char terrActual = mapaResultado[actual.st.fila][actual.st.columna];
            int h_sig_orig = mapaCotas[sig.fila][sig.columna];

            int operaciones[3] = {0, -1, 1}; 
            for (int op : operaciones) {
                if (terrSig == 'A' && op != 0) continue; 
                
                int h_sig_efectiva = h_sig_orig + op;
                
                if (h_sig_efectiva == actual.h_efectiva || h_sig_efectiva == actual.h_efectiva ) {  // +1 porque al ir el ingeniero delante puede hacer el install mirando al tecnico
                    
                    int delta_impacto = getCosteEcoLocal(0, terrActual) + getCosteEcoLocal(0, terrSig);
                    int delta_energia = getCosteEnergiaLocal(0, terrActual) + getCosteEnergiaLocal(0, terrSig);
                    
                    if (op != 0) {
                        delta_impacto += getCosteEcoLocal(op, terrSig); 
                        delta_energia += getCosteEnergiaLocal(op, terrSig); 
                    }
                    
                    if (delta_impacto >= 1000000 || delta_energia >= 1000000) continue; 

                    int nuevo_impacto = actual.impacto + delta_impacto;
                    int nueva_energia = actual.energia + delta_energia;
                    
                    // SEGUNDA DOBLE PODA IMPLACABLE
                    if (nuevo_impacto > max_impacto) continue; 
                    if (nueva_energia > max_energia) continue; 

                    nodoN4 hijo = actual;
                    hijo.st = sig;
                    hijo.g = actual.g + 1;  
                    hijo.impacto = nuevo_impacto;
                    hijo.energia = nueva_energia; // Asignamos la nueva energía simulada
                    hijo.h_efectiva = h_sig_efectiva;
                    hijo.h = 0; 
                    hijo.secuencia.push_back({sig.fila, sig.columna, op});
                    
                    Abiertos.push(hijo);
                }
            }
        }
    }
    return std::list<Paso>(); 
}

// =========================================================
// Nivel 5 
// =========================================================

std::list<Action> ComportamientoIngeniero::a_estrella_navegacion(int orig_f, int orig_c, int orig_rumbo, int dest_f, int dest_c, bool zap) {
    std::priority_queue<nodoNav> Abiertos;
    std::set<estadoNav> Cerrados;

    nodoNav inicial;
    inicial.st = {orig_f, orig_c, orig_rumbo};
    inicial.g = 0;
    inicial.h = std::abs(orig_f - dest_f) + std::abs(orig_c - dest_c);
    Abiertos.push(inicial);

    auto esTransitable = [&](int f, int c) {
        if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
        char terr = mapaResultado[f][c];
        if (terr == 'M' || terr == 'P') return false;
        if (terr == 'B' ) return false;
        return true;
    };

    while (!Abiertos.empty()) {
        nodoNav actual = Abiertos.top();
        Abiertos.pop();

        if (actual.st.f == dest_f && actual.st.c == dest_c) {
            return actual.secuencia; 
        }

        if (Cerrados.find(actual.st) != Cerrados.end()) continue;
        Cerrados.insert(actual.st);

        // Giros
        nodoNav hijoL = actual; hijoL.st.rumbo = (actual.st.rumbo + 7) % 8; hijoL.g += 1; hijoL.secuencia.push_back(TURN_SL); Abiertos.push(hijoL);
        nodoNav hijoR = actual; hijoR.st.rumbo = (actual.st.rumbo + 1) % 8; hijoR.g += 1; hijoR.secuencia.push_back(TURN_SR); Abiertos.push(hijoR);

        // --- MOVIMIENTOS ---
        ubicacion ubi_act = {actual.st.f, actual.st.c, (Orientacion)actual.st.rumbo};
        ubicacion f1 = Delante(ubi_act);

        // Solo podemos avanzar si la casilla frontal es físicamente transitable
        if (esTransitable(f1.f, f1.c)) {
            
            // 1. LÓGICA DE WALK
            int difW = mapaCotas[f1.f][f1.c] - mapaCotas[actual.st.f][actual.st.c];
            if (std::abs(difW) <= 1 || (zap && std::abs(difW) <= 2)) {
                nodoNav hijoW = actual;
                hijoW.st.f = f1.f; hijoW.st.c = f1.c;
                hijoW.g += 1; 
                hijoW.h = std::abs(f1.f - dest_f) + std::abs(f1.c - dest_c);
                hijoW.secuencia.push_back(WALK);
                Abiertos.push(hijoW);
            }

            // 2. LÓGICA DE JUMP (Basada en tus normas)
            // Como f1 ya es transitable (condición cumplida), miramos a f2
            ubicacion f2 = Delante(f1);
            if (esTransitable(f2.f, f2.c)) {
                // La diferencia de altura en JUMP se calcula solo entre Destino e Inicial
                int difJ = mapaCotas[f2.f][f2.c] - mapaCotas[actual.st.f][actual.st.c];
                
                if (std::abs(difJ) <= 1 || (zap && std::abs(difJ) <= 2)) {
                    nodoNav hijoJ = actual;
                    hijoJ.st.f = f2.f; hijoJ.st.c = f2.c;
                    hijoJ.g += 2; // Cuesta un poco más para evitar que salte si no es estrictamente necesario
                    hijoJ.h = std::abs(f2.f - dest_f) + std::abs(f2.c - dest_c);
                    hijoJ.secuencia.push_back(JUMP);
                    Abiertos.push(hijoJ);
                }
            }
        }
    }
    return std::list<Action>();
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores){
  // Actualizar el mapa interno con la información de los sensores
  ActualizarMapa(sensores);
  
  // OJO: El ingeniero actualiza sus zapatillas aquí
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // Definicion del comportamiento del agente 
  if (sensores.superficie[0] == 'U'){ 
    return IDLE; // Llegamos a la meta
  }
  
  // Calculamos la accesibilidad REAL usando la cota y las zapatillas
  char i = CasillaAccesibleI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = CasillaAccesibleI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = CasillaAccesibleI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  // Llamar al motor de exploración pasándole el estado de las zapatillas
  Action accion = MejorAccion(i, c, d, tiene_zapatillas, sensores);

  last_action = accion;
  return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
 if (mapa_visitas.empty()) {
        mapa_visitas.assign(mapaResultado.size(), std::vector<int>(mapaResultado[0].size(), 0));
    }

    ActualizarMapa(sensores);
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    // Solo sumar visita al cambiar de casilla físicamente
    static int f_ant_i = -1, c_ant_i = -1;
    if (sensores.posF != f_ant_i || sensores.posC != c_ant_i) {
        mapa_visitas[sensores.posF][sensores.posC]++;
        f_ant_i = sensores.posF;
        c_ant_i = sensores.posC;
    }

    char i = CasillaAccesibleI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
    char c = CasillaAccesibleI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
    char d = CasillaAccesibleI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

    ubicacion actual;
    actual.f = sensores.posF; actual.c = sensores.posC; actual.brujula = sensores.rumbo;

    ubicacion u_izq = actual; u_izq.brujula = (Orientacion)(((int)actual.brujula + 7) % 8); u_izq = Delante(u_izq);
    ubicacion u_cen = Delante(actual);
    ubicacion u_der = actual; u_der.brujula = (Orientacion)(((int)actual.brujula + 1) % 8); u_der = Delante(u_der);

    // Los agentes y los muros son obstáculos insalvables
    int v_izq = (i != 'P' && sensores.agentes[1] == '_') ? mapa_visitas[u_izq.f][u_izq.c] : 999999;
    int v_cen = (c != 'P' && sensores.agentes[2] == '_') ? mapa_visitas[u_cen.f][u_cen.c] : 999999;
    int v_der = (d != 'P' && sensores.agentes[3] == '_') ? mapa_visitas[u_der.f][u_der.c] : 999999;


    // SISTEMA ESTRICTO DE PUNTUACIÓN (Menor puntuación = Mejor ruta)
    int score_izq = (v_izq == 999999) ? 999999 : (v_izq * 1000);
    int score_cen = (v_cen == 999999) ? 999999 : (v_cen * 1000);
    int score_der = (v_der == 999999) ? 999999 : (v_der * 1000);

    // Bonificaciones (solo si no es pared)
    if (score_izq != 999999) {
        if (i == 'C') score_izq -= 30;
        else if (i == 'S') score_izq -= 20;
        score_izq -= 2; // INGENIERO: Personalidad de abrazo a la IZQUIERDA
    }
    if (score_cen != 999999) {
        if (c == 'C') score_cen -= 30;
        else if (c == 'S') score_cen -= 20;
        score_cen -= 5; // Preferencia universal por no zigzaguear (ir recto)
    }
    if (score_der != 999999) {
        if (d == 'C') score_der -= 30;
        else if (d == 'S') score_der -= 20;
        // El Ingeniero no recibe bonificación a la derecha
    }

    Action accion = TURN_SR; // Acción por defecto si esta en un callejon sin salida
    int min_score = 999999;

    // Elegimos la ruta con menor puntuación total
    if (score_cen < min_score) { min_score = score_cen; accion = WALK; }
    if (score_izq < min_score) { min_score = score_izq; accion = TURN_SL; }
    if (score_der < min_score) { min_score = score_der; accion = TURN_SR; }

    last_action = accion;
    return accion;
}

// Niveles avanzados (Uso de búsqueda)
/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
    Action accion = IDLE;

    if (!hayPlan) {
        // Configuramos el estado de inicio
        EstadoI inicio;
        inicio.f = sensores.posF;
        inicio.c = sensores.posC;
        inicio.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas;
        // Verificar si nacimos sobre las zapatillas
        if (mapaResultado[sensores.posF][sensores.posC] == 'D') tiene_zapatillas = true;

        // Configuramos el estado objetivo (donde está la filtración)
        EstadoI objetivo;
        objetivo.f = sensores.BelPosF;
        objetivo.c = sensores.BelPosC;

        // Calculamos la ruta
        plan = B_Anchura_Ingeniero(inicio, objetivo, mapaResultado, mapaCotas);
        
        if (!plan.empty()) {
            // CORRECCIÓN: Empaquetar posición en 'ubicacion' para VisualizaPlan
            ubicacion u_inicio;
            u_inicio.f = sensores.posF;
            u_inicio.c = sensores.posC;
            u_inicio.brujula = sensores.rumbo;
            
            VisualizaPlan(u_inicio, plan); 
            hayPlan = true;
        }
    }

    // Si tenemos plan y no está vacío, ejecutamos la siguiente acción
    if (hayPlan && !plan.empty()) {
        accion = plan.front();
        plan.pop_front();
        if (plan.empty()) {
            hayPlan = false;
        }
    }

    PintaPlan(plan); // Visualizamos el plan restante en cada paso

    return accion;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  if (!hayPlan) {
        std::cout << "[INFO] Limite ecologico para este mapa: " << sensores.max_ecologico << std::endl;
        
        estadoN4 inicio = {sensores.BelPosF, sensores.BelPosC}; 
        estadoN4 destino_ficticio = {-1, -1}; // Ya no hace falta, Dijkstra se para al ver la 'U'
        
        planTuberias = dijkstra_nivel4(inicio, destino_ficticio, sensores.max_ecologico, sensores.energia);
        
        if (!planTuberias.empty()) {
            VisualizaRedTuberias(planTuberias); 
            hayPlan = true;
        } else {
            std::cout << "[ERROR] Imposible bajo estas condiciones físicas." << std::endl;
        }
    }
    return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  ActualizarMapa(sensores);
    static bool yendo_a_zapatillas_i = false;

    // 1. Sistema de Zapatillas (Opcional si se cruzan en su vista)
    if (!tiene_zapatillas) {
        if (sensores.superficie[0] == 'D') {
            tiene_zapatillas = true; yendo_a_zapatillas_i = false;
            estado_ing = CALCULANDO_I; plan_navegacion.clear();
            return IDLE;
        }
        bool veo_zap = false;
        for (int i = 1; i < 16; i++) if (sensores.superficie[i] == 'D') { veo_zap = true; break; }

        if (veo_zap && !yendo_a_zapatillas_i) {
            int zf = -1, zc = -1;
            for (int f = 0; f < mapaResultado.size(); f++)
                for (int c = 0; c < mapaResultado[0].size(); c++)
                    if (mapaResultado[f][c] == 'D') { zf = f; zc = c; }
            if (zf != -1) {
                yendo_a_zapatillas_i = true;
                plan_navegacion = a_estrella_navegacion(sensores.posF, sensores.posC, sensores.rumbo, zf, zc, false);
            }
        }
        if (yendo_a_zapatillas_i && !plan_navegacion.empty()) {
            Action sig = plan_navegacion.front(); plan_navegacion.pop_front();
            return sig;
        }
    }

    auto NavegadorInteligente = [&](int df, int dc) {
        return a_estrella_navegacion(sensores.posF, sensores.posC, sensores.rumbo, df, dc, tiene_zapatillas);
    };

    // 2. Máquina de Estados del Ingeniero
    switch (estado_ing) {
        case CALCULANDO_I:
            estado_ing = NAVEGANDO_I;
            plan_navegacion.clear();
            return IDLE;

        case NAVEGANDO_I:
            if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC) {
                estadoN4 inicio = {sensores.BelPosF, sensores.BelPosC}; 
                planTuberias = dijkstra_nivel4(inicio, {-1,-1}, sensores.max_ecologico, sensores.energia);
                
                if (!planTuberias.empty()) {
                    VisualizaRedTuberias(planTuberias); 
                    cout << "\n[DEBUG] Plan de Tuberias generado:" << endl;
                    PintaPlan(planTuberias); 
                    
                    estado_ing = TERRAFORMANDO_I; 
                    paso_idx_ing = 0; 
                } else {
                    return TURN_SR; 
                }
                return IDLE;
            }
            if (plan_navegacion.empty()) plan_navegacion = NavegadorInteligente(sensores.BelPosF, sensores.BelPosC);
            if (plan_navegacion.empty()) return TURN_SR;
            { Action sig = plan_navegacion.front(); plan_navegacion.pop_front(); return sig; }

            case TERRAFORMANDO_I: {
            if (paso_idx_ing >= planTuberias.size()) {
                cout << "\n[INFO] Terraformacion completada. Volviendo a la Belkanita..." << endl;
                estado_ing = VOLVIENDO_BASE_I; 
                plan_navegacion.clear();
                return IDLE;
            }

            auto it_curr = planTuberias.begin(); 
            std::advance(it_curr, paso_idx_ing);
            
            // 1. ¿Estamos de pie SOBRE la casilla que toca arreglar?
            if (sensores.posF == it_curr->fil && sensores.posC == it_curr->col) {
                
                // Ya estamos encima. Hacemos las acciones EN ESTA casilla.
                if (it_curr->op > 0) { 
                    it_curr->op--; 
                    cout << "[ACCION] Pisando (" << sensores.posF << "," << sensores.posC << "), hago RAISE." << endl;
                    return RAISE; 
                }
                else if (it_curr->op < 0) { 
                    it_curr->op++; 
                    cout << "[ACCION] Pisando (" << sensores.posF << "," << sensores.posC << "), hago DIG." << endl;
                    return DIG; 
                }
                else {
                    cout << "[ACCION] Casilla (" << sensores.posF << "," << sensores.posC << ") lista." << endl;
                    paso_idx_ing++; 
                    return IDLE; 
                }
            } 
            else {
                // 2. Aún no la estamos pisando. Calculamos el giro más rápido para avanzar.
                int f_dest = it_curr->fil;
                int c_dest = it_curr->col;
                int f_act = sensores.posF;
                int c_act = sensores.posC;
                
                // Averiguamos el rumbo exacto al que tenemos que mirar
                int rumbo_deseado = sensores.rumbo;
                if (f_dest < f_act && c_dest == c_act) rumbo_deseado = 0;      // Norte
                else if (f_dest < f_act && c_dest > c_act) rumbo_deseado = 1;  // Noreste
                else if (f_dest == f_act && c_dest > c_act) rumbo_deseado = 2; // Este
                else if (f_dest > f_act && c_dest > c_act) rumbo_deseado = 3;  // Sureste
                else if (f_dest > f_act && c_dest == c_act) rumbo_deseado = 4; // Sur
                else if (f_dest > f_act && c_dest < c_act) rumbo_deseado = 5;  // Suroeste
                else if (f_dest == f_act && c_dest < c_act) rumbo_deseado = 6; // Oeste
                else if (f_dest < f_act && c_dest < c_act) rumbo_deseado = 7;  // Noroeste

                if (sensores.rumbo == rumbo_deseado) {
                    // Si ya le miramos, damos el paso
                    return WALK;
                } else {
                    // Si no le miramos, calculamos el giro óptimo (Derecha o Izquierda)
                    int diff = (rumbo_deseado - sensores.rumbo + 8) % 8;
                    if (diff <= 4 && diff > 0) return TURN_SR;
                    else return TURN_SL;
                }
            }
        }

        case VOLVIENDO_BASE_I:
            if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC) {
                cout << "[INFO] Ingeniero: He vuelto. Llamando al Técnico..." << endl;
                estado_ing = LLAMANDO_I;
                return IDLE;
            }
            if (plan_navegacion.empty()) plan_navegacion = NavegadorInteligente(sensores.BelPosF, sensores.BelPosC);
            if (plan_navegacion.empty()) return TURN_SR;
            { Action sig = plan_navegacion.front(); plan_navegacion.pop_front(); return sig; }

        case LLAMANDO_I:
            // Ejecuta la acción COME. Esto activa 'venpaca' en el Técnico.
            estado_ing = ESPERANDO_TECNICO_I;
            return COME; 

        case ESPERANDO_TECNICO_I:
            if (sensores.enfrente) {
                // ¡Están cara a cara! Sincronización perfecta.
                cout << "[INFO] Ingeniero: Técnico posicionado. ¡A instalar a la vez!" << endl;
                estado_ing = INSTALANDO_I;
                
                // Empezamos a recorrer el plan de Dijkstra desde la casilla 1 (igual que el técnico)
                paso_idx_ing = 0; 
                return INSTALL; 
            } 
            
            // Si aún no ha llegado el técnico o no nos mira, nos quedamos apuntando a la casilla 1
            if (planTuberias.size() < 2) return IDLE;
            
            {
                auto it_next = planTuberias.begin(); 
                std::advance(it_next, 1); // La primera tubería de la obra
                
                ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
                ubicacion frente = Delante(actual);
                
                // Si ya estamos mirando fijamente a la casilla de la obra, nos quedamos quietos (IDLE)
                if (frente.f == it_next->fil && frente.c == it_next->col) {
                    return IDLE; 
                } else {
                    // Solo giramos si no estábamos apuntando al sitio correcto
                    return TURN_SR; 
                }
            }

        case INSTALANDO_I: {
            if (paso_idx_ing >= planTuberias.size()) {
                cout << "[INFO] Ingeniero: ¡Obra de tuberías finalizada en equipo!" << endl;
                return IDLE; 
            }

            auto it_curr = planTuberias.begin(); 
            std::advance(it_curr, paso_idx_ing);
            static bool instalada_aqui = false;

            // 1. ¿Estamos de pie sobre la casilla que nos toca?
            if (sensores.posF == it_curr->fil && sensores.posC == it_curr->col) {
                
                // 2. Girar hacia la siguiente tubería (donde está el técnico) antes de instalar
                if (paso_idx_ing + 1 < planTuberias.size()) {
                    auto it_next = planTuberias.begin();
                    std::advance(it_next, paso_idx_ing + 1);
                    
                    ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
                    ubicacion frente = Delante(actual);

                    if (frente.f != it_next->fil || frente.c != it_next->col) {
                        return TURN_SR; 
                    }
                }

                // 3. Ya estamos mirando al frente. ¿Nos mira el técnico?
                if (!instalada_aqui) {
                    if (sensores.enfrente) {
                        // ¡Nos mira! Disparamos a la vez.
                        instalada_aqui = true;
                        cout << "[ACCION] Ingeniero instalando en (" << sensores.posF << "," << sensores.posC << ")" << endl;
                        return INSTALL;
                    } else {
                        // Aún no nos mira. Nos quedamos quietos esperando su mirada.
                        return IDLE; 
                    }
                } else {
                    // Ya hemos soltado el tubo en el tick anterior. Avanzamos.
                    paso_idx_ing++;
                    instalada_aqui = false; 
                    return IDLE;
                }
            } 
            // 4. Si no estamos en la casilla, la perseguimos
            else {
                ubicacion destino = {it_curr->fil, it_curr->col, (Orientacion)0};
                ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
                
                if (Delante(actual).f == destino.f && Delante(actual).c == destino.c) {
                    
                    // ¡NUEVA LÓGICA DE ESPERA INTELIGENTE!
                    // Comprobamos si el técnico ('t') sigue ocupando la casilla de enfrente
                    if (sensores.agentes[2] == 't') {
                        return IDLE; // Esperamos amablemente a que el técnico avance
                    } else {
                        return WALK; // La casilla está libre, ¡damos el paso!
                    }
                    
                } else {
                    return TURN_SR; 
                }
            }
        }


        default: return IDLE;
          }
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
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
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
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

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
