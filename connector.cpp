#include <stdio.h>      // Para printf() e fprintf()
#include <stdlib.h>     // Para atoi() e exit()
#include <string.h>     // Para memset()
#if defined(_WIN32) || defined(WIN32)
    #define WINDOWS_PLATFORM
    #include <winsock2.h>
    #include <windows.h>
    #include <WS2tcpip.h>
#elif defined(__unix__) || defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
    #define UNIX_PLATFORM
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#else
    #error "Plataforma não suportada!"
#endif

#define BUFFER_SIZE 1024

int connectToHSM(const char *command) {
    int sock = 0;
    int result;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.1");
    serverAddr.sin_port = htons(12345); 
    char buffer[BUFFER_SIZE]; // Buffer para a resposta do HSM
    int bytesSent, bytesRead;

    // Converte endereços IPv4 e IPv6 de texto para binário
    if(inet_pton(AF_INET, "192.168.1.1", &serverAddr.sin_addr) <= 0) {
        printf("Endereço IP inválido ou Endereço não suportado\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Erro ao criar socket");
        #ifdef WINDOWS_PLATFORM
            WSACleanup();
        #endif

        return 1;
    }

    #ifdef WINDOWS_PLATFORM
        WSADATA wsaData;

        // Inicializa a Winsock
        result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            printf("WSAStartup falhou: %d\n", result);
            return 1;
        }

        // Conecta-se ao HSM
        result = connect(sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            closesocket(sock);
            printf("Falha na conexão: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        printf("Conectado ao HSM.\n");

        // Envia comando ao HSM e recebe dados...
        result = send(sock, command, (int)strlen(command), 0);
        if (result == SOCKET_ERROR) {
            printf("Falha ao enviar comando: %ld\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            return 1;
        }

         // Aguarda e lê a resposta do HSM
        memset(buffer, 0, BUFFER_SIZE); // Limpa o buffer
        bytesRead = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesRead == SOCKET_ERROR) {
            printf("Erro ao ler resposta: %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            return 1;
        }

    // Imprime a resposta recebida do HSM
    printf("Resposta recebida do HSM: %s\n", buffer);

        // Fecha o socket
        closesocket(sock);
        WSACleanup();

    #elif defined(UNIX_PLATFORM)

        if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Conexão falhou");
            return 1;
        }

        printf("Conectado ao HSM.\n");

        // Envia e recebe dados...
        bytesSent = send(sock, command, strlen(command), 0);
        if (bytesSent < 0) {
            perror("Erro ao enviar comando");
            close(sock);
            exit(EXIT_FAILURE);
        }

         // Aguarda e lê a resposta do HSM
        memset(buffer, 0, BUFFER_SIZE); // Limpa o buffer
        bytesRead = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesRead < 0) {
            perror("Erro ao ler resposta");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Imprime a resposta recebida do HSM
        printf("Resposta recebida do HSM: %s\n", buffer);

        // Fecha o socket
        close(sock);

    #endif

    return 0;
}
