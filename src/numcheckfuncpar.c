/* Você deve implementar uma versão usando paralelismo de funções (também
 * chamado decomposição por funções (function decomposition)
 * Para simplificar, você pode considerar que seu programa criará as
 * threads que você julgar necessárias para processar todos os números,
 * onde cada thread executará uma função diferente, com parte das ações
 * esperadas do programa final.
 * 
 * O programa deve aceitar o mesmo parâmetro de linha de comando da versão
 * sequencial - e nenhum outro. 
 * A saída deve sequir exatamente o mesmo formato da versão sequencial.
 */
// Contadores para cada uma das condições testadas
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "timediff.h"   // calcula tempo decorrido
#include "numchecks.h"  // conta números com mais condições válidas
#include "conditions.h" // verifica cada condição

long match_some_test = 0,
     palindromes = 0,
     repeated_seqs = 0,
     sums_are_ap = 0,
     have_tripled_digits = 0,
     have_four_repetitions = 0;

int thread_count = 6;
pthread_mutex_t mutex;


/* check_num: concentra todos os testes a serem aplicados a cada número.
 */
struct problem{
    long n;
    int ndigits;
} problem;

// void check_num(long n, int ndigits);
void *check_palindrome(void *data);
void *check_repeated_seq(void *data);
void *check_sum_is_pa(void *data);
void *check_has_triple_digits(void *data);
void *check_has_four_reps(void *data);
void *update_maxes(void *data);

unsigned char *bytes;
unsigned char filter = 5 << 5; //mascara para validar que o numero passou pelos 5 testes.

void count_up_checks(long n, int checked)
{
    pthread_mutex_lock(&mutex);
    bytes[n] += 1 << 5;
    bytes[n] += checked;
    pthread_mutex_unlock(&mutex);
}

int all_checks_done(long n)
{
    // pthread_mutex_lock(&mutex);
    return (bytes[n] >= filter);
    // pthread_mutex_unlock(&mutex);

}

int get_checked_amount(long n)
{
    // pthread_mutex_lock(&mutex);
    return bytes[n] - filter;
    // pthread_mutex_unlock(&mutex);
}

int main( int argc, char* argv[] )
{
    int  ndigits; // núm. de dígitos para representar até o maior número

    long thread, tmp, maxnum;

    struct timeval t1, t2; // marcação do tempo de execução

    // tratamento da linha de comando
    if (argc!=2) {
        fprintf(stdout,"usage: %s maxnum\n",argv[0]);
        exit(1);
    }
    maxnum = atol(argv[1]);
    bytes = calloc(maxnum+1,sizeof(unsigned char));

    // determinação de ndigits em função do maxnum
    tmp = maxnum;
    ndigits=0;
    do {
        ndigits++;
        tmp=tmp/10;
    } while (tmp>0);

    pthread_mutex_init(&mutex, NULL); // ninicia o mutex

    pthread_t thread_handles[thread_count];
    struct problem *param;
    param = malloc(sizeof(param));
    param->ndigits = ndigits;
    param->n = maxnum;
    // Marca o tempo e checa cada número na faixa definida.
    gettimeofday(&t1,NULL);
    pthread_create(&thread_handles[0], NULL, check_palindrome, (void*) param);
    pthread_create(&thread_handles[1], NULL, check_repeated_seq, (void*) param);
    pthread_create(&thread_handles[2], NULL, check_sum_is_pa, (void*) param);
    pthread_create(&thread_handles[3], NULL, check_has_triple_digits, (void*) param);
    pthread_create(&thread_handles[4], NULL, check_has_four_reps, (void*) param);
    pthread_create(&thread_handles[5], NULL, update_maxes, (void*) param);
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
    free(bytes);
}

void *check_palindrome(void *data)
{
    struct problem param = *(struct problem *)data;
    int pal,i;
    digit_t num;
    for (i=0;i<=param.n;++i) {
        // Transforma número (n) em vetor de dígitos (num)
        break_into_digits(i, num, param.ndigits);

        // Aplica os diversos testes a um dado número
        pal = is_palindrome( num, param.ndigits );

        // Atualiza os contador de palindromos
        palindromes += pal; //nao preciso de mutex aqui pois so essa thread mexe nessa global
        count_up_checks(i, pal);
    }
    return NULL;
}

void *check_repeated_seq(void *data)
{
    struct problem param = *(struct problem *)data; //nao preciso de mutex aqui pois threads so fazem leitura
    int rep,i;
    digit_t num;
    for (i=0;i<=param.n;++i) {
        // Transforma número (n) em vetor de dígitos (num)
        break_into_digits(i, num, param.ndigits);

        // Aplica os diversos testes a um dado número
        rep = has_repeated_seq( num, param.ndigits);

        // Atualiza os contador de palindromos
        repeated_seqs += rep; //nao preciso de mutex aqui pois so essa thread mexe nessa global
        count_up_checks(i, rep);
    }
    return NULL;
}

void *check_sum_is_pa(void *data)
{
    struct problem param = *(struct problem *)data; //nao preciso de mutex aqui pois threads so fazem leitura
    int sum,i;
    digit_t num;
    for (i=0;i<=param.n;++i) {
        // Transforma número (n) em vetor de dígitos (num)
        break_into_digits(i, num, param.ndigits);

        // Aplica os diversos testes a um dado número
        sum = sum_is_ap( num, param.ndigits );

        // Atualiza os contador de palindromos
        sums_are_ap += sum; //nao preciso de mutex aqui pois so essa thread mexe nessa global
        count_up_checks(i, sum);
    }
    return NULL;
}

void *check_has_triple_digits(void *data)
{
    struct problem param = *(struct problem *)data; //nao preciso de mutex aqui pois threads so fazem leitura
    int dou,i;
    digit_t num;
    for (i=0;i<=param.n;++i) {
        // Transforma número (n) em vetor de dígitos (num)
        break_into_digits(i, num, param.ndigits);

        // Aplica os diversos testes a um dado número
        dou = has_tripled_digits( num, param.ndigits );

        // Atualiza os contador de palindromos
        have_tripled_digits += dou; //nao preciso de mutex aqui pois so essa thread mexe nessa global
        count_up_checks(i, dou);
    }
    return NULL;
}

void *check_has_four_reps(void *data)
{
    struct problem param = *(struct problem *)data; //nao preciso de mutex aqui pois threads so fazem leitura
    int fou,i;
    digit_t num;
    for (i=0;i<=param.n;++i) {
        // Transforma número (n) em vetor de dígitos (num)
        break_into_digits(i, num, param.ndigits);

        // Aplica os diversos testes a um dado número
        fou = has_four_repetitions( num, param.ndigits );

        // Atualiza os contador de palindromos
        have_four_repetitions += fou; //nao preciso de mutex aqui pois so essa thread mexe nessa global
        count_up_checks(i, fou);
    }
    return NULL;
}

void *update_maxes(void *data)
{
    struct problem param = *(struct problem *)data; //nao preciso de mutex aqui pois threads so fazem leitura
    int i,all;

    for (i=0;i<=param.n;++i) {
        while ( !(all_checks_done(i))){}
        all = get_checked_amount(i);
        if (all > 0) {
            match_some_test += 1; // nao preciso de mutex aqui pois so essa thread mexe nessa global
        }
        update_max( i, all ); // nao preciso de mutex aqui pois so essa thread mexe nessa global
    }
    return NULL;
}