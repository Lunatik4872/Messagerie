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

void transmettre(){
    return;
}

void recevoir(int sock){
    struct sockaddr_in sk_addr;
    memset(&sk_addr, 0, sizeof(sk_addr));
    sk_addr.sin_family = AF_INET;
    sk_addr.sin_port = htons(12345);
    sk_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock,(struct sockaddr*) &sk_addr, sizeof(sk_addr))==-1){
        perror("Erreur bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if(listen(sock,10)==-1){
        perror("Erreur d'ecoute");
        close(sock);
        exit(EXIT_FAILURE);
    }
       
    struct sockaddr_in new_addr;
    socklen_t addr_size;
    char buffer[1024];
    int new_sock;
    char name[100];

    addr_size = sizeof(new_addr);
    new_sock = accept(sock, (struct sockaddr*)&new_addr, &addr_size);

    recv(new_sock, name, 100, 0);

    strcat

    FILE *fp;
    fp = fopen(name, "w");
    if(fp == NULL){
        perror("Erreur lors de l'ouverture du fichier.");
        exit(1);
    }

    while(1){
        recv(new_sock, buffer, 1024, 0);
        if(strcmp(buffer, "fin") == 0){
            break;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, 1024);
    }

    fclose(fp);
}

char *demande(int sock, char* IPServDNS, char* email){
    struct sockaddr_in dst_serv_addr;
    memset(&dst_serv_addr, 0, sizeof(dst_serv_addr));
    dst_serv_addr.sin_family = AF_INET; 
    dst_serv_addr.sin_port = htons(12344);
    inet_pton(AF_INET, IPServDNS, &(dst_serv_addr.sin_addr)); 

    if(connect(sock,(struct sockaddr*) &dst_serv_addr, sizeof(dst_serv_addr)) == -1){
        perror("Erreur connexion");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if(send(sock,"0\0",2,0)==-1){
        perror("Erreur requete");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char* ip = malloc(40 * sizeof(char)); 

    if(send(sock,email,100,0)==-1){
        perror("Erreur requete");
        close(sock);
        exit(EXIT_FAILURE);
    }
    if(recv(sock,ip,40,0)==-1) {
        printf("Serveur hors ligne\n");
        free(ip); 
        return NULL;
    }
    
    return ip;
}

int main(){
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        perror("Erreur ouverture du socket");
        exit(EXIT_FAILURE);
    }

    recevoir(sock);
    close(sock);
    return 0;
}