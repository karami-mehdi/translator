# Translator

A web-based translator built with **C++17** and **HTML, CSS, and JavaScript**. The application uses a local C++ HTTP server to connect the web interface to the MyMemory Translation API.

## Architecture

Layers:
- **Frontend:** Web Browser (HTML, CSS, and JavaScript)
- **Backend:** A C++17 HTTP server, powered by `cpp-httplib`, which handles requests, validates input, and communicates with the translation service.
- **Translation API:** `MyMemory` provides translations through HTTPS requests.

```text
Web Browser          C++ Backend              Translation
(Frontend)              Server                    API
   │                      │                        │
   │                      │                        │
   │    Input Text        │                        │
   │─────────────────────►│                        │
   │                      │   Translation Request  │
   │                      │───────────────────────►│
   │                      │                        │
   │                      │   Translated Text      │
   │                      │◄───────────────────────│
   │    JSON Response     │                        │
   │◄─────────────────────│                        │
   │                      │                        │
   │                      │                        │
index.html              main.cpp             MyMemory API
```

## Installation & Execution

### 1. Install Dependencies (macOS)

Using Homebrew:

```bash
$ brew install cmake curl nlohmann-json cpp-httplib
```

### 2. Configure the Project
```text
└── CMakeLists.txt    # CMake Build Configuration
```

```bash
$ cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix)"
```

### 3. Build

```bash
$ cmake --build build
```

### 4. Run

```bash
$ ./build/Translator
```

The server will start at:

```text
http://localhost:8080
```

Open this address in your browser to use the translator.

## Translation API

https://mymemory.translated.net

### API Endpoint

*POST `/translate`*

Example request:

```json
{
  "text": "Hello, how are you?",
  "source": "en",
  "target": "fa"
}
```

Example response:

```json
{
  "translation": "سلام، حال شما چطور است؟"
}
```