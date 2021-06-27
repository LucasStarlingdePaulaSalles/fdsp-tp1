/* Você deve implementar uma versão usando paralelismo de dados (também
 * chamado decomposição/partição de domínio (domain partition/decomposition)
 * Para simplificar, você pode considerar que seu programa criará oito
 * threads para processar todos os números. Todas as threads devem executar
 * o mesmo código, já que estarão executando as mesmas operações para
 * partes diferentes do conjunto de dados.
 * 
 * O programa deve aceitar o mesmo parâmetro de linha de comando da versão
 * sequencial - e nenhum outro. 
 * A saída deve sequir exatamente o mesmo formato da versão sequencial.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "timediff.h"   // calcula tempo decorrido
#include "numchecks.h"  // conta números com mais condições válidas
#include "conditions.h" // verifica cada condição

// Contadores para cada uma das condições testadas
long match_some_test = 0,
     palindromes = 0,
     repeated_seqs = 0,
     sums_are_ap = 0,
     have_tripled_digits = 0,
     have_four_repetitions = 0;

int thread_count = 8;
pthread_mutex_t mutex;

/* check_num: concentra todos os testes a serem aplicados a cada número.
 */

struct range{
    long from,to;
    int ndigits;
} range;

void check_num(long n, int ndigits);
void *check_range(void *data);

int main( int argc, char* argv[] )
{
    int  ndigits; // núm. de dígitos para representar até o maior número

    long thread, tmp, maxnum;

    struct timeval t1, t2; // marcação do tempo de execução

    // tratamento da linha de comando
    if (argc!=2) {
        fprintf(stderr,"usage: %s maxnum\n",argv[0]);
        exit(1);
    }
    maxnum = atol(argv[1]);
    pthread_mutex_init(&mutex, NULL); // ninicia o mutex

    // determinação de ndigits em função do maxnum
    tmp = maxnum;
    ndigits=0;
    do {
        ndigits++;
        tmp=tmp/10;
    } while (tmp>0);

    pthread_t thread_handles[thread_count];
    // Marca o tempo e checa cada número na faixa definida.
    // Note que o valor do parâmetro maxnum é considerado inclusive (<=)
    gettimeofday(&t1,NULL);
    long quota = maxnum/thread_count;
    for(thread = 0; thread < thread_count-1; ++thread){
        struct range *param;
        param = malloc(sizeof(param));
        param->from = thread*quota;
        param->to = thread*quota + quota-1;
        param->ndigits = ndigits;
        pthread_create(&thread_handles[thread], NULL, check_range, (void*) param);
    }
    struct range *param;
    param = malloc(sizeof(param));
    param->from = thread*quota;
    param->to = maxnum;
    param->ndigits = ndigits;
    pthread_create(&thread_handles[thread], NULL, check_range, (void*) param);
    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    gettimeofday(&t2,NULL);

    // Escrita das estatísticas ao final da execução
    printf("%ld match_some_test (%d%%)\n", match_some_test, (int)((100.0*match_some_test)/maxnum));
    printf("%ld palindromes\n", palindromes);
    printf("%ld repeated_seqs\n", repeated_seqs);
    printf("%ld sums_are_ap\n", sums_are_ap);
    printf("%ld have_tripled_digits\n", have_tripled_digits);
    printf("%ld have_four_repetitions\n", have_four_repetitions);
    print_max( ndigits );
    printf("\ntempo: %lf\n",timediff(&t2,&t1));
}

void check_num(long n, int ndigits)
{
    int all, pal, rep, sum, dou, fou;
    digit_t num;
    long orign = n;

    // Transforma número (n) em vetor de dígitos (num)
    break_into_digits(n, num, ndigits);

    // Aplica os diversos testes a um dado número
    pal = is_palindrome( num, ndigits );
    rep = has_repeated_seq( num, ndigits );
    sum = sum_is_ap( num, ndigits );
    dou = has_tripled_digits( num, ndigits );
    fou = has_four_repetitions( num, ndigits );

    // Para processar número de condições satisfeitas
    all = pal + rep + sum + dou + fou;
    pthread_mutex_lock(&mutex);
    if (all>0) {
        match_some_test += 1;
    }
    update_max( orign, all );

    // Atualiza os contadores por condição
    palindromes += pal;
    repeated_seqs += rep;
    sums_are_ap += sum;
    have_tripled_digits += dou;
    have_four_repetitions += fou;
    pthread_mutex_unlock(&mutex);
}

void *check_range(void *data){
    long i;
    struct range *param = (struct range *)data;
    for (i=param->from;i<=param->to;++i) {
        check_num(i,param->ndigits);
    }
    return NULL;
}

