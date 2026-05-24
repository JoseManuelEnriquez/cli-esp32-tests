/**
 * @file uart.cpp
 * @brief Implementación de la clase UART.
 * @author José Manuel Enríquez Baena
 * @version 1.0
 *
 * Implementa la configuración del driver UART de ESP-IDF, la lectura
 * bloqueante por líneas y el envío de datos por el puerto serie.
 */

#include "include/uart.hpp"

/**
 * @brief Constructor: instala y configura el driver UART.
 *
 * ### Pasos de inicialización
 *
 * 1. **uart_driver_install()** — instala el driver con un buffer RX
 *    de DEFAULT_BUFFER_SIZE * 2 bytes. El buffer TX se deja en 0
 *    (modo síncrono), sin cola de eventos ni flags especiales.
 *
 * 2. **uart_param_config()** — aplica la configuración de línea:
 *    115200 baud, 8N1, sin control de flujo, reloj por defecto.
 *
 * 3. **uart_set_pin()** — asigna los GPIOs de RX y TX. Si se pasa
 *    UART_PIN_NO_CHANGE, el puerto conserva sus pines por defecto.
 *    RTS y CTS se dejan sin cambio (no se usa control de flujo HW).
 *
 * 4. **uart_set_mode()** — configura el modo estándar UART (sin RS485
 *    ni modo irDA).
 *
 * @note Todas las llamadas al driver se envuelven en ESP_ERROR_CHECK;
 *       cualquier fallo provoca un abort() con el error en el log.
 *
 * @param port      Puerto físico UART a inicializar.
 * @param baud_rate Parámetro recibido pero no utilizado actualmente;
 *                  la velocidad está fijada a 115200 en uart_config.
 * @param rx_pin    GPIO para la línea RX.
 * @param tx_pin    GPIO para la línea TX.
 */
UART::UART(uart_port_t port, int baud_rate, uint32_t rx_pin, uint32_t tx_pin):
    uart_port(port)
{
    // Instalar el driver con buffer RX doble para evitar pérdida de datos.
    ESP_ERROR_CHECK(uart_driver_install(uart_port,
        DEFAULT_BUFFER_SIZE * 2, // Tamaño buffer RX
        0,                       // Tamaño buffer TX (0 = síncrono)
        0,                       // Tamaño cola de eventos (0 = sin cola)
        NULL,                    // Handle de la cola (no se usa)
        0));                     // Flags de interrupción

    // Configuración del protocolo serie: 115200 baud, 8N1, sin flujo HW.
    uart_config_t uart_config = {};
    uart_config.baud_rate  = 115200;
    uart_config.data_bits  = UART_DATA_8_BITS;
    uart_config.parity     = UART_PARITY_DISABLE;
    uart_config.stop_bits  = UART_STOP_BITS_1;
    uart_config.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));

    // Asignar pines físicos; UART_PIN_NO_CHANGE conserva el pin por defecto.
    ESP_ERROR_CHECK(uart_set_pin(uart_port,
        rx_pin, tx_pin,
        UART_PIN_NO_CHANGE,  // RTS — no se usa
        UART_PIN_NO_CHANGE   // CTS — no se usa
    ));

    ESP_ERROR_CHECK(uart_set_mode(uart_port, UART_MODE_UART));
}

/**
 * @brief Lee del puerto UART hasta recibir un terminador de línea.
 *
 * ### Algoritmo
 *
 * - Se lee en chunks de 31 bytes (temp[32] con un byte reservado)
 *   con un timeout de 100 ms por llamada a uart_read_bytes().
 * - Cada chunk se acumula en internal_buffer (std::string).
 * - El bucle continúa hasta que se detecta '\\r' o '\\n' en el buffer.
 * - Una vez encontrado el terminador en la posición @c pos:
 *   - Se calcula cuántos bytes copiar: mínimo entre @c pos y
 *     @c length-1, evitando desbordamiento del buffer del llamador.
 *   - Se copia al buffer de salida y se añade '\\0'.
 *   - Se elimina la línea consumida de internal_buffer (por si quedó
 *     contenido posterior al terminador).
 *
 * @param buffer  Buffer destino (debe tener al menos @p length bytes).
 * @param length  Capacidad del buffer incluyendo el '\\0' final.
 * @return Número de bytes escritos en @p buffer (sin contar '\\0').
 */
uint32_t UART::receive(uint8_t* buffer, uint32_t length) {
    uint8_t temp[32];
    size_t pos = std::string::npos;
    std::string internal_buffer;

    // Leer en chunks hasta encontrar '\r' o '\n'.
    while (pos == std::string::npos) {
        int rx_bytes = uart_read_bytes(uart_port, temp, sizeof(temp) - 1, pdMS_TO_TICKS(100));
        if (rx_bytes > 0) {
            internal_buffer.append(reinterpret_cast<const char*>(temp), rx_bytes);
            pos = internal_buffer.find_first_of("\r\n");
        }
    }

    // Truncar si la línea excede la capacidad del buffer del llamador.
    uint32_t bytes_to_copy = (pos < length - 1) ? pos : length - 1;

    std::memcpy(buffer, internal_buffer.data(), bytes_to_copy);
    buffer[bytes_to_copy] = '\0'; // Garantizar terminación de cadena.

    // Limpiar la línea consumida del buffer interno.
    internal_buffer.erase(0, pos + 1);

    return bytes_to_copy;
}

/**
 * @brief Envía un bloque de datos por el puerto UART.
 *
 * Delegación directa a uart_write_bytes(). La función es no
 * bloqueante para el llamador; el driver gestiona la transmisión
 * en segundo plano.
 *
 * @param data Puntero al buffer de datos a enviar.
 * @param len  Número de bytes a transmitir.
 */
void UART::send(const char* data, int len) {
    uart_write_bytes(uart_port, data, len);
}

/**
 * @brief Destructor: desinstala el driver UART.
 *
 * Libera los recursos reservados por el driver en ESP-IDF para este
 * puerto. Tras la destrucción el puerto queda libre y puede ser
 * reinicializado con una nueva instancia de UART.
 */
UART::~UART() {
    uart_driver_delete(uart_port);
}