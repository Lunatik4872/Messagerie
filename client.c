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

void consulter();

void reception(){
    return;
}

void envoie(int sock, char* ipServer, char* path, char* name){

    struct sockaddr_in dst_serv_addr;
    memset(&dst_serv_addr, 0, sizeof(dst_serv_addr));
    dst_serv_addr.sin_family = AF_INET; 
    dst_serv_addr.sin_port = htons(12345);
    inet_pton(AF_INET, ipServer, &(dst_serv_addr.sin_addr)); 

    if(connect(sock,(struct sockaddr*) &dst_serv_addr, sizeof(dst_serv_addr)) == -1){
        perror("Erreur connexion");
        close(sock);
        return;
    }

    if(send(sock, name, sizeof(name), 0) == -1){
        perror("Erreur lors de l'envoi du fichier.");
        exit(1);
    }

    char buffer[1024];
    FILE *fp;

    fp = fopen(path, "r");
    if(fp == NULL){
        perror("Erreur lors de l'ouverture du fichier.");
        exit(1);
    }

    while(fgets(buffer, 1024, fp) != NULL) {
        if(send(sock, buffer, sizeof(buffer), 0) == -1){
            perror("Erreur lors de l'envoi du fichier.");
            exit(1);
        }
        bzero(buffer, 1024);
    }

    fclose(fp);
    printf("\nMail envoyé avec succès\n");
}

void ecrire(int sock, char* ipServer, char* MyEmail){

    char email[100], objet[300], message[2000], filepath[150] = "envoie/";
    FILE *fptr;

    printf("À : ");
    fflush(stdout);
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    printf("Objet : ");
    fflush(stdout);
    fgets(objet, sizeof(objet), stdin);
    objet[strcspn(objet, "\n")] = 0;

    printf("\n");
    fflush(stdout);
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;

    strcat(email, ".txt");
    strcat(filepath, email);

    fptr = fopen(filepath, "w");
    if(fptr == NULL)
    {
        printf("Erreur!");
        exit(1);
    }

    fprintf(fptr,"De : %s\n", MyEmail);
    fprintf(fptr,"Objet : %s\n", objet);
    fprintf(fptr,"\n%s\n", message);
    fclose(fptr);

    envoie(sock,ipServer,filepath,email);
}

void lire(char* filename, char* name) {
    printf("\n--------------------------------------------------------------------------------\n\n");

    FILE *f;
    char line[1024];
    char filepath[1024];
    int firstLine = 1;

    sprintf(filepath, "%s/%s", name, filename);

    f = fopen(filepath,"r");
    if (f == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

     if(strcmp(name,"reception")) printf("À : %s\n",filename);

    while (fgets(line, sizeof(line), f)) {
        if(!strcmp(name,"reception")){
            printf("%s", line);
        }
        else{
            if (firstLine) {
                firstLine = 0;
                continue;
            }
            printf("%s", line);
        }
    }
    
    if(!strcmp(name,"reception")) printf("\n\n0- Retour | 1- Repondre");

    else printf("\n0- Retour | 1- Envoyer un nouveau mail");

    int choix;
    char line_choix[10];
    do {
        
        printf("\nVeuillez coisir 0 ou 1 : ");
        fflush(stdout);
        fgets(line_choix, sizeof(line_choix), stdin);
        choix = atoi(line_choix);
        if (choix < 0 || choix > 1) printf("\nSaisie invalide. Veuillez réessayer.\n");
    } while (choix < 0 || choix > 1);

    if(!choix) consulter(name);
    else printf("A implementer envoyer un nouveau mail");

    fclose(f);
}


void consulter(char* name){
    printf("\n--------------------------------------------------------------------------------\n\n");
    struct dirent *dir;
    char* files[100];

    DIR *d = opendir(name);
    if(!d){
        printf("ERREUR ouverture message !");
        return;
    }
    int i = 1;
    printf("0- Retour\n");
    while ((dir = readdir(d))) {
        if(strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")) {
            printf("%d- %s\n", i, dir->d_name);
            files[i-1] = strdup(dir->d_name);
            i++;
        }
    }

    if(i < 2) printf("\nVous n'avez aucun message !\n");

    closedir(d);

    int choix;
    char line[10];
    do {
        if(i < 2) printf("\nSelectionné 0 pour retourner au menu : ");
        else printf("\nVeuillez entrer un nombre entre 0 et %d : ", i-1);
        fflush(stdout);
        fgets(line, sizeof(line), stdin);
        choix = atoi(line);
        if (choix < 0 || choix >= i) {
            printf("\nSaisie invalide. Veuillez réessayer.\n");
        }
    } while (choix < 0 || choix >= i);

    if (choix == 0) {
        return;
    } else {
        lire(files[choix-1],name);
    }

    for (int j = 0; j < i-1; j++) {
        free(files[j]);
    }
}

void connexion(int sock, char* IPServDNS, char* MyIP, char* email){

    struct sockaddr_in dst_serv_addr;
    memset(&dst_serv_addr, 0, sizeof(dst_serv_addr));
    dst_serv_addr.sin_family = AF_INET; 
    dst_serv_addr.sin_port = htons(12344);
    inet_pton(AF_INET, IPServDNS, &(dst_serv_addr.sin_addr)); 

    if(connect(sock,(struct sockaddr*) &dst_serv_addr, sizeof(dst_serv_addr)) == -1){
        perror("Erreur connexion");
        close(sock);
        return;
    }

    if(send(sock,"1\0",2,0)==-1){
        perror("Erreur requete");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if(send(sock,email,100,0)==-1){
        perror("Erreur requete");
        close(sock);
        exit(EXIT_FAILURE);
    }
    if(send(sock,MyIP,40,0)==-1){
        perror("Erreur requete");
        close(sock);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]){

    if(argc < 4){
        printf("Usage : %s IPServeurSMTP IPServeurDNS adresseMail",argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        perror("Erreur ouverture du socket");
        exit(EXIT_FAILURE);
    }

    connexion(sock,argv[2],"127.0.0.1",argv[3]);

    int enCour = 1;
    // pthread_t threadReception;

    // if(pthread_create(&threadReception, NULL, reception, NULL)!=0){
    //     perror("Execution thread");
    // }

    while(enCour){
        printf("\n--------------------------------------------------------------------------------\n");
        printf("\t\t\t\t MESSAGERIE PROJET\n\n");

        printf("0- Envoyer un message\n");
        printf("1- Consulter vos messages\n");
        printf("2- Consulter vos messages envoyé\n");
        printf("3- Quitter\n");

        int choix;
        char line[10];
        do {
            printf("\nVeuillez entrer un nombre entre 0 et 3 : ");
            fflush(stdout);
            fgets(line, sizeof(line), stdin);
            choix = atoi(line);
            if (choix < 0 || choix > 3) {
                printf("\nSaisie invalide. Veuillez réessayer.\n");
            }
        } while (choix < 0 || choix > 3);

        if(choix == 0) ecrire(sock, argv[1], argv[3]);
        else if(choix == 1) consulter("reception\0");
        else if(choix == 2) consulter("envoie\0");
        else{
            printf("\nAUREVOIR\n");
            enCour = 0;
        }

    }
    close(sock);
    return 0;
}
