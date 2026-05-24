/**
 * @file cli.cpp
 * @brief Implementación de la clase CLI.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Contiene el constructor (inicialización de UART e Invoker, registro
 * de comandos) y el método receive_command() que actúa como parser y
 * despachador de entrada serie.
 */

#include "include/cli.hpp"

/**
 * @brief Constructor de CLI.
 *
 * Inicializa los miembros en el orden en que están declarados:
 *   1. uart   — instancia UART sobre UART_PORT_0 con los valores por
 *               defecto (115200 baud, 8N1, sin control de flujo).
 *   2. invoker — registro vacío de comandos.
 *
 * Tras la inicialización, registra los comandos disponibles.
 * Cada comando recibe un puntero a uart para poder enviar respuestas
 * al usuario por el mismo canal serie.
 */
CLI::CLI(): uart(UART(UART_PORT_0)), invoker(Invoker()) {
    invoker.register_command("test-cli", std::make_unique<TestCliCommand>(&uart));
}

/**
 * @brief Lee, parsea y despacha un comando recibido por UART.
 *
 * ### Flujo detallado
 *
 * 1. **Recepción** — llama a uart.receive() con un buffer de
 *    DEFAULT_BUFFER_SIZE bytes. La función bloquea hasta recibir
 *    una línea completa (terminada en '\\r' o '\\n').
 *
 * 2. **Validación** — si no se leyeron bytes o la vista resultante
 *    está vacía, retorna sin hacer nada.
 *
 * 3. **Parseo** — busca el primer espacio en la cadena recibida:
 *    - Sin espacio → todo el contenido es el nombre del comando;
 *      los parámetros quedan como cadena vacía.
 *    - Con espacio → la parte anterior es el nombre; la posterior
 *      son los parámetros (se eliminan espacios iniciales).
 *
 * 4. **Despacho** — llama a invoker.process(name, parameters), que
 *    busca el comando por nombre y ejecuta Command::execute().
 *
 * ### Ejemplo
 * Entrada serie:  "test-cli foo bar"
 *   - name       = "test-cli"
 *   - parameters = "foo bar"
 */
void CLI::receive_command() {
    uint32_t size_buf = DEFAULT_BUFFER_SIZE;
    uint8_t  buf[size_buf];

    // Bloquea hasta recibir una línea; retorna la cantidad de bytes leídos.
    uint32_t len = uart.receive(buf, size_buf);
    if (len > 0) {
        std::string_view cmd(reinterpret_cast<const char*>(buf), len);
        if (cmd.empty()) return;

        // --- Parseo nombre / parámetros ---
        size_t space_pos = cmd.find(' ');
        std::string_view name;
        std::string_view parameters;

        if (space_pos == std::string_view::npos) {
            // Comando sin parámetros: "test-cli"
            name       = cmd;
            parameters = "";
        } else {
            // Comando con parámetros: "test-cli foo bar"
            name       = cmd.substr(0, space_pos);
            parameters = cmd.substr(space_pos + 1);

            // Eliminar espacios iniciales en los parámetros
            size_t first_not_space = parameters.find_first_not_of(' ');
            if (first_not_space != std::string_view::npos) {
                parameters = parameters.substr(first_not_space);
            }
        }

        // Despachar al comando registrado bajo ese nombre
        invoker.process(name, parameters);
    }
}