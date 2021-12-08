/*
Universidade Federal de Itajubá - UNIFEI
Sistemas Operacionais - ECOS01 (2021.2)
Projeto: Aplicações através de algoritmos
Tema: Shortest Job First - SJF
*/

#include "stdio.h"

int main() {
    FILE *arquivo_entrada;
    char sinal;
    int quantidade = 0;

    // Abrir o arquivo de entrada
    arquivo_entrada = fopen("stdin.txt", "r");
    if(arquivo_entrada == NULL) {
        printf("Erro ao abrir o arquivo!");
        return 0;
    }

    // Contar a quantidade de linhas do arquivo que é igual a quantidade de processos
    while(fread(&sinal, sizeof(char), 1, arquivo_entrada)) {
        if(sinal == '\n') {
            quantidade++;
        }
    }
    quantidade++;

    // Transferir os dados do arquivo de entrada para os vetores
    rewind(arquivo_entrada);
    int tempo_chegada[quantidade], tempo_execucao[quantidade+1], tempo_espera[quantidade], tempo_vida[quantidade];
    for(int i = 0; i < quantidade; i++) {
        tempo_chegada[i] = tempo_execucao[i] = tempo_espera[i] = tempo_vida[i] = 0;
    }
    tempo_execucao[quantidade] = 0;
    for(int i = 0; i < quantidade; i++) {
        fscanf(arquivo_entrada, "%d %d\n", &tempo_chegada[i], &tempo_execucao[i]);
    }
    tempo_execucao[quantidade] = 999;
    fclose(arquivo_entrada);

    // Realizar o escalonamento SJF
    int tempo = 0, contador = 0, menor, alerta;
    float tempo_medio_espera = 0, tempo_medio_vida = 0;
    while(contador < quantidade) {
        menor = quantidade;
        alerta = 0;
        for(int i = 0; i < quantidade; i++) {
            if(tempo_chegada[i] <= tempo && tempo_execucao[i] < tempo_execucao[menor] && tempo_execucao[i] > 0) {
                menor = i;
                alerta = 1;
            }
        }
        if(alerta == 1) {
            tempo_espera[menor] = tempo - tempo_chegada[menor];
            tempo += tempo_execucao[menor];
            tempo_vida[menor] = tempo - tempo_chegada[menor];
            tempo_execucao[menor] = 0;
            contador++;
        } else {
            tempo++;
        }
    }

    // Calcular os tempos médios de espera e de vida
    for(int i = 0; i < quantidade; i++) {
        tempo_medio_espera += tempo_espera[i];
        tempo_medio_vida += tempo_vida[i];
    }
    tempo_medio_espera /= quantidade;
    tempo_medio_vida /= quantidade;

    // Preencher o arquivo de saída com o diagrama e os dados calculados
    FILE *arquivo_saida;
    arquivo_saida = fopen("stdout.txt", "w+");
    if(arquivo_saida == NULL) {
        printf("Erro ao criar o arquivo de saida!");
        return 0;
    }

    fprintf(arquivo_saida, "tempo");
    for(int i = 1; i <= quantidade; i++) {
        fprintf(arquivo_saida, " %2d", i);
    }
    fprintf(arquivo_saida, "\n");

    int diagrama[tempo][quantidade+2];
    for(int i = 0; i < tempo; i++) {
        for(int j = 0; j < quantidade+2; j++) {
            diagrama[i][j] = 0;
        }
    }

    for(int i = 0; i < tempo; i++) {
        diagrama[i][0] = i;
        diagrama[i][1] = i+1;
    }
    for(int j = 0; j < quantidade; j++) {
        for(int i = 0; i < tempo; i++) {
            if(i >= tempo_chegada[j] && i < (tempo_chegada[j] + tempo_espera[j])) {
                diagrama[i][j+2] = 1;
            }
            if(i >= (tempo_chegada[j] + tempo_espera[j]) && i < (tempo_chegada[j] + tempo_vida[j])) {
                diagrama[i][j+2] = 2;
            }
        }
    }

    for(int i = 0; i < tempo; i++) {
        fprintf(arquivo_saida, "%2d-%2d", diagrama[i][0], diagrama[i][1]);
        for(int j = 2; j < quantidade+2; j++) {
            switch(diagrama[i][j]) {
                case 0:
                    fprintf(arquivo_saida, "   ");
                    break;
                case 1:
                    fprintf(arquivo_saida, " --");
                    break;
                case 2:
                    fprintf(arquivo_saida, " ##");
                    break;
            }
        }
        fprintf(arquivo_saida, "\n");
    }

    fprintf(arquivo_saida, "\nTempo médio de espera: %.2f", tempo_medio_espera);
    fprintf(arquivo_saida, "\nTempo médio de resposta: %.2f", tempo_medio_vida);
    fprintf(arquivo_saida, "\nNúmero de troca de contexto: %d", quantidade);
    fclose(arquivo_saida);

    return 0;
}