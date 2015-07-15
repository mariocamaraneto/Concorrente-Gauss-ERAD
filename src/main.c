#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <string.h>
#include <sched.h>

#define max_size 10001

double A[max_size][max_size];
double B[max_size];
double x[max_size];
int tamanho;


void TrocarLinhas( int latual, int ltrocar)
{
    double aux;
    int i;
    for(i = latual; i <= tamanho; ++i) //Substitui as linhas
    {
        aux = A[latual][i]; //Guarda valor da linha original
        A[latual][i] = A[ltrocar][i]; //Troca pelo valor da outra linha
        A[ltrocar][i] = aux; //Restaura o valor da linha original posicionado em outra linha
    }
    aux = B[latual]; //Mesmo processo com o vetor b
    B[latual] = B[ltrocar];
    B[ltrocar] = aux;

}

void SubsRetroativa()
{

    double soma[tamanho];
    int i,j;
    for(i = tamanho; i > 0; i--)
    {

    	soma[i] = 0;
		#pragma omp parallel shared(A,B,tamanho, soma) private(j)
        {

			#pragma omp for schedule(guided,1)
			for(j = 1 + i; j <= tamanho; ++j){
				double aux = A[i][j] * x[j];
				#pragma omp atomic
					soma[i]+=aux;

			}
        }

        x[i] = (B[i] - soma[i]) / A[i][i];
    }

}

void Imprimir_Vetor()
{
    int i;

    for(i = 1; i <= tamanho; ++i){
        printf("%2.4f\n", x[i]);
    }
    printf("\n");
}

void MEG() //Pivotamento = 0 -> MEG sem pivotamento; Pivotamento = 1 -> MEG com pivotamento
{
    int i, j, k;
    double pivo, m;


    for(k = 1; k < tamanho; ++k) // k = Coluna a ser zerada
    {
        pivo = A[k][k];

        if(pivo) //Se o elemento pivô for 0, fará a troca da linha atual pela próxima e voltará a iteração
        {
				for(i = 1 + k; i <= tamanho; i++) // i = Linha da Matriz; percorrer as linhas a fim de mudar valores de seus elementos
				{
					j = k;
					m = A[i][j]/pivo; //Cálculo do multiplicador
					A[i][j] = 0; //Zerando o elemento da coluna, a fim de evitar que as aproximações deixem o elemento com valor diferente de 0
					for(j = 1 + k; j <= tamanho; j++){ // j = Coluna da Matriz; percorrer as colunas para calcular seus valores
						double aux;
						aux = ((m*A[k][j]) * -1);
						//#pragma omp atomic
						A[i][j] += aux; //Cálculo de todos os elementos da Linha
					}

					B[i] = B[i] - m * B[k]; //Cálculo dos elementos de b;
				 }
        }
        else
        {
        	 for(i = 1 + k; i <= tamanho; ++i){ //Acha um pivô diferente de 0
				if(A[i][k])
				{
					pivo = A[i][k];
					TrocarLinhas( k, i);
					k--;
					break;
				}
			} //fecha for

			if(!pivo)//Caso todos os elementos abaixo o pivô sejam 0
			{
				printf("Nao foi possivel evitar divisao por 0 durante a utilizacao do metodo!\n");
				exit(0);
			}
        }
    }

    for(i = 1; i <= tamanho; i++){
        if(!A[i][i])
        {
            printf("Matriz singular!\nNao é possível utilizar o método!\n");
            exit(0);
        }
    }

    //Substituição retroativa
    SubsRetroativa();

    //printf("Resolvendo o sistema Ax = b, obtemos o Vetor x:\n");
    Imprimir_Vetor();
}


int main()
{
    int i, j;

    printf("LENDO ENTRADA\n");
    fflush(stdout);

    scanf("%d", &tamanho);


    for(i=1; i<=tamanho; ++i){
    	for(j=1; j<=tamanho; ++j){
    		scanf("%lf",&A[i][j]);
    	}
    }

    printf("LENDO matriz B\n");

    for(i=1; i<=tamanho; ++i){
    	scanf("%lf",&B[i]);
    }

    printf("PROCESSANDO\n");

    MEG(tamanho);

    return 0;
}
