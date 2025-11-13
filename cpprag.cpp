#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <curl/curl.h>  // 用來呼叫 API
#include "json.hpp"

using json = nlohmann::json;

// ============ 簡易 HTTP POST helper ============
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string http_post(const std::string& url, const std::string& json_body) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)json_body.size());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// ============ 餘弦相似度 ============
float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) {
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    return dot / (std::sqrt(normA) * std::sqrt(normB) + 1e-6f);
}

// ============ 範例：本地小型「知識庫」 ============
struct Doc {
    std::string text;
    std::vector<float> embedding;
};

// 模擬 embedding API
std::vector<float> get_embedding(const std::string& text) {
    std::cout << "取得文字嵌入: " << text << std::endl;
    const std::string url = "http://localhost:11434/api/embeddings";
    json req = {
        {"model", "nomic-embed-text"},  // Ollama 內建 embedding 模型
        {"prompt", text}
    };

    std::string response = http_post(url, req.dump());
    auto j = json::parse(response);
    if (j.find("embedding") == j.end()) {
        return {};
    }
    std::vector<float> emb = j["embedding"].get<std::vector<float>>();
    return emb;
}

// 模擬 LLM 回答
std::string call_llm(const std::string& prompt) {
    std::cout << "呼叫 LLM，提示語: " << prompt << std::endl;
    const std::string url = "http://localhost:11434/api/generate";
    json req = {
        {"model", "gpt-oss:20b"},  // 你可以換成你本地 Ollama 模型名稱
        {"prompt", prompt},
        {"stream", false}     // 不用逐行串流
    };

    std::string response = http_post(url, req.dump());

    if (response.empty()) {
        return "Error: No response from LLM.";
    }
    auto j = json::parse(response);
    if (j.find("response") == j.end()) {
        return "Error: Invalid response from LLM.";
    }
    return j["response"].get<std::string>();
}

int main() {
    // 1️⃣ 建立知識庫
    std::vector<Doc> knowledge = {
        {"Apple is a fruit.", get_embedding("Apple is a fruit.")},
        {"Bananas are yellow.", get_embedding("Bananas are yellow.")},
        {"The sky is blue.", get_embedding("The sky is blue.")}
    };

    // 2️⃣ 使用者提問
    std::string query = "What color is the sky?";
    std::vector<float> q_emb = get_embedding(query);

    // 3️⃣ 找出最相似的文件
    float best_score = -1.0f;
    std::string best_doc;
    for (const auto& doc : knowledge) {
        float score = cosine_similarity(q_emb, doc.embedding);
        if (score > best_score) {
            best_score = score;
            best_doc = doc.text;
        }
    }

    // 4️⃣ 建立 RAG prompt
    std::string prompt = "資料來源: " + best_doc + "\n問題: " + query;

    // 5️⃣ 呼叫 LLM
    std::string answer = call_llm(prompt);

    std::cout << "最相似的文件: " << best_doc << std::endl;
    std::cout << "生成的回答: " << answer << std::endl;

    return 0;
}
