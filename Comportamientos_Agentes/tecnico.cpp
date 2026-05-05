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

// =========================================================
// Nivel 5
// =========================================================

std::list<Action> ComportamientoTecnico::a_estrella_navegacion(int orig_f, int orig_c, int orig_rumbo, int dest_f, int dest_c, bool zap) {
    std::priority_queue<nodoNav> Abiertos;
    std::set<estadoNav> Cerrados;

    nodoNav inicial;
    inicial.st = {orig_f, orig_c, orig_rumbo};
    inicial.g = 0;
    inicial.h = std::abs(orig_f - dest_f) + std::abs(orig_c - dest_c);
    Abiertos.push(inicial);

    // Verificamos la transitabilidad según la física base (sin alturas)
    auto esTransitable = [&](int f, int c) {
        if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
        char terr = mapaResultado[f][c];
        if (terr == 'M' || terr == 'P') return false;
        if (terr == 'B' && !zap) return false;
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

        // La casilla intermedia SIEMPRE debe ser transitable (tanto para WALK como para JUMP)
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

            // 2. LÓGICA DE JUMP
            ubicacion f2 = Delante(f1);
            if (esTransitable(f2.f, f2.c)) {
                // La diferencia de altura en JUMP se calcula solo entre Destino e Inicial
                int difJ = mapaCotas[f2.f][f2.c] - mapaCotas[actual.st.f][actual.st.c];
                
                // Aplicamos las normas estrictas que nos has enseñado
                if (std::abs(difJ) <= 1 || (zap && std::abs(difJ) <= 2)) {
                    nodoNav hijoJ = actual;
                    hijoJ.st.f = f2.f; hijoJ.st.c = f2.c;
                    
                    // ¡LA MAGIA ESTÁ AQUÍ! PENALIZACIÓN EXTREMA:
                    // Le ponemos coste 10 al JUMP. Así el A* NUNCA lo usará por gusto en llano.
                    // Solo "pagará" este alto coste cuando f1 sea un muro de altura insalvable
                    // que le impida generar un nodo WALK.
                    hijoJ.g += 10; 
                    
                    hijoJ.h = std::abs(f2.f - dest_f) + std::abs(f2.c - dest_c);
                    hijoJ.secuencia.push_back(JUMP);
                    Abiertos.push(hijoJ);
                }
            }
        }
    }
    return std::list<Action>();
}

std::list<Paso> ComportamientoTecnico::dijkstra_nivel4(const estadoN4& inicio, const estadoN4& destino, int max_impacto, int max_energia) {
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
  ActualizarMapa(sensores);
    
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    // Esta variable la usaremos para saber si ya hemos hecho INSTALL en el turno actual
    static bool instalada_aqui = false;

    // =========================================================
    // 1. VIAJE INICIAL HACIA LA BASE (Aparcar a 3 pasos)
    // =========================================================
    if (estado_tec == ACERCANDOSE_ING){
        if (!hay_plan) {
            Estado origen = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
            Estado destino = {sensores.BelPosF, sensores.BelPosC, 0, false};
            plan_actual = AEstrella(origen, destino);
        
            if (!plan_actual.empty()) hay_plan = true;
            else return IDLE; 
        }

        if (hay_plan && !plan_actual.empty()) {
            if (plan_actual.size() <= 3) {
                hay_plan = false;
                estado_tec = ESPERANDO_LLAMADA;
                plan_actual.clear();
                return IDLE;
            }
            Action sig_accion = plan_actual.front();
            plan_actual.pop_front();
            last_action = sig_accion;
            return sig_accion;
        }
    }

    // =========================================================
    // 2. ESPERA ACTIVA A LA SEÑAL DEL INGENIERO
    // =========================================================
    if (estado_tec == ESPERANDO_LLAMADA) {
        if (sensores.venpaca) {
            // ¡Luz verde! En vez de buscar la mirada del ingeniero, 
            // nos ponemos a calcular la ruta directamente.
            estado_tec = CALCULANDO_T;
            hay_plan = false; 
            plan_actual.clear();
        }
        return IDLE; 
    }

    // =========================================================
    // 3. CALCULAR PLAN DE TUBERÍAS
    // =========================================================
    if (estado_tec == CALCULANDO_T) {
        estadoN4 inicio = {sensores.BelPosF, sensores.BelPosC}; 
        planTuberias = dijkstra_nivel4(inicio, {-1,-1}, sensores.max_ecologico, sensores.energia);
        
        if (!planTuberias.empty()) {
            // Reutilizamos este estado para viajar a la primera casilla del tubo
            estado_tec = ACUDIENDO_LLAMADA; 
            paso_idx_t = 1; // Índice 1: La casilla JUSTO DELANTE de la Belkanita
            hay_plan = false;
            plan_actual.clear();
        }
        return IDLE;
    }

   // =========================================================
    // 4. VIAJE DIRECTO A LA PRIMERA CASILLA DE TUBERÍA
    // =========================================================
    if (estado_tec == ACUDIENDO_LLAMADA) {
        auto it = planTuberias.begin();
        std::advance(it, paso_idx_t); // Extraemos la coordenada de planTuberias[1]
        
        // 1. Condición de éxito: Ya estamos de pie sobre la primera zona de la obra
        if (sensores.posF == it->fil && sensores.posC == it->col) {
            
            // ¡CRÍTICO PARA LA COORDINACIÓN! Buscamos la mirada del ingeniero
            if (sensores.enfrente) {
                // ¡Nos estamos mirando!
                estado_tec = INSTALANDO_T;
                instalada_aqui = false;
                // Disparamos el INSTALL instantáneo para cuadrar el primer tick
                return INSTALL; 
            } else {
                // Si aún no nos miramos, giramos como una peonza hasta cruzar miradas
                return TURN_SR;
            }
        }

        // 2. Trazar el A* hacia esa primera casilla
        if (!hay_plan) {
            Estado origen = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
            Estado destino = {it->fil, it->col, 0, false}; 

            // =========================================================
            // ¡EL TRUCO MAGISTRAL! 
            // Guardamos lo que había y ponemos un muro en la Belkanita
            // para obligar al A* a esquivar al Ingeniero.
            // =========================================================
            unsigned char celda_original = mapaResultado[sensores.BelPosF][sensores.BelPosC];
            mapaResultado[sensores.BelPosF][sensores.BelPosC] = 'M'; // 'M' de Muro

            // Calculamos el plan esquivando la base
            plan_actual = AEstrella(origen, destino);
        
            // Restauramos la casilla inmediatamente para no romper el mapa
            mapaResultado[sensores.BelPosF][sensores.BelPosC] = celda_original;

            if (!plan_actual.empty()) hay_plan = true;
            else return TURN_SR; // Retorno de seguridad anti-atascos
        }

        // 3. Ejecutar el A*
        if (hay_plan && !plan_actual.empty()) {
            Action sig_accion = plan_actual.front();
            plan_actual.pop_front();
            
            // Si el plan se acaba, reseteamos para comprobar si hemos llegado en el próximo tick
            if (plan_actual.empty()) hay_plan = false; 
            
            last_action = sig_accion;
            return sig_accion;
        }
    }
    // =========================================================
    // 5. INSTALACIÓN DE TUBERÍAS SEGUIDA (CON BRÚJULA INTELIGENTE)
    // =========================================================
    if (estado_tec == INSTALANDO_T) {
        if (paso_idx_t >= planTuberias.size()) return IDLE;

        auto it_curr = planTuberias.begin(); 
        std::advance(it_curr, paso_idx_t);

        // 1. Si estamos parados EXACTAMENTE en la casilla correcta
        if (sensores.posF == it_curr->fil && sensores.posC == it_curr->col) {
            
            int f_ing, c_ing;
            if (paso_idx_t == 0) { 
                f_ing = sensores.BelPosF; 
                c_ing = sensores.BelPosC; 
            } else {
                auto it_prev = planTuberias.begin();
                std::advance(it_prev, paso_idx_t - 1);
                f_ing = it_prev->fil; 
                c_ing = it_prev->col;
            }
            
            ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
            ubicacion frente = Delante(actual);

            // Si no miramos a la casilla del ingeniero, calculamos el giro ÓPTIMO
            if (frente.f != f_ing || frente.c != c_ing) {
                int rumbo_deseado = sensores.rumbo;
                if (f_ing < actual.f && c_ing == actual.c) rumbo_deseado = 0;      // Norte
                else if (f_ing < actual.f && c_ing > actual.c) rumbo_deseado = 1;  // Noreste
                else if (f_ing == actual.f && c_ing > actual.c) rumbo_deseado = 2; // Este
                else if (f_ing > actual.f && c_ing > actual.c) rumbo_deseado = 3;  // Sureste
                else if (f_ing > actual.f && c_ing == actual.c) rumbo_deseado = 4; // Sur
                else if (f_ing > actual.f && c_ing < actual.c) rumbo_deseado = 5;  // Suroeste
                else if (f_ing == actual.f && c_ing < actual.c) rumbo_deseado = 6; // Oeste
                else if (f_ing < actual.f && c_ing < actual.c) rumbo_deseado = 7;  // Noroeste

                // Calculamos si es mejor girar a la derecha o a la izquierda
                int diff = (rumbo_deseado - sensores.rumbo + 8) % 8;
                if (diff <= 4 && diff > 0) return TURN_SR;
                else return TURN_SL;
            }

            // Si ya estamos orientados hacia él, esperamos a que sus ojos se crucen con los nuestros
            if (sensores.enfrente) {
                if (!instalada_aqui) {
                    instalada_aqui = true;
                    return INSTALL; 
                } else {
                    paso_idx_t++; 
                    instalada_aqui = false; 
                    return IDLE;
                }
            } else {
                return IDLE; 
            }
        } 
        // 2. Si no la estamos pisando, avanzamos hacia ella de la forma más rápida
        else {
            ubicacion destino = {it_curr->fil, it_curr->col, (Orientacion)0};
            ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
            
            if (Delante(actual).f == destino.f && Delante(actual).c == destino.c) {
                return WALK;
            } else {
                int rumbo_deseado = sensores.rumbo;
                if (destino.f < actual.f && destino.c == actual.c) rumbo_deseado = 0;
                else if (destino.f < actual.f && destino.c > actual.c) rumbo_deseado = 1;
                else if (destino.f == actual.f && destino.c > actual.c) rumbo_deseado = 2;
                else if (destino.f > actual.f && destino.c > actual.c) rumbo_deseado = 3;
                else if (destino.f > actual.f && destino.c == actual.c) rumbo_deseado = 4;
                else if (destino.f > actual.f && destino.c < actual.c) rumbo_deseado = 5;
                else if (destino.f == actual.f && destino.c < actual.c) rumbo_deseado = 6;
                else if (destino.f < actual.f && destino.c < actual.c) rumbo_deseado = 7;

                // Calculamos el giro óptimo para caminar
                int diff = (rumbo_deseado - sensores.rumbo + 8) % 8;
                if (diff <= 4 && diff > 0) return TURN_SR;
                else return TURN_SL;
            }
        }
    }

    return IDLE;
  }


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


