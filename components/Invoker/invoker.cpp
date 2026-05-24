/**
 * @file invoker.cpp
 * @brief Implementación de la clase Invoker.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Contiene la lógica de registro de comandos y el despachador que
 * busca y ejecuta el comando cuyo nombre coincida con la entrada
 * recibida por UART.
 */

#include "include/invoker.hpp"

/**
 * @brief Constructor del Invoker.
 *
 * Inicializa el contador de comandos en cero. El array de entradas
 * queda en estado por defecto (string_view vacío, unique_ptr nulo)
 * hasta que se llame a register_command().
 */
Invoker::Invoker(): count(0) {}

/**
 * @brief Registra un comando en el array interno.
 *
 * Comprueba primero que haya espacio disponible. Si lo hay, almacena
 * el nombre (como string_view, sin copia) y transfiere el ownership
 * del comando mediante std::move, dejando el unique_ptr del llamador
 * en estado nulo.
 *
 * @param name    Nombre clave del comando.
 * @param command Puntero único al comando concreto.
 * @return true  si se registró con éxito.
 * @return false si el array ya está lleno (count == MAX_COMMANDS).
 */
bool Invoker::register_command(std::string_view name, std::unique_ptr<Command> command) {
    if (count == MAX_COMMANDS) {
        return false; // Límite de comandos alcanzado; no se registra nada.
    }

    commands[count].name    = name;
    commands[count].command = std::move(command); // Transfiere ownership al array.
    count++;
    return true;
}

/**
 * @brief Despacha un comando según su nombre.
 *
 * Recorre linealmente las entradas registradas (de 0 a count - 1).
 * En la primera coincidencia de nombre invoca execute() y retorna
 * inmediatamente, garantizando que cada llamada ejecuta como máximo
 * un comando.
 *
 * Si no se encuentra ninguna coincidencia la función retorna sin
 * realizar ninguna acción ni emitir error. Gestionar comandos no
 * reconocidos queda pendiente como mejora futura.
 *
 * @param name_cmd   Nombre del comando a buscar.
 * @param parameters Parámetros que se pasan directamente a execute().
 */
void Invoker::process(std::string_view name_cmd, std::string_view parameters) {
    for (size_t i = 0; i < count; ++i) {
        if (commands[i].name == name_cmd) {
            commands[i].command->execute(parameters);
            return; // Comando encontrado y ejecutado; salir del bucle.
        }
    }
    // Si se llega aquí, ningún comando coincidió con name_cmd.
}