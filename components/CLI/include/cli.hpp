/**
 * @file cli.hpp
 * @brief Interfaz de línea de comandos (CLI) sobre UART para ESP32.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Define la clase CLI, punto de integración entre la recepción serie
 * y el despachador de comandos. Orquesta la inicialización del driver
 * UART, el registro de comandos disponibles y el bucle de escucha.
 *
 * Patrón de diseño utilizado: Command + Invoker.
 * La CLI no conoce la lógica de ningún comando; solo los registra y
 * delega su ejecución al Invoker.
 *
 * @par Dependencias
 *   - invoker.hpp  — registro y despacho de comandos
 *   - command.hpp  — clase base abstracta de comandos
 *   - uart.hpp     — abstracción del driver UART de ESP-IDF
 *   - test.hpp     — comando de prueba TestCliCommand
 */

#ifndef CLI_HPP
#define CLI_HPP

#include "invoker.hpp"
#include "command.hpp"
#include "uart.hpp"
#include "test.hpp"

/**
 * @class CLI
 * @brief Controlador principal de la interfaz de línea de comandos.
 *
 * Inicializa el puerto UART y el Invoker, registra los comandos
 * disponibles y expone receive_command() para ser llamado en bucle
 * desde app_main.
 *
 * @par Ejemplo de uso en app_main:
 * @code
 *   CLI cli;
 *   while (true) {
 *       cli.receive_command();
 *   }
 * @endcode
 */
class CLI {
public:
    /**
     * @brief Construye la CLI, inicializa UART e Invoker, y registra
     *        los comandos disponibles.
     *
     * Actualmente registra:
     *   - "test-cli" → TestCliCommand
     */
    CLI();

    /**
     * @brief Lee una línea del puerto UART, la parsea y despacha el
     *        comando correspondiente.
     *
     * El formato esperado es:
     * @code
     *   <nombre> [parámetros]
     * @endcode
     * Si no se reciben bytes o el buffer está vacío, la función
     * retorna sin hacer nada.
     */
    void receive_command();

private:
    UART    uart;     ///< Driver UART configurado en UART_PORT_0.
    Invoker invoker;  ///< Despachador que asocia nombres a comandos.
};

#endif // CLI_HPP