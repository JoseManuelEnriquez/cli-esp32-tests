/**
 * @file invoker.hpp
 * @brief Registro y despachador de comandos de la CLI.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Define la clase Invoker, responsable de almacenar los comandos
 * disponibles y de ejecutar el correcto según el nombre recibido.
 *
 * Forma parte del patrón de diseño **Command**: el Invoker desacopla
 * completamente a quien emite la solicitud (CLI) de quien la ejecuta
 * (subclases de Command).
 *
 * @par Límite de comandos
 * El número máximo de comandos registrables está fijado en tiempo de
 * compilación por la macro MAX_COMMANDS. Superar ese límite hace que
 * register_command() devuelva false sin modificar el estado interno.
 */

#ifndef INVOKER_HPP
#define INVOKER_HPP

#include "command.hpp"
#include <memory>
#include <array>
#include <string_view>

/** @brief Número máximo de comandos que el Invoker puede registrar. */
#define MAX_COMMANDS 10

/**
 * @class Invoker
 * @brief Registro estático de comandos y despachador por nombre.
 *
 * Mantiene un array de hasta MAX_COMMANDS entradas, cada una con el
 * nombre del comando y su implementación concreta. Cuando CLI llama
 * a process(), el Invoker busca la entrada cuyo nombre coincida y
 * delega la ejecución al objeto Command correspondiente.
 *
 * @note El Invoker toma ownership exclusivo de cada comando a través
 *       de std::unique_ptr, por lo que los comandos se destruyen
 *       automáticamente junto con el Invoker.
 */
class Invoker {
public:
    /**
     * @brief Construye un Invoker vacío (sin comandos registrados).
     */
    Invoker();

    /**
     * @brief Registra un nuevo comando asociado a un nombre.
     *
     * Añade la entrada al array interno y transfiere el ownership
     * del comando al Invoker mediante std::move.
     *
     * @param name    Nombre clave con el que se invocará el comando
     *                (p. ej. "test-cli"). Debe ser una cadena con
     *                tiempo de vida mayor que el Invoker, ya que se
     *                almacena como string_view (sin copia).
     * @param command Puntero único al objeto Command concreto.
     *                Tras la llamada el puntero original queda vacío.
     * @return true  si el comando se registró correctamente.
     * @return false si ya se alcanzó el límite MAX_COMMANDS.
     */
    bool register_command(std::string_view name, std::unique_ptr<Command> command);

    /**
     * @brief Busca el comando por nombre y ejecuta su lógica.
     *
     * Recorre el array de entradas registradas en orden de inserción.
     * En cuanto encuentra una coincidencia, llama a
     * Command::execute(parameters) y retorna. Si ningún nombre
     * coincide, la función no hace nada (sin efecto visible).
     *
     * @param name_cmd   Nombre del comando tal como fue parseado
     *                   por CLI::receive_command().
     * @param parameters Parámetros del comando, ya sin espacios
     *                   iniciales. Puede ser una vista vacía.
     */
    void process(std::string_view name_cmd, std::string_view parameters);

private:
    /**
     * @brief Entrada interna que asocia un nombre a su comando.
     *
     * @note name es un string_view (vista sin copia). El string
     *       literal al que apunta debe permanecer válido durante
     *       toda la vida del Invoker.
     */
    typedef struct {
        std::string_view         name;    ///< Nombre clave del comando (vista de solo lectura).
        std::unique_ptr<Command> command; ///< Implementación concreta del comando.
    } CommandEntry;

    std::array<CommandEntry, MAX_COMMANDS> commands; ///< Array estático de entradas registradas.
    uint32_t count; ///< Número de comandos actualmente registrados.
};

#endif // INVOKER_HPP