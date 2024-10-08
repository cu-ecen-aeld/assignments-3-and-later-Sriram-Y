#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#define PORT "9000"
#define BUFFER_SIZE 1024
#define DATA_FILE "/var/tmp/aesdsocketdata"

volatile sig_atomic_t is_running = 1;

void signal_handler(int signal);
void *get_address(struct sockaddr *address);
void configure_signal_handling();
int setup_server_socket();
void handle_client_connection(int client_fd);

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        // Fork a new process if "-d" is passed
        pid_t child_pid = fork();
        if (child_pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        if (child_pid > 0)
        {
            exit(EXIT_SUCCESS);
        }
    }

    configure_signal_handling();
    int server_fd = setup_server_socket();
    syslog(LOG_USER, "Server is waiting for connections...\n");

    while (is_running)
    {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0)
        {
            syslog(LOG_ERR, "Accepting connection failed.\n");
            continue;
        }

        char address_buffer[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family, get_address((struct sockaddr *)&client_addr), address_buffer, sizeof(address_buffer));
        syslog(LOG_USER, "Accepted connection from %s\n", address_buffer);

        handle_client_connection(client_fd);
        syslog(LOG_USER, "Closed connection from %s\n", address_buffer);
    }

    remove(DATA_FILE);
    close(server_fd);
    return 0;
}

void signal_handler(int signal)
{
    // Handle termination signals (SIGINT, SIGTERM)
    printf("Received signal %i. Stopping aesdsocket.\n", signal);
    syslog(LOG_USER, "Received signal %i. Stopping aesdsocket.\n", signal);
    is_running = 0;
}

void *get_address(struct sockaddr *address)
{
    if (address->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)address)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)address)->sin6_addr);
}

void configure_signal_handling()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler; // set the signal handler function
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int setup_server_socket()
{
    int server_fd;
    struct addrinfo hints, *server_info, *p;
    int opt = 1;
    int addr_info_status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Get a list of possible server addresses
    addr_info_status = getaddrinfo(NULL, PORT, &hints, &server_info);
    if (addr_info_status != 0)
    {
        syslog(LOG_ERR, "getaddrinfo: %s\n", gai_strerror(addr_info_status));
        exit(EXIT_FAILURE);
    }

    // Iterate through the results to find a valid address
    for (p = server_info; p != NULL; p = p->ai_next)
    {
        // Create the socket
        server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_fd < 0)
        {
            syslog(LOG_ERR, "Socket creation failed: %s\n", strerror(errno));
            continue;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            syslog(LOG_ERR, "Setting socket options failed: %s\n", strerror(errno));
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Bind the socket to the current address and port
        if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0)
        {
            syslog(LOG_ERR, "Binding socket failed: %s\n", strerror(errno));
            close(server_fd);
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (p == NULL)
    {
        syslog(LOG_ERR, "Failed to bind to any address\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        syslog(LOG_ERR, "Failed to listen on socket: %s\n", strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

void handle_client_connection(int client_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    FILE *file = fopen(DATA_FILE, "a+b");

    if (file == NULL)
    {
        perror("File open failed");
        return;
    }

    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, 1, bytes_received, file);
        fseek(file, 0, SEEK_SET);

        for (ssize_t i = 0; i < bytes_received; i++)
        {
            if (buffer[i] == '\n')
            {
                // Read the file and send its content
                char character;
                while ((character = fgetc(file)) != EOF)
                {
                    send(client_fd, &character, 1, 0);
                }
            }
        }
    }

    fclose(file);
    close(client_fd);
}
