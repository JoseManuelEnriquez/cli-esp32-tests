/**
 * @file command.cpp
 * @brief Implementación de la clase base Command.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Contiene únicamente el constructor, cuya responsabilidad es
 * almacenar el puntero UART que las subclases usarán para
 * comunicarse con el usuario.
 */

#include "include/command.hpp"

/**
 * @brief Constructor de Command.
 *
 * Inicializa el miembro protegido uart con el puntero recibido.
 * Las subclases deben llamar a este constructor desde su propia
 * lista de inicialización:
 * @code
 *   MiComando(UART* uart_ptr) : Command(uart_ptr) {}
 * @endcode
 *
 * @param uart_ptr Puntero al objeto UART. Se almacena tal cual;
 *                 el ownership permanece en quien lo creó (CLI).
 */
Command::Command(UART* uart_ptr): uart(uart_ptr) {}