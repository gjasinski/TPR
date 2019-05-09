#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

int size, buckets, threads;

typedef struct node {
  int val;
  struct node *next;
} node;

int check_order(int *A, unsigned int size) {
  for(int i = 0; i < size - 1; ++i) {
    if (A[i] > A[i+1]) return 0;
  }
  return 1;
}

void print_array(int *A, unsigned int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ",A[i]);
  }
  printf("\n");
}
 
int *generate_array(unsigned int size) {
  srand(time(NULL));
  int *A = (int *) calloc(sizeof(int), size);
  for (int i = 0; i < size; ++i) {
    A[i] = rand() % 100;
  }
 
  return A;
}

node *get_max(node**head) {
  node*w = (node*)malloc(sizeof(node));
  w->next = *head;
  node*p = *head;
  node*q = w;
 
  while(p->next !=  NULL){
    if (p->next->val > q->next->val)
      q = p;
 
    p = p->next;
  }
 
  node*max = q->next;
  q->next = max->next;
  max->next = NULL;
 
  *head = w->next;
  free(w);
 
  return max;
}

int get_max_val(int *A, int n) {
  int max = A[0];
  for (int i = 1; i < n; ++i) {
    if (A[i] > max)
      max = A[i];
  }
 
  return max;
}

node *sort_bucket(node *head) {
  node *res = NULL;
 
  while (head) {
    node *p = get_max(&head);
    p->next = res;
    res = p;
  }
 
  return res;
}
 
 
void bucket_sort(int *A, unsigned int size, int k, int threads) {
  node **B = (node **) calloc(sizeof(node*), k * threads);
 
  int max = get_max_val(A, size);
  int idx = 0;

  
  double bucket_size = (double) max / (double) k;
  #pragma omp parallel num_threads(threads)
  {
 //printf("number of Threads1: %d\n", omp_get_thread_num());
  #pragma omp for private(idx)
  for (int i = 0; i < size; ++i) {
    int thread = omp_get_thread_num();
    
   // printf("number of Threads2: %d\n", omp_get_num_threads());
    idx = (double)A[i] / bucket_size;
    if (idx == k)
      idx--;


  //  printf("moj kobelek %d A[i]: %d bucket_size: %d", idx, A[i], bucket_size);
    node *p = (node *) malloc(sizeof(node));
    p->val = A[i];
    p->next = B[idx + (thread) * k];

 
     
    B[idx + (thread) * k] = p;
    printf("wrzucam wartosc %d do kubelka %d aa %d\n", A[i], idx  + thread * k, idx);
  }
  }

  int p = 0;
  int i;
  
  for(i = 0; i< k; i++) {
    for(int j = 1; j < threads; j++){
        node* head = B[i];
        while(head != NULL && head->next != NULL){
           head = head->next;
        }
        if(head != NULL){
           head->next = B[i + j*k];
        }
        else{
           head = B[i +j*k];
          B[i]=head;
     }}
   
  }
  for(i = 0; i< k; i++) {
 node* head = B[i];
 printf("\n");
    while(head!= NULL){
      printf("%d ", head->val);
      head=head->next;

}  
  B[i] = sort_bucket(B[i]);
    head = B[i];
    printf("\n");
    while(head!= NULL){
      printf("%d ", head->val);
      head=head->next;

}}
 printf("\n");
  p = 0;
  
  for (int i = 0; i < k; i++) {
    for (node *a = B[i]; a != NULL; a = a->next) {
      A[p] = a->val;
      p++;
printf("A[%d]=%d\n", p, a->val);
    }
    free(B[i]);
  }
  
  free(B);
}

int main(int argc, char**argv){
  int next_option = -1;
  const char* short_opt = "n:k:t:";
  
  do {
    next_option = getopt(argc, argv, short_opt);
    if (next_option == 'n') {
	size = atoi(optarg);
    } else if(next_option == 'k') {
	buckets = atoi(optarg);
    } else if(next_option == 't'){
	threads = atoi(optarg);
   }  else {
       next_option = -1;
   } 
  } while(next_option != -1);
  printf("Rozmiar tablicy: %d Ilosc bucketow: %d, ilosc watkow %d\n", size, buckets, threads);

  int *a = generate_array(size);

  double start = omp_get_wtime();

 print_array(a, size);
  bucket_sort(a, size, buckets, threads);
  print_array(a, size);

  double end = omp_get_wtime();

  double delta = end - start;

  printf("%f\n", delta);
  printf("Tablica jest posortowana rosnaco : %s\n", check_order(a, size) == 1 ? "TAK" : "NIE");
  return 0;
}

