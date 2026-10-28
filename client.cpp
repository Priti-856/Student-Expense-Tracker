// ===================== COMPUTER NETWORKS: TCP CLIENT =====================
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

int main(int argc, char* argv[]) {
    const char* ip = argc > 1 ? argv[1] : "127.0.0.1";
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    inet_pton(AF_INET, ip, &serv.sin_addr);

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) { perror("connect"); return 1; }
    cout << "Connected to expense server " << ip << ":8080\n"
         << "First: REGISTER username password  |  LOGIN username password\n"
         << "Commands (after login):\n"
         << "  INCOME amount category     (e.g. INCOME 5000 Pocket_Money)\n"
         << "  EXPENSE amount category    (e.g. EXPENSE 120 Food)\n"
         << "  LIST | CATEGORY name | SUMMARY\n"
         << "  UPDATE id new_amount | DELETE id | LOGOUT | QUIT\n";

    string line;
    while (cout << "> " && getline(cin, line)) {
        if (line.empty()) continue;
        send(sock, line.c_str(), line.size(), 0);
        if (line == "QUIT") break;
        char buf[4096];
        ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = 0;
        cout << buf;
    }
    close(sock);
}
