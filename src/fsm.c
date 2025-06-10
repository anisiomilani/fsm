#include "fsm.h"         // Inclui a interface do próprio módulo FSM
#include "driverlib.h"   // Para DEVICE_DELAY_US (simulação de tempo)
#include "device.h"      // Para Device_init, etc. (se alguma inicializacao ficar aqui)
#include <stdbool.h>     // Para o tipo bool

// --- Variáveis de Estado Globais do Módulo (Definição) ---
volatile ConverterState_t g_converterState = CONVERTER_STATE_INIT;
volatile unsigned int g_faultFlags = 0U;
volatile unsigned long g_operationCounter = 0UL;

// --- Implementações das Funções de Evento Simuladas (Internas) ---

static unsigned int contador_ciclos =0U; //exercio proposto para contagem

static unsigned int startup_counter = 0U;
static bool enable_cmd = false;
static bool oc_fault_sim = false;
static bool ov_fault_sim = false;
static bool ot_fault_sim = false;
static bool comm_err_sim = false;
static unsigned int recovery_counter = 0U;


// Protótipos de Funções Handler de Estado (Internas)
static void state_init_handler(void);
static void state_standby_handler(void);
static void state_operating_handler(void);
static void state_fault_overcurrent_handler(void);
static void state_fault_overvoltage_handler(void);
static void state_fault_temp_handler(void);
static void state_fault_comm_handler(void);
static void state_recovering_handler(void);

// Typedef Interno para Ponteiro de Função de Handler de Estado
typedef void (*StateHandler_t_Internal)(void);

// Array Global de Ponteiros para Funções (Jump Table)
static StateHandler_t_Internal g_stateHandlers[] =
{
    state_init_handler,
    state_standby_handler,
    state_operating_handler,
    state_fault_overcurrent_handler,
    state_fault_overvoltage_handler,
    state_fault_temp_handler,
    state_fault_comm_handler,
    state_recovering_handler
};
#define NUM_STATES (sizeof(g_stateHandlers) / sizeof(StateHandler_t_Internal))


// Funções de Evento Simuladas (Internas ao módulo)
static bool check_startup_complete(void);
static bool check_enable_command(void);
static bool check_overcurrent_fault(void);
static bool check_overvoltage_fault(void);
static bool check_overtemp_fault(void);
static bool check_comm_error(void);
static bool check_recovery_complete(void);


// --- Implementações das Funções Públicas do Módulo FSM ---

void FSM_Init(void)
{
    g_converterState = CONVERTER_STATE_INIT;
    g_faultFlags = 0U;
    g_operationCounter = 0UL;

    // Resetar contadores das funcoes de evento simuladas para um estado limpo
    startup_counter = 0U;
    enable_cmd = false;
    oc_fault_sim = false;
    ov_fault_sim = false;
    ot_fault_sim = false;
    comm_err_sim = false;
    recovery_counter = 0U;
}

void FSM_RunCycle(void)
{
    if (g_converterState < NUM_STATES && g_stateHandlers[g_converterState] != NULL)
    {
        g_stateHandlers[g_converterState]();
    }
    else
    {
        g_converterState = CONVERTER_STATE_FAULT_OVERCURRENT;
    }
}


// --- Implementações das Funções Handler de Estado (Internas) ---

void state_init_handler(void)
{
    if (check_startup_complete())
    {
        g_converterState = CONVERTER_STATE_STANDBY;
    }
}

void state_standby_handler(void)
{
    if (check_enable_command())
    {
        g_converterState = CONVERTER_STATE_OPERATING;
    }
}

void state_operating_handler(void)
{
    g_operationCounter++;


    if (check_overcurrent_fault())
    {
        g_faultFlags = g_faultFlags | FAULT_OVERCURRENT;
        g_converterState = CONVERTER_STATE_FAULT_OVERCURRENT;
    }
    else if (check_overvoltage_fault())
    {
        g_faultFlags = g_faultFlags | FAULT_OVERVOLTAGE;
        g_converterState = CONVERTER_STATE_FAULT_OVERVOLTAGE;
    }
    else if (check_overtemp_fault())        //foi adicionado essa condição
    {
        g_faultFlags = g_faultFlags | FAULT_TEMPERATURE;
        g_converterState = CONVERTER_STATE_FAULT_TEMP;
    }
    else if (check_comm_error())            //foi adicionado essa condição
     {
         g_faultFlags = g_faultFlags | FAULT_COMM_ERROR;
         g_converterState = CONVERTER_STATE_FAULT_COMM;
     }

}

void state_fault_overcurrent_handler(void)
{

    contador_ciclos++;

    if (contador_ciclos >= CICLO_TIME)  //tempo de espera conforme exercicio proposto
    {
        g_faultFlags = g_faultFlags & (~FAULT_OVERCURRENT);
        g_converterState = CONVERTER_STATE_RECOVERING;
        contador_ciclos = 0U;
    }

}

void state_fault_overvoltage_handler(void)
{
    contador_ciclos++;

    if (contador_ciclos >= CICLO_TIME) //tempo de espera conforme exercicio proposto
    {
        g_faultFlags = g_faultFlags & (~FAULT_OVERVOLTAGE);
        g_converterState = CONVERTER_STATE_RECOVERING;
        contador_ciclos = 0U;
    }
}

void state_fault_temp_handler(void)
{
    contador_ciclos++;

    if (contador_ciclos >= CICLO_TIME) //tempo de espera conforme exercicio proposto
    {
        g_faultFlags = g_faultFlags & (~FAULT_TEMPERATURE);
        g_converterState = CONVERTER_STATE_RECOVERING;
        contador_ciclos = 0U;
    }
}

void state_fault_comm_handler(void)
{
    contador_ciclos++;

    if (contador_ciclos >= CICLO_TIME) //tempo de espera conforme exercicio proposto
    {
        g_faultFlags = g_faultFlags & (~FAULT_COMM_ERROR);
        g_converterState = CONVERTER_STATE_RECOVERING;
        contador_ciclos = 0U;
    }
}

void state_recovering_handler(void)
{
    if (check_recovery_complete())
    {
        g_converterState = CONVERTER_STATE_STANDBY;
    }
}


static bool check_startup_complete(void) {
    // Usa TIME_STARTUP definido no fsm.h
    if (g_converterState == CONVERTER_STATE_INIT && startup_counter++ > TIME_STARTUP) {
        startup_counter = 0U; return true; }
    return false;
}

static bool check_enable_command(void) {
    if (g_converterState == CONVERTER_STATE_STANDBY && enable_cmd) {
        enable_cmd = false; return true; }
    return false;
}

static bool check_overcurrent_fault(void) {
    if (g_converterState == CONVERTER_STATE_OPERATING && oc_fault_sim) {
        oc_fault_sim = false; return true; }
    return false;
}

static bool check_overvoltage_fault(void) {
    if (g_converterState == CONVERTER_STATE_OPERATING && ov_fault_sim) {
        ov_fault_sim = false; return true; }
    return false;
}

static bool check_overtemp_fault(void) {
    if (g_converterState == CONVERTER_STATE_OPERATING && ot_fault_sim) {
        ot_fault_sim = false; return true; }
    return false;
}

static bool check_comm_error(void) {
    if (g_converterState == CONVERTER_STATE_OPERATING && comm_err_sim) {
        comm_err_sim = false; return true; }
    return false;
}

static bool check_recovery_complete(void) {
    // Usa TIME_RECOVERY definido no fsm.h
    if (g_converterState >= CONVERTER_STATE_FAULT_OVERCURRENT && recovery_counter++ > TIME_RECOVERY) {
        recovery_counter = 0U; return true; }
    return false;
}
