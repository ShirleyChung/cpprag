# Retrieval-Augmented Generation (RAG) in C++ with Ollama

本專案示範如何使用 **純 C++11** 實作最小可行的\
**Retrieval-Augmented Generation (RAG)**：

-   使用 Ollama 本地模型產生 Embedding\
-   使用 Cosine Similarity 做向量檢索\
-   用 LLM（例如 llama3）生成回答\
-   透過 cURL 與 Ollama API 通訊\
-   使用 `json.hpp`（nlohmann/json）解析 API 回應

這個專案非常適合希望在 **C++ (98/03/11)** 環境下實作 RAG，但不想依賴
Python 的工程師。

## ✨ Features

-   使用 C++11 或 C++98（搭配 jsoncpp）\
-   本地向量資料庫（std::vector）\
-   Cosine Similarity 向量比對\
-   與 Ollama 互動（embedding + LLM generate）\
-   完整 JSON 錯誤處理\
-   不依賴任何 Python 生態系

## 📦 Requirements

### 基本環境

-   C++ 編譯器（g++, clang, MSVC 皆可）
-   Ollama（本機已執行）
-   libcurl（HTTP requests）
-   nlohmann/json（單 header 版）或 jsoncpp（若要使用 C++98）

### 安裝 libcurl

    brew install curl        # macOS
    sudo apt install libcurl4-openssl-dev   # Ubuntu / Debian

### 安裝單檔 JSON

將你下載的 `json.hpp` 放在與 `main.cpp` 同一資料夾即可。

## 📥 Pull Required Models

    ollama pull nomic-embed-text
    ollama pull llama3

## 🧱 Build

### C++11（推薦）

    g++ rag.cpp -std=c++11 -lcurl -o rag

### C++98（需 jsoncpp）

    g++ rag.cpp -std=c++98 -lcurl -ljsoncpp -o rag

## ▶️ Run

    ./rag

## 🧩 Project Structure

    .
    ├── rag.cpp
    └── json.hpp

## 🔧 How It Works

### 1. Embedding

    POST /api/embeddings

### 2. Retrieval（Cosine similarity）

### 3. Generate with LLM

    POST /api/generate

## 🛡 JSON Error Handling

遇到 Ollama 回傳錯誤：

    {"error": "model not found"}

需檢查：

-   JSON 可解析\
-   是否包含 `"error"`\
-   是否包含 `"embedding"`

避免：

    type_error.302: type must be array, but is null
