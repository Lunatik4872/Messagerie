#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include <winsock.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>

#define MAX_ENTRIES 100

typedef struct {
    char email[50];
    char ip[50];
} Entry;

int sock;

void handle_sigint() {
    close(sock);
    printf("\nFermeture du serveur DNS !\n");
    exit(0);
}

void ecrireEmailIP(char* email, char* ip){
    Entry entries[MAX_ENTRIES];
    int num_entries = 0;

    FILE *file = fopen("adresse.csv", "r");
    if (file != NULL) {
        while (fscanf(file, "%[^,],%s\n", entries[num_entries].email, entries[num_entries].ip) != EOF) {
            if (strcmp(entries[num_entries].email, email) == 0) {
                strcpy(entries[num_entries].ip, ip);
            }
            num_entries++;
        }
        fclose(file);
    }

    if (num_entries == 0 || strcmp(entries[num_entries-1].email, email) != 0) {
        strcpy(entries[num_entries].email, email);
        strcpy(entries[num_entries].ip, ip);
        num_entries++;
    }

    file = fopen("adresse.csv", "w");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_entries; i++) {
        fprintf(file, "%s,%s\n", entries[i].email, entries[i].ip);
    }
    fclose(file);
}

char *trouverIP(char* email){
    FILE *file = fopen("adresse.csv", "r");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return NULL;
    }

    char emailS[50];
    char ip[50];

    while (fscanf(file, "%[^,],%s\n", emailS, ip) != EOF) {
        if(!strcmp(emailS,email)){
            fclose(file);
            char *result = malloc(strlen(ip) + 1);
            strcpy(result, ip);
            return result;
        };
    }

    fclose(file);
    return NULL;
}

int main(){
    signal(SIGINT, handle_sigint);

    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        perror("Erreur socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in sk_addr;
    memset(&sk_addr, 0, sizeof(sk_addr));
    sk_addr.sin_family = AF_INET;
    sk_addr.sin_port = htons(12344);
    sk_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock,(struct sockaddr*) &sk_addr, sizeof(sk_addr))==-1){
        perror("Erreur bind");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("Serveur DNS en cour...\n");

    if(listen(sock,10)==-1){
        perror("Erreur d'ecoute");
        close(sock);
        return EXIT_FAILURE;
    }

    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(sock, (struct sockaddr *) &client_addr, &client_len);

        if(client_sock == -1) {
            perror("Erreur accept");
            continue;
        }

        pid_t pid = fork();
        if(pid < 0) {
            perror("Erreur fork");
            return EXIT_FAILURE;
        }

        if(pid == 0) { 
            close(sock); 

            char choix[2];
            if(recv(client_sock,choix,2,0)==-1){
                printf("Client deconnecte\n");
                break;
            }

            char email[100];
            if(recv(client_sock,email,100,0)==-1){
                printf("Client deconnecte\n");
                break;
            }

            if(atoi(choix)){
                char ip[40];

                if(recv(client_sock,ip,40,0)==-1){
                    printf("Client deconnecte\n");
                    break;
                }

                ecrireEmailIP(email,ip);
            }

            else{
                if(send(client_sock,trouverIP(email),40,0)==-1){
                    perror("Erreur envoie");
                    close(client_sock);
                    return EXIT_FAILURE;
                }
            }

            close(client_sock);
            exit(0);
        } else {
            close(client_sock); 
        }
    }
    

    return 0;
}