/**
 * @file uart.hpp
 * @brief Abstracción del driver UART de ESP-IDF para ESP32.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Encapsula la configuración y el uso del driver `driver/uart.h` de
 * ESP-IDF en una clase C++ con gestión automática de recursos (RAII).
 * El constructor instala y configura el driver; el destructor lo
 * libera al salir del scope.
 *
 * @par Configuración fija (hardcoded en el constructor)
 *   - Baud rate : 115200
 *   - Bits de datos : 8
 *   - Paridad : ninguna
 *   - Bits de stop : 1
 *   - Control de flujo : deshabilitado
 *
 * @par Macros de conveniencia
 *   - UART_PORT_0 / UART_PORT_1 / UART_PORT_2 — alias de UART_NUM_x
 *   - DEFAULT_BUFFER_SIZE — tamaño del buffer de recepción (1024 bytes)
 */

#ifndef UART_HPP
#define UART_HPP

#include "driver/uart.h"
#include <string>
#include <cstring> // memcpy

/** @brief Alias para UART_NUM_0 (puerto serie de depuración por defecto). */
#define UART_PORT_0 UART_NUM_0

/** @brief Alias para UART_NUM_1. */
#define UART_PORT_1 UART_NUM_1

/** @brief Alias para UART_NUM_2. */
#define UART_PORT_2 UART_NUM_2

/** @brief Tamaño en bytes del buffer de recepción usado por receive(). */
#define DEFAULT_BUFFER_SIZE (1024)

/**
 * @class UART
 * @brief Wrapper RAII sobre el driver UART de ESP-IDF.
 *
 * Gestiona el ciclo de vida completo del driver: instalación en el
 * constructor y liberación en el destructor. Expone únicamente las
 * operaciones necesarias para la CLI: receive() y send().
 *
 * @warning No es seguro copiar ni mover objetos de esta clase, ya que
 *          el driver está ligado al puerto físico. Úsese siempre por
 *          referencia o puntero.
 */
class UART {
private:
    uart_port_t uart_port; ///< Puerto UART físico gestionado por esta instancia.

public:
    /**
     * @brief Instala y configura el driver UART.
     *
     * Llama internamente a uart_driver_install(), uart_param_config(),
     * uart_set_pin() y uart_set_mode(). Cualquier error en estas
     * llamadas provoca un abort() a través de ESP_ERROR_CHECK.
     *
     * @param port      Puerto UART a usar (UART_PORT_0, _1 o _2).
     * @param baud_rate Velocidad en baudios. Por defecto 115200.
     *                  @note El valor del parámetro es ignorado en la
     *                  implementación actual; la velocidad está fijada
     *                  a 115200 de forma hardcoded.
     * @param rx_pin    GPIO para RX. UART_PIN_NO_CHANGE conserva el
     *                  pin por defecto del puerto.
     * @param tx_pin    GPIO para TX. UART_PIN_NO_CHANGE conserva el
     *                  pin por defecto del puerto.
     */
    UART(uart_port_t port,
         int      baud_rate = 115200,
         uint32_t rx_pin    = UART_PIN_NO_CHANGE,
         uint32_t tx_pin    = UART_PIN_NO_CHANGE);

    /**
     * @brief Lee bytes del puerto UART hasta encontrar un terminador
     *        de línea ('\\r' o '\\n').
     *
     * Bloquea en un bucle interno leyendo chunks de 31 bytes con un
     * timeout de 100 ms por iteración, acumulando los datos en un
     * buffer interno hasta detectar el terminador. Una vez encontrado,
     * copia el contenido (sin el terminador) al buffer del llamador y
     * añade '\\0' al final.
     *
     * @param buffer  Buffer destino donde se escribirá la línea leída.
     *                Debe tener al menos @p length bytes de capacidad.
     * @param length  Capacidad máxima del buffer en bytes (incluyendo
     *                el '\\0' final). Se recomienda DEFAULT_BUFFER_SIZE.
     * @return Número de bytes copiados al buffer (sin contar '\\0').
     *
     * @warning Si la línea recibida supera @p length - 1 bytes, se
     *          trunca al tamaño del buffer para evitar desbordamientos.
     */
    uint32_t receive(uint8_t* buffer, uint32_t length);

    /**
     * @brief Envía datos por el puerto UART.
     *
     * Wrapper directo sobre uart_write_bytes(). No añade terminadores
     * de línea; el llamador es responsable de incluirlos si los necesita.
     *
     * @param data Puntero al buffer de datos a enviar.
     * @param len  Número de bytes a transmitir.
     */
    void send(const char* data, int len);

    /**
     * @brief Destructor: libera el driver UART del puerto asociado.
     *
     * Llama a uart_driver_delete() para liberar los recursos del
     * driver en ESP-IDF. Tras la destrucción el puerto queda disponible
     * para ser reinicializado.
     */
    ~UART();
};

#endif // UART_HPP