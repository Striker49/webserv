#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::atomic<int> requests_sent(0);
std::atomic<int> responses_received(0);

void send_request(const std::string& request, const std::string& host, int port) {
    std::cout << "Sending request to " << host << ":" << port << std::endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed to " << host << ":" << port << std::endl;
        close(sock);
        return;
    }

    ssize_t sent = send(sock, request.c_str(), request.size(), 0);
    if (sent < 0) {
        std::cerr << "Failed to send request: " << std::string(strerror(errno)) << std::endl;
        close(sock);
        return;
    }
    requests_sent++;
    std::cout << "Request sent successfully to " << host << ":" << port << ". Bytes sent: " << sent << std::endl;

    char buffer[4096];
    int bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received response: " << buffer << std::endl;
        responses_received++;
    }

    if (bytes_received < 0) {
        std::cerr << "Failed to receive response: " << std::string(strerror(errno)) << std::endl;
    }

    close(sock);
    std::cout << "Request processed successfully." << std::endl;
}

void send_valid_requests(const std::string& request) {
    std::cout << "Sending multiple requests..." << std::endl;
    std::vector<std::thread> threads;

    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([=]() { send_request(request, "127.0.0.1", 2323); });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    std::cout << "Finished sending multiple requests" << std::endl;
    std::cout << "Total requests sent: " << requests_sent.load() << std::endl;
    std::cout << "Total responses received: " << responses_received.load() << std::endl;
}

void send_post_request(const std::string& request) {
    std::cout << "Sending multiple requests..." << std::endl;
    std::vector<std::thread> threads;

    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([=]() { send_request(request, "127.0.0.1", 80); });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    std::cout << "Finished sending multiple requests" << std::endl;
    std::cout << "Total requests sent: " << requests_sent.load() << std::endl;
    std::cout << "Total responses received: " << responses_received.load() << std::endl;
}

void send_unvalid_requests(const std::string& request) {
    std::cout << "Sending multiple requests..." << std::endl;
    std::vector<std::thread> threads;

    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([=]() { send_request(request, "127.0.0.1", 2323); });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    std::cout << "Finished sending multiple requests" << std::endl;
    std::cout << "Total requests sent: " << requests_sent.load() << std::endl;
    std::cout << "Total responses received: " << responses_received.load() << std::endl;
}

void send_chunked_request() {
    std::vector<std::string> chunks_8080 = {
        "GET / HTT",
        "P/1.1\r\n",
        "Host: loc\r\n",
        "alhost:8080",
        "Cn",
        "nection: clse\r\n\r\n"
    };

    std::vector<std::string> chunks_2323 = {
        "GET / HTT",
        "P/1.1\r\n",
        "Host: loc\r\n",
        "alhost:2323",
        "Con",
        "nection: close\r\n\r\n"
    };

    auto send_chunks = [](const std::vector<std::string>& chunks, int port) {
        std::cout << "Starting chunked request to port " << port << "..." << std::endl;

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Connection failed to port " << port << std::endl;
            close(sock);
            return;
        }

        for (const auto& chunk : chunks) {
            ssize_t sent = send(sock, chunk.c_str(), chunk.size(), 0);
            if (sent < 0) {
                std::cerr << "Failed to send chunk: " << std::string(strerror(errno)) << std::endl;
                close(sock);
                return;
            }
            std::cout << "Sent chunk to port " << port << ": " << chunk << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        requests_sent++;

        char buffer[4096];
        int bytes_received;
        while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received response from port " << port << ": " << buffer << std::endl;
        }
        responses_received++;

        if (bytes_received < 0) {
            std::cerr << "Failed to receive response from port " << port << ": " << std::string(strerror(errno)) << std::endl;
        }

        close(sock);
        std::cout << "Chunked request to port " << port << " processed successfully." << std::endl;
    };

    std::thread thread_8080(send_chunks, chunks_8080, 8080);
    std::thread thread_2323(send_chunks, chunks_2323, 2323);

    if (thread_8080.joinable()) {
        thread_8080.join();
    }
    if (thread_2323.joinable()) {
        thread_2323.join();
    }
}

void send_malformed_request() {
    std::string request = "GET / HTTP/1.1\r\nHost: localhost:2323\r\nConnection: ccl";
    send_request(request, "127.0.0.1", 2323);
}

int main() {
    std::cout << "Starting stress tests..." << std::endl;
    std::string request = "GET / HTTP/1.1\r\nHost: localhost:2323\r\nConnection: close\r\n\r\n";
    std::string post_request = "POST /upload HTTP/1.1\r\nHost: localhost:80\r\nConnection: close\r\n\r\nThis is the body!!!\r\n\r\n";
    std::string cgi_post_request = "POST /upload.py HTTP/1.1\r\nHost: localhost:2323\r\nConnection: close\r\n\r\n-----------------------------221737088436447738901116600594\n\rContent-Disposition: form-data; name=\"file\"; filename=\"bravo.txt\"\r\nContent-Type: text/plain\nThis is the body!!!\r\n-----------------------------221737088436447738901116600594--\r\n\r\n";
    std::string w_request = "GET / HTTP/1.1\r\nHost: localhost:2323\r\nConnection: cose\r\n\r\n";
    std::string string_query = "GET /user?name=Sebastien&filename=text.txt HTTP/1.1\r\nHost: localhost:2323\r\nConnection: close\r\n\r\nThis is the body!!!\r\n\r\n";
    std::string cgi_request = "GET /hello_world.py HTTP/1.1\r\nHost: localhost:2323\r\nConnection: close\r\n\r\n";
    //send_valid_requests(cgi_request);
    //send_post_request(post_request);
    send_post_request(post_request);
    //send_unvalid_requests(w_request);
    // send_chunked_request();
    // send_large_payload();
    //send_malformed_request();
    // send_persistent_requests();
    std::cout << "Total requests sent: " << requests_sent.load() << std::endl;
    std::cout << "Total responses received: " << responses_received.load() << std::endl;
    std::cout << "Finished stress tests" << std::endl;
    return 0;
}

//g++ -std=c++11 -pthread -o client_tester client_tester.cpp