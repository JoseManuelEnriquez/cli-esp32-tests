# 🖥️ cli-esp32-tests

Proyecto de pruebas para una interfaz de línea de comandos (CLI) sobre ESP32, desarrollado con **ESP-IDF** y estructurado con el patrón de diseño **Command**. Permite recibir comandos por UART y ejecutar acciones registradas de forma dinámica.

---

## 🏗️ Arquitectura

El proyecto sigue el patrón **Command** desacoplando la recepción de comandos, su registro y su ejecución en componentes independientes:

```
main/
└── main.cpp              → Punto de entrada (app_main), instancia la CLI

components/
├── CLI/                  → Orquesta la recepción y despacho de comandos
├── Command/              → Clase base abstracta para todos los comandos
├── Invoker/              → Registra y despacha comandos por nombre
├── UART/                 → Abstracción del driver UART de ESP-IDF
└── Test/                 → Comando de prueba (test-cli)
```

### ⚡ Flujo de ejecución

```
UART (recepción serie)
    └─> CLI::receive_command()
            └─> parser
                    └─> Invoker::process(nombre, parámetros)
                                └─> Command::execute(parámetros)
```

---

## 🧩 Componentes

### `UART` 📡
Abstracción sobre el driver `driver/uart.h` de ESP-IDF. Configura el puerto en 115200 baud, 8N1, sin control de flujo. Expone `receive()` y `send()`.

### `Command` 📦
Clase base abstracta. Todos los comandos heredan de ella e implementan `execute(std::string_view parameters)`. Reciben un puntero a `UART` para poder responder al usuario.

### `Invoker` 🗂️
Registro de comandos. Almacena hasta `MAX_COMMANDS` (10) entradas en un array estático de `CommandEntry` (nombre + puntero único al comando). El método `process()` busca por nombre y delega la ejecución.

### `CLI` 🔌
Punto de integración. Inicializa `UART` e `Invoker`, registra los comandos disponibles y expone `receive_command()` que es llamada en bucle desde `app_main`.

### `TestCliCommand` 🧪
Comando de prueba que responde `>>> CORRIENDO: TEST_CLI <<<` por UART. Registrado bajo el nombre `"test-cli"`.

---

## 📋 Requisitos

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/) v5.x o superior
- CMake ≥ 3.22
- Placa ESP32 (cualquier variante con UART0 disponible)

---

## ⚙️ Configuración del entorno

### 🐳 Opción A — Dev Container (recomendado)

El repositorio incluye un `.devcontainer` listo para usar con VS Code y Docker.

1. Instalar [VS Code](https://code.visualstudio.com/) con la extensión **Dev Containers**.
2. Abrir el repositorio y aceptar **"Reopen in Container"**.
3. El contenedor instala ESP-IDF automáticamente basándose en `espressif/idf:latest`.

### 💻 Opción B — Entorno local

```bash
# Instalar ESP-IDF siguiendo la guía oficial
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

# Activar el entorno
. $IDF_PATH/export.sh
```

---

## 🚀 Compilar y flashear

```bash
# Configurar el target
idf.py set-target esp32

# Compilar
idf.py build

# Flashear (sustituir /dev/ttyUSB0 por el puerto correspondiente)
idf.py -p /dev/ttyUSB0 flash

# Monitorear la salida serie
idf.py -p /dev/ttyUSB0 monitor
```

---

## 🖱️ Uso

Una vez el firmware esté corriendo, abrir el monitor serie y enviar el nombre del comando:

```
test-cli
```

Respuesta esperada:

```
>>> CORRIENDO: TEST_CLI <<<
```

---

## ➕ Añadir nuevos comandos

1. Crear una clase que herede de `Command` e implemente `execute()`.
2. Registrarla en el constructor de `CLI`:

```cpp
invoker.register_command("mi-comando", std::make_unique<MiComando>(&uart));
```

3. Añadir el nuevo componente (carpeta + `CMakeLists.txt`) bajo `components/`.

---

## 📊 Estado actual

- [x] ✅ Abstracción UART
- [x] ✅ Patrón Command + Invoker
- [x] ✅ Comando de prueba `test-cli`
- [x] 🔧 Parser de entrada (nombre + parámetros) — **pendiente**
- [ ] 🔧 Gestión de errores / comandos no reconocidos

---
