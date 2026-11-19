# Translator

A text translation application developed in C++, providing a web interface for translating input text between English and Persian using an online translation service.

[Translator URL](https://karami-mehdi.github.io/translator)


## Architecture

Layers:
- **Frontend:** Web Application (HTML, CSS, and JavaScript)
- **Backend:** Written in C++, the backend manages translation requests, processes API responses, and communicates input data and translation results to the frontend.
  - **WebAssembly:** The C++ backend is compiled into WebAssembly by `Emscripten` and runs directly in the browser.
- **Translation API:** `MyMemory` provides translations through HTTPS requests.

### Diagram 1. Application Architecture
```text
Web Application        C++ Backend               Translation
  (Frontend)                 │                       API
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
   index.html              main.cpp              MyMemory API
```

### Diagram 2. Compilation and Execution Architecture
```text
 C++ Source Code
     main.cpp
         │
         ▼
Emscripten Toolchain
(CMake + Emscripten)
         │
         │         ┌───► translator.wasm ───┐
         ▼         │      (WebAssembly)     │
 Compiled Output ──┤                        ├──► Web Browser
                   │                        │  (Execution Env.)
                   └────► translator.js ────┘
                       (JavaScript Runtime)
```

## Translation API

[MyMemory Translation API](https://mymemory.translated.net)

### API Endpoint

The C++ backend sends an asynchronous **GET** request to:

```text
https://api.mymemory.translated.net/get
```

The request includes the URL-encoded input text (`q`) and language pair (`langpair`). The application supports:

- English to Persian (`en|fa`)
- Persian to English (`fa|en`)


### Example Request and Response

Example request:

```bash
curl --get "https://api.mymemory.translated.net/get" \
  --data-urlencode "q=Hello, how are you?" \
  --data-urlencode "langpair=en|fa"
```

Example response:

```json
{
  "responseStatus": 200,
  "responseData": {
    "translatedText": "سلام، حال شما چطور است؟"
  }
  ...
}
```


## Build & Execution

This is the **macOS** build guide. For Linux and Windows, adapt the steps to your system.

To build the project from source:

### 1. Install Dependencies

The project requires:
- **CMake**
- **Emscripten SDK**
- **nlohmann-json** (JSON for Modern C++)

Install dependencies using Homebrew:

```bash
$ brew install cmake emscripten nlohmann-json
```

> **Note:** The current `CMakeLists.txt` contains a Homebrew-specific include path for `nlohmann-json` (`/opt/homebrew/opt/nlohmann-json/include`). On Linux and Windows, update the path in `CMakeLists.txt` to the include directory provided by your package manager before configuring the project.

### 2. Configure

From the repository root, run:

```bash
$ emcmake cmake -S . -B build
```

### 3. Build

```bash
$ cmake --build build
```

The build generates `translator.js` and `translator.wasm` in the project root.

### 4. Run

The application runs in a web browser. Serve the project directory over HTTP, for example:

```bash
$ python3 -m http.server 8000
```

Then on your broswer, open:

```url
http://localhost:8000
```

The browser loads the frontend and the generated WebAssembly module. A separate C++ HTTP server is not required.

