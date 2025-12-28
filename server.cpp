#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>
#include <sstream>
#include <cctype>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define closesocket close
#endif

struct Asset
{
    std::string name, symbol, category, type;
    double price, change;
    long long marketCap;
    int score;
};

class TrieNode
{
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    std::vector<Asset *> assets;
};

class Trie
{
    std::unique_ptr<TrieNode> root;
    void collect(TrieNode *node, std::vector<Asset *> &res, int max)
    {
        if (!node) return;
        if (res.size() >= (size_t)max)
            return;
        // add assets in this node
        for (auto *a : node->assets)
        {
            if (res.size() >= (size_t)max) break;
            res.push_back(a);
        }
        if (res.size() >= (size_t)max) return;
        for (auto &p : node->children)
        {
            collect(p.second.get(), res, max);
            if (res.size() >= (size_t)max) break;
        }
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}
    void insert(const std::string &word, Asset *asset)
    {
        TrieNode *cur = root.get();
        for (char ch : word)
        {
            unsigned char uch = static_cast<unsigned char>(ch);
            char c = static_cast<char>(std::tolower(uch));
            if (!cur->children[c])
                cur->children[c] = std::make_unique<TrieNode>();
            cur = cur->children[c].get();
        }
        cur->assets.push_back(asset);
    }
    std::vector<Asset *> search(const std::string &prefix, int max = 20)
    {
        std::vector<Asset *> res;
        TrieNode *cur = root.get();
        for (char ch : prefix)
        {
            unsigned char uch = static_cast<unsigned char>(ch);
            char c = static_cast<char>(std::tolower(uch));
            auto it = cur->children.find(c);
            if (it == cur->children.end())
                return res;
            cur = it->second.get();
        }
        collect(cur, res, max);
        return res;
    }
};

class InvestmentSystem
{
    std::vector<Asset> assets;
    Trie trie;
    std::vector<std::string> prefs = {"defi", "ai", "tech"};

    void initData()
    {
            assets = {
            // Cryptocurrencies (50 samples)
            {"Bitcoin", "BTC", "layer1", "crypto", 107772.53, -0.14, 2150000000000LL, 92},
            {"Ethereum", "ETH", "layer1", "crypto", 3874.64, -0.30, 468410000000LL, 88},
            {"Tether", "USDT", "stablecoin", "crypto", 1.00, 0.0, 182000000000LL, 85},
            {"Binance Coin", "BNB", "layer1", "crypto", 1068.77, -0.23, 148860000000LL, 84},
            {"XRP", "XRP", "crypto", "crypto", 2.41, -0.79, 144660000000LL, 80},
            {"Solana", "SOL", "layer1", "crypto", 184.46, -0.13, 100820000000LL, 86},
            {"USD Coin", "USDC", "stablecoin", "crypto", 1.00, 0.02, 76270000000LL, 83},
            {"TRON", "TRX", "crypto", "crypto", 0.3206, 0.04, 30350000000LL, 78},
            {"Dogecoin", "DOGE", "meme", "crypto", 0.1932, -0.43, 29270000000LL, 78},
            {"Cardano", "ADA", "layer1", "crypto", 0.64, -0.44, 22940000000LL, 77},
            {"Chainlink", "LINK", "defi", "crypto", 17.98, 0.49, 12200000000LL, 78},
            {"Hypersign", "HYPE", "crypto", "crypto", 35.67, -0.55, 12010000000LL, 77},
            {"Stellar", "XLM", "layer1", "crypto", 0.3148, -0.38, 10080000000LL, 75},
            {"Bitcoin Cash", "BCH", "layer1", "crypto", 465.86, -0.53, 9290000000LL, 75},
            {"Sui", "SUI", "layer1", "crypto", 2.48, -0.58, 9019000000LL, 75},
            {"Avalanche", "AVAX", "defi", "crypto", 19.70, -0.04, 8400000000LL, 76},
            {"LEO Token", "LEO", "crypto", "crypto", 8.97, -0.01, 8200000000LL, 75},
            {"Hedera", "HBAR", "crypto", "crypto", 0.1701, -0.20, 7200000000LL, 74},
            {"Litecoin", "LTC", "layer1", "crypto", 92.32, -0.64, 7050000000LL, 76},
            {"Shiba Inu", "SHIB", "meme", "crypto", 0.00001, 1.2, 6000000000LL, 72},
            {"Polygon", "MATIC", "layer1", "crypto", 0.87, 3.45, 8100000000LL, 80},
            {"Uniswap", "UNI", "defi", "crypto", 8.34, 6.78, 5100000000LL, 82},
            {"Aave", "AAVE", "defi", "crypto", 78.90, -0.56, 1100000000LL, 83},
            {"Render", "RNDR", "ai", "crypto", 7.89, 8.45, 2900000000LL, 85},
            {"Sandbox", "SAND", "gaming", "crypto", 0.45, 12.34, 1200000000LL, 76},
            {"Basic Attention Token", "BAT", "defi", "crypto", 0.28, -1.2, 1500000000LL, 75},
            {"Theta Network", "THETA", "media", "crypto", 1.89, -0.9, 3000000000LL, 74},
            {"VeChain", "VET", "layer1", "crypto", 0.11, -0.5, 6800000000LL, 73},
            {"Filecoin", "FIL", "storage", "crypto", 4.01, -0.3, 1700000000LL, 75},
            {"Algorand", "ALGO", "layer1", "crypto", 1.23, -0.7, 2500000000LL, 75},
            {"Cosmos", "ATOM", "layer1", "crypto", 12.34, -0.9, 3300000000LL, 74},
            {"Internet Computer", "ICP", "layer1", "crypto", 6.78, -1.1, 1500000000LL, 73},
            {"Kusama", "KSM", "layer1", "crypto", 180.00, -0.8, 1000000000LL, 74},
            {"Zcash", "ZEC", "privacy", "crypto", 60.00, -0.4, 900000000LL, 75},
            {"Dash", "DASH", "privacy", "crypto", 130.00, -0.6, 800000000LL, 73},
            {"PancakeSwap", "CAKE", "defi", "crypto", 4.50, -1.5, 700000000LL, 70},
            {"Gala", "GALA", "gaming", "crypto", 0.15, -0.7, 600000000LL, 72},
            {"Axie Infinity", "AXS", "gaming", "crypto", 10.50, -0.9, 500000000LL, 73},
            {"Compound", "COMP", "defi", "crypto", 90.00, -1.3, 250000000LL, 74},
            {"Maker", "MKR", "defi", "crypto", 1500.00, -0.7, 100000000LL, 75},
            {"Synthetix", "SNX", "defi", "crypto", 3.20, -1.0, 300000000LL, 70},
            {"Ocean Protocol", "OCEAN", "defi", "crypto", 0.60, -0.5, 250000000LL, 72},
            {"Storj", "STORJ", "storage", "crypto", 0.95, -0.9, 200000000LL, 70},
            {"Enjin Coin", "ENJ", "gaming", "crypto", 1.45, -0.9, 250000000LL, 71},
            {"Celo", "CELO", "layer1", "crypto", 2.50, -1.1, 150000000LL, 72},
            {"Ren", "REN", "defi", "crypto", 0.90, -1.4, 120000000LL, 70},

            // Stocks (50 samples)
            {"Apple Inc", "AAPL", "tech", "stock", 178.25, 1.85, 2800000000000LL, 90},
            {"Microsoft Corp", "MSFT", "tech", "stock", 412.30, 2.14, 3100000000000LL, 92},
            {"NVIDIA Corp", "NVDA", "tech", "stock", 495.80, 5.67, 1230000000000LL, 95},
            {"Amazon.com Inc", "AMZN", "tech", "stock", 145.60, 1.23, 1510000000000LL, 88},
            {"Tesla Inc", "TSLA", "auto", "stock", 242.50, 3.45, 770000000000LL, 82},
            {"Alphabet Inc", "GOOGL", "tech", "stock", 139.80, 1.89, 1750000000000LL, 89},
            {"Meta Platforms", "META", "tech", "stock", 485.20, 2.67, 1240000000000LL, 87},
            {"Netflix Inc", "NFLX", "media", "stock", 478.30, 3.12, 210000000000LL, 81},
            {"Adobe Inc", "ADBE", "tech", "stock", 567.90, 2.89, 260000000000LL, 88},
            {"Salesforce Inc", "CRM", "tech", "stock", 267.40, 2.45, 260000000000LL, 84},
            {"Visa Inc", "V", "financial", "stock", 220.50, 1.30, 487000000000LL, 87},
            {"Johnson & Johnson", "JNJ", "healthcare", "stock", 165.30, 0.75, 435000000000LL, 83},
            {"Walmart Inc", "WMT", "retail", "stock", 140.20, 0.58, 395000000000LL, 80},
            {"Procter & Gamble", "PG", "consumer", "stock", 150.55, 1.10, 345000000000LL, 82},
            {"Mastercard Inc", "MA", "financial", "stock", 370.10, 1.50, 390000000000LL, 85},
            {"Intel Corp", "INTC", "tech", "stock", 60.35, 0.80, 250000000000LL, 75},
            {"Cisco Systems", "CSCO", "tech", "stock", 50.20, 0.45, 210000000000LL, 76},
            {"PepsiCo Inc", "PEP", "consumer", "stock", 180.25, 0.90, 230000000000LL, 78},
            {"Coca-Cola Co", "KO", "consumer", "stock", 60.10, 0.60, 260000000000LL, 77},
            {"Netflix Inc", "NFLX", "media", "stock", 478.30, 3.12, 210000000000LL, 81},
            {"Berkshire Hathaway", "BRK.B", "financial", "stock", 540.15, 1.05, 650000000000LL, 88},
            {"Home Depot", "HD", "retail", "stock", 300.40, 2.00, 350000000000LL, 84},
            {"Pfizer Inc", "PFE", "healthcare", "stock", 40.10, 0.30, 230000000000LL, 79},
            {"Abbott Laboratories", "ABT", "healthcare", "stock", 115.20, 1.10, 200000000000LL, 79},
            {"Nike Inc", "NKE", "consumer", "stock", 115.00, 1.50, 220000000000LL, 80},
            {"Walt Disney Co", "DIS", "media", "stock", 135.60, 1.20, 330000000000LL, 81},
            {"Salesforce", "CRM", "tech", "stock", 267.40, 2.45, 260000000000LL, 84},
            {"ExxonMobil", "XOM", "energy", "stock", 70.10, 0.55, 400000000000LL, 80},
            {"Chevron Corp", "CVX", "energy", "stock", 105.20, 0.65, 300000000000LL, 79},
            {"IBM Corp", "IBM", "tech", "stock", 135.00, 0.40, 120000000000LL, 74},
            {"Boeing Co", "BA", "industrial", "stock", 145.00, 1.10, 140000000000LL, 77},
            {"Qualcomm Inc", "QCOM", "tech", "stock", 140.00, 1.25, 160000000000LL, 78},
            {"McDonald's Corp", "MCD", "consumer", "stock", 230.00, 0.90, 190000000000LL, 79},
            {"American Airlines", "AAL", "industrial", "stock", 18.50, 1.40, 13000000000LL, 65},
            {"General Electric", "GE", "industrial", "stock", 100.00, 0.75, 120000000000LL, 74},
            {"Ford Motor Co", "F", "auto", "stock", 15.00, 1.10, 44000000000LL, 70},
            {"Caterpillar Inc", "CAT", "industrial", "stock", 190.00, 1.20, 110000000000LL, 76},
            {"Visa Inc", "V", "financial", "stock", 220.50, 1.30, 487000000000LL, 87},
            {"Oracle Corp", "ORCL", "tech", "stock", 90.00, 0.85, 200000000000LL, 75},
            {"Adobe Inc", "ADBE", "tech", "stock", 567.90, 2.89, 260000000000LL, 88},
            {"Netflix Inc", "NFLX", "media", "stock", 478.30, 3.12, 210000000000LL, 81},
            {"Salesforce Inc", "CRM", "tech", "stock", 267.40, 2.45, 260000000000LL, 84},
            {"Intel Corp", "INTC", "tech", "stock", 60.35, 0.80, 250000000000LL, 75}
        };

        // insert into trie
        for (auto &a : assets)
        {
            trie.insert(a.name, &a);
            trie.insert(a.symbol, &a);
            trie.insert(a.category, &a);
        }
    }

    double calcScore(const Asset &a)
    {
        double s = a.score;
        if (std::find(prefs.begin(), prefs.end(), a.category) != prefs.end())
            s += 15;
        if (a.marketCap > 50000000000LL)
            s += 10;
        return std::min(100.0, s);
    }

    std::string toJSON(const Asset &a)
    {
        std::ostringstream oss;
        oss << "{\"name\":\"" << a.name << "\",\"symbol\":\"" << a.symbol
            << "\",\"price\":" << a.price << ",\"change\":" << a.change
            << ",\"cap\":" << (a.marketCap / 1e9) << ",\"cat\":\"" << a.category
            << "\",\"type\":\"" << a.type << "\",\"score\":" << (int)calcScore(a) << "}";
        return oss.str();
    }

public:
    InvestmentSystem() { initData(); }

    std::string searchJSON(const std::string &query, const std::string &type)
    {
        std::vector<Asset *> results;
        if (query.empty())
        {
            for (auto &a : assets)
            {
                if (type.empty() || a.type == type)
                    results.push_back(&a);
            }
        }
        else
        {
            auto found = trie.search(query, 200);
            for (auto *a : found)
            {
                if (type.empty() || a->type == type)
                    results.push_back(a);
            }
        }

        // dedupe by symbol
        std::vector<Asset *> unique;
        std::unordered_map<std::string, bool> seen;
        for (auto *a : results)
        {
            if (!seen[a->symbol])
            {
                unique.push_back(a);
                seen[a->symbol] = true;
            }
        }

        std::sort(unique.begin(), unique.end(), [this](Asset *a, Asset *b)
                  { return calcScore(*a) > calcScore(*b); });

        std::string json = "[";
        for (size_t i = 0; i < unique.size() && i < 50; i++)
        {
            if (i > 0)
                json += ",";
            json += toJSON(*unique[i]);
        }
        json += "]";
        return json;
    }

    std::string getRecommendationsJSON(const std::string &type)
    {
        std::vector<Asset *> candidates;
        for (auto &a : assets)
        {
            if (type.empty() || a.type == type)
                candidates.push_back(&a);
        }

        std::sort(candidates.begin(), candidates.end(), [this](Asset *a, Asset *b)
                  { return calcScore(*a) > calcScore(*b); });

        std::string json = "[";
        size_t limit = std::min(size_t(5), candidates.size());
        for (size_t i = 0; i < limit; i++)
        {
            if (i > 0)
                json += ",";
            json += toJSON(*candidates[i]);
        }
        json += "]";
        return json;
    }

    std::string getStatsJSON()
    {
        double totalCap = 0.0;
        double avgScore = 0.0;
        int cryptoCount = 0, stockCount = 0;
        for (auto &a : assets)
        {
            totalCap += static_cast<double>(a.marketCap);
            avgScore += calcScore(a);
            if (a.type == "crypto")
                cryptoCount++;
            else if (a.type == "stock")
                stockCount++;
        }
        if (!assets.empty())
            avgScore /= static_cast<double>(assets.size());

        std::ostringstream oss;
        oss << "{\"total\":" << assets.size()
            << ",\"cryptos\":" << cryptoCount
            << ",\"stocks\":" << stockCount
            << ",\"avgScore\":" << static_cast<int>(avgScore)
            << ",\"totalCap\":" << (totalCap / 1e12) << "}";
        return oss.str();
    }
};

class SimpleHTTPServer
{
    InvestmentSystem &system;
    int port;

    // simple URL-decode for + and %XX (very small, only used for q param here)
    static std::string urlDecode(const std::string &s)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] == '+') oss << ' ';
            else if (s[i] == '%' && i + 2 < s.size())
            {
                int hi = std::isdigit(s[i+1]) ? s[i+1]-'0' : std::tolower(s[i+1]) - 'a' + 10;
                int lo = std::isdigit(s[i+2]) ? s[i+2]-'0' : std::tolower(s[i+2]) - 'a' + 10;
                char ch = static_cast<char>((hi << 4) | lo);
                oss << ch;
                i += 2;
            }
            else oss << s[i];
        }
        return oss.str();
    }

    std::unordered_map<std::string, std::string> parseQuery(const std::string &queryStr)
    {
        std::unordered_map<std::string, std::string> params;
        size_t pos = 0;
        while (pos < queryStr.size())
        {
            size_t amp = queryStr.find('&', pos);
            std::string pair = queryStr.substr(pos, amp == std::string::npos ? std::string::npos : amp - pos);
            size_t eq = pair.find('=');
            if (eq != std::string::npos)
            {
                std::string k = pair.substr(0, eq);
                std::string v = pair.substr(eq + 1);
                params[k] = urlDecode(v);
            }
            pos = (amp == std::string::npos) ? queryStr.size() : amp + 1;
        }
        return params;
    }

    std::string handleRequest(const std::string &request)
    {
        std::string path = "/";
        std::string query;
        std::string type;

        size_t getPos = request.find("GET ");
        size_t httpPos = request.find(" HTTP");
        if (getPos != std::string::npos && httpPos != std::string::npos && httpPos > getPos)
        {
            std::string fullPath = request.substr(getPos + 4, httpPos - (getPos + 4));
            size_t qpos = fullPath.find('?');
            if (qpos != std::string::npos)
            {
                path = fullPath.substr(0, qpos);
                query = fullPath.substr(qpos + 1);
                auto params = parseQuery(query);
                if (params.count("q")) query = params["q"]; else query.clear();
                if (params.count("type")) type = params["type"];
            }
            else
            {
                path = fullPath;
            }
        }

        std::string response;
        if (path == "/api/search")
        {
            response = system.searchJSON(query, type);
        }
        else if (path == "/api/stats")
        {
            response = system.getStatsJSON();
        }
        else if (path == "/api/recommend")
        {
            response = system.getRecommendationsJSON(type);
        }
        else
        {
            response = "{\"error\":\"Not found\"}";
        }

        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: application/json\r\n"
            << "Access-Control-Allow-Origin: *\r\n"
            << "Content-Length: " << response.length() << "\r\n"
            << "\r\n"
            << response;
        return oss.str();
    }

public:
    SimpleHTTPServer(InvestmentSystem &sys, int p) : system(sys), port(p) {}

    void start()
    {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
        {
            std::cerr << "Error creating socket\n";
            return;
        }

        int opt = 1;
#ifdef _WIN32
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            std::cerr << "Error binding socket\n";
            closesocket(serverSocket);
            return;
        }

        listen(serverSocket, 5);
        std::cout << "🚀 Server running on http://localhost:" << port << "\n";
        std::cout << "API Endpoints:\n";
        std::cout << "  - GET /api/search?q=<query>&type=<crypto|stock>\n";
        std::cout << "  - GET /api/stats\n";
        std::cout << "  - GET /api/recommend?type=<crypto|stock>\n";
        std::cout << "Press Ctrl+C to stop...\n\n";

        while (true)
        {
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientLen);
            if (clientSocket < 0)
                continue;

            char buffer[8192];
            std::memset(buffer, 0, sizeof(buffer));
            ssize_t received = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
            if (received <= 0)
            {
                closesocket(clientSocket);
                continue;
            }
            std::string request(buffer, static_cast<size_t>(received));
            std::string response = handleRequest(request);
            send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
            closesocket(clientSocket);

            if (request.find("GET /api/") != std::string::npos)
            {
                size_t start = request.find("GET ") + 4;
                size_t end = request.find(" HTTP");
                if (start != std::string::npos && end != std::string::npos && end > start)
                    std::cout << "Request: " << request.substr(start, end - start) << "\n";
            }
        }

        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

int main()
{
    
    InvestmentSystem system;
    SimpleHTTPServer server(system, 8080);
    std::cout << "=== Investment Recommendation System ===\n";
    std::cout << "C++ Backend Server\n\n";
    server.start();
    return 0;
}
