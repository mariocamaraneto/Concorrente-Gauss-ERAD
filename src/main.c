#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <string.h>
#include <sched.h>


void TrocarLinhas(int n, int latual, int ltrocar, double **A, double *b)
{
    double aux;
    int i;
    for(i = latual; i <= n; i++) //Substitui as linhas
    {
        aux = A[latual][i]; //Guarda valor da linha original
        A[latual][i] = A[ltrocar][i]; //Troca pelo valor da outra linha
        A[ltrocar][i] = aux; //Restaura o valor da linha original posicionado em outra linha
    }
    aux = b[latual]; //Mesmo processo com o vetor b
    b[latual] = b[ltrocar];
    b[ltrocar] = aux;

}

void SubsRetroativa(int n, double **A, double *b, double *x)
{

    double soma[n];
    int i,j;
    for(i = n; i > 0; i--)
    {

    	soma[i] = 0;
		#pragma omp parallel shared(A,b,n, soma) private(j)
        {

			#pragma omp for schedule(guided,1)
			for(j = 1 + i; j <= n; j++){
				double aux = A[i][j] * x[j];
				#pragma omp atomic
					soma[i]+=aux;

			}
        }

        x[i] = (b[i] - soma[i]) / A[i][i];

    }

}

void Imprimir_Vetor(int n, double *b)
{
    int i;

    for(i = 1; i <= n; i++){
        printf("%2.4f\n", b[i]);
    }
    printf("\n");
}

void MEG(int n, double **A, double *b, double *x, int pivotamento) //Pivotamento = 0 -> MEG sem pivotamento; Pivotamento = 1 -> MEG com pivotamento
{
    int i, j, k;
    double pivo, m;


    for(k = 1; k < n; k++) // k = Coluna a ser zerada
    {
        pivo = A[k][k];

        if(!pivo) //Se o elemento pivô for 0, fará a troca da linha atual pela próxima e voltará a iteração
        {
            for(i = 1 + k; i <= n; i++){ //Acha um pivô diferente de 0
                if(A[i][k])
                {
                    pivo = A[i][k];
                    TrocarLinhas(n, k, i, A, b);
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
        else //Caso contrário, segue normalmente com a triangularização
        {
            for(i = 1 + k; i <= n; i++) // i = Linha da Matriz; percorrer as linhas a fim de mudar valores de seus elementos
            {
                j = 0 + k;
                m = A[i][j]/pivo; //Cálculo do multiplicador
                A[i][j] = 0; //Zerando o elemento da coluna, a fim de evitar que as aproximações deixem o elemento com valor diferente de 0
                for(j = 1 + k; j <= n; j++){ // j = Coluna da Matriz; percorrer as colunas para calcular seus valores
                	double aux;
                	aux = ((m*A[k][j]) * -1);
					//#pragma omp atomic
                    A[i][j] += aux; //Cálculo de todos os elementos da Linha
                }

                b[i] = b[i] - m * b[k]; //Cálculo dos elementos de b;
             }
        }
    }

    for(i = 1; i <= n; i++){
        if(!A[i][i])
        {
            printf("Matriz singular!\nNao é possível utilizar o método!\n");
            exit(0);
        }
    }

    //Substituição retroativa
    SubsRetroativa(n, A, b, x);

    //printf("Resolvendo o sistema Ax = b, obtemos o Vetor x:\n");
    Imprimir_Vetor(n, x);
}


int main()
{
	int control;
	int a;
    int n;
    int i;
    int j;
    char *valor;
    int num;
    char info[60000];
    double **A;
    double *B;
    double *x;
    FILE *arq;

    arq = fopen("arquivo.txt","r");
    if(arq == NULL){
        		puts("Não foi possivel abrir o arquivo");
    }
    else {
    num=0;
    i=1;
    control=0;
    while((fgets(info, sizeof(info), arq) != NULL)){


    	if(control==0){

    		n = atoi(info);
    		control++;
    		A = (double **) malloc (n+1, sizeof(double *));
    		for ( i = 0; i <= n; i++ ) {
    		      A[i] = (double*) malloc (n+1, sizeof(double));

    		      }
    		B = (double *) malloc (n+1, sizeof(double *));
    		x = (double *) malloc (n+1, sizeof(double *));
    		valor = (char *) malloc (n+1, sizeof(double *));
    		i=1;
    	}
    	else {
    	a=0;
    	if(control <= n){

    		j=1;
    		while(info[a]!= '\0'){

    			num=0;
    			while(info[a]!= ' ' && info[a]!='\0'){

    				valor[num]=info[a];
    				num++;
    				a++;
    			}
    			;
    			a++;
    			valor[num]= '\0';
    			num=0;
    			A[i][j]= (double) atof(valor);
    			//printf("%f",A[i][j]);
    			j++;
    		}
    		i++;
    		control++;

    	}

    	else{

    		a=0;
    		i=1;
    		num=0;
    		while(info[a]!= '\0'){

    		    	num=0;
    		    	while(info[a]!= ' ' && info[a]!='\0'){
    		    			valor[num]=info[a];
    		    			num++;
    		    			a++;
    		    	}
    		    	a++;
    		    	valor[num]= '\0';
    		    	num=0;
    		    	B[i]= (double)atof(valor);
    		    	i++;
    		    	}
    		    	control++;
    	}

    	}
    }
    }
    fclose(arq);

    MEG(n,A,B,x,0);

    return 0;
}
