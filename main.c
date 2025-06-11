//#############################################################################
//
// ARQUIVO:    main.c
//
// TÍTULO:    Exemplo Principal de Uso da Máquina de Estados do Conversor
//
//! Este arquivo contém a função principal que inicializa o microcontrolador
//! e chama as funções do módulo da máquina de estados (FSM) para simular
//! o comportamento de um conversor de potência.
//
//#############################################################################
//
// $Data de Lançamento: $
// $Copyright:
// Copyright (C) 2013-2023 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribuição e uso em formatos de código-fonte e binários, com ou sem
// modificação, são permitidos desde que as seguintes condições sejam
// atendidas:
//
//   As redistribuições do código-fonte devem reter o aviso de direitos autorais
//   acima, esta lista de condições e a seguinte isenção de responsabilidade.
//
//   As redistribuições em formato binário devem reproduzir o aviso de direitos autorais
//   acima, esta lista de condições e a seguinte isenção de responsabilidade na
//   documentação e/ou outros materiais fornecidos com a distribuição.
//
//   Nem o nome da Texas Instruments Incorporated nem os nomes de
//   seus colaboradores podem ser usados para endossar ou promover produtos derivados
//   do software sem permissão prévia por escrito.
//
// ESTE SOFTWARE É FORNECIDO PELOS DETENTORES DOS DIREITOS AUTORAIS E COLABORADORES
// "AS IS" E QUAISQUER GARANTIAS EXPRESSAS OU IMPLÍCITAS, INCLUINDO, MAS NÃO
// SE LIMITANDO A, AS GARANTIAS IMPLÍCITAS DE COMERCIALIZAÇÃO E ADEQUAÇÃO PARA
// UM PROPÓSITO ESPECÍFICO SÃO REJEITADAS. EM NENHUM CASO O DETENTOR DOS DIREITOS AUTORAIS
// OU COLABORADORES SERÃO RESPONSÁVEIS POR QUAISQUER DANOS DIRETOS, INDIRETOS, INCIDENTAIS,
// ESPECIAIS, EXEMPLARES OU CONSEQUENCIAIS (INCLUINDO, MAS NÃO SE LIMITANDO A,
// AQUISIÇÃO DE BENS OU SERVIÇOS SUBSTITUTOS; PERDA DE USO, DADOS OU LUCROS;
// OU INTERRUPÇÃO DE NEGÓCIOS) SEJA QUAL FOR A CAUSA E SOB QUALQUER TEORIA DE
// RESPONSABILIDADE, SEJA EM CONTRATO, RESPONSABILIDADE ESTRITA OU ATO ILÍCITO
// (INCLUINDO NEGLIGÊNCIA OU OUTRO) DECORRENTE DE QUALQUER FORMA DO USO DESTE
// SOFTWARE, MESMO SE AVISADO DA POSSIBILIDADE DE TAL DANO.
// $
//#############################################################################

// Arquivos Incluídos
#include "driverlib.h" // Biblioteca de drivers da TI
#include "device.h"    // Configurações específicas do dispositivo (TMS320F28379D)
#include "fsm.h"       // Interface do módulo da Máquina de Estados (seu fsm.h)

//Exercicio
void initLEDGPIO(void);  //definindo uma função para iniciar os leds

/**
 * main.c
 *
 * Ponto de entrada principal da aplicação do firmware.
 * Responsável pela inicialização do sistema e execução do loop principal.
 */
void main(void)
{
    // 1. Inicialização de Periféricos Básicos do Microcontrolador
    // (Funções da DriverLib e device.h)
    Device_init();       // Inicializa o clock do dispositivo e o PIE (Peripheral Interrupt Expansion)
    Device_initGPIO();   // Inicializa as configurações básicas dos pinos GPIO


    initLEDGPIO(); //iniciando a função de iniciar os leds


    // 2. Inicialização do Módulo de Interrupções
    // (As interrupções serão tratadas em detalhes em outra aula,
    // mas a inicialização básica é necessária aqui para o ambiente de execução.)
    Interrupt_initModule();       // Inicializa o módulo PIE
    Interrupt_initVectorTable();  // Inicializa a tabela de vetores PIE

    // 3. Habilita Interrupções Globais
    // EINT: Habilita interrupções globais da CPU (Interrupt Enable Mask - INTM)
    // ERTM: Habilita interrupções de depuração em tempo real (Real-Time Debug Mode - DBGM)
    EINT;
    ERTM;

    // 4. Inicializa o Módulo da Máquina de Estados
    // Chama a função de inicialização do seu módulo FSM.
    FSM_Init();

    // 5. Loop Infinito Principal do Firmware
    // O microcontrolador executa continuamente as tarefas dentro deste loop.
    for(;;)
    {
        // Executa um ciclo da máquina de estados do conversor.
        // Toda a lógica de estados e transições está encapsulada no módulo FSM.
        FSM_RunCycle();

        // Adiciona um atraso para controlar a taxa de execução dos ciclos da FSM.
        // TIME_DELAY_US é definido em fsm.h e permite ajustar a "velocidade" da simulação.
        DEVICE_DELAY_US(TIME_DELAY_US);
    }
}

void initLEDGPIO(void)
{
    GPIO_setPadConfig(LED_GPIO_PIN_1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(LED_GPIO_PIN_1, GPIO_DIR_MODE_OUT);
    GPIO_writePin(LED_GPIO_PIN_1, 1); // LED inicia desligado (ativo baixo) azul

    GPIO_setPadConfig(LED_GPIO_PIN_2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(LED_GPIO_PIN_2, GPIO_DIR_MODE_OUT);
    GPIO_writePin(LED_GPIO_PIN_2, 1); // LED inicia desligado (ativo baixo) vermelho
}
