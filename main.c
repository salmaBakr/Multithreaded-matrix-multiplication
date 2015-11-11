#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
double** a;
double** b;
double** c;
/**dimensions of matrix A ang matrix B**/
int a_rows,a_cols,b_rows,b_cols;
struct dimensions
{
    int row;
    int col;
};
struct dimensions tmp;
double** readFile(char *file)
{
    char line[3000];
    FILE *fr;

    /** open the file for reading **/
    fr = fopen (file, "r");
    if(fr==NULL)
    {
        /**reading file error**/
        printf ("this file doesn't exist");
        tmp.col=-1;
        tmp.row=-1;
        return NULL;
    }
    /**get number of rows and number of columns**/
    fgets(line, 300, fr);
    sscanf (line,"row=%d col=%d", &tmp.row,&tmp.col);
    int i,j;
    double** arr;
    arr = (double**) malloc(tmp.row*sizeof(double*));
    for (i = 0; i < tmp.row; i++)
        arr[i] = (double*) malloc(tmp.col*sizeof(double));
    /**read matrix and save it in arr**/
    for(i=0; i<tmp.row; ++i)
    {
        for(j=0; j<tmp.col; ++j)
        {
            fscanf (fr, "%lf",&arr[i][j]);
        }
    }
    fclose(fr);
    return arr;
}

/**1st method computes  row in the output C matrix**/
void *matrix_mul(void * arg)
{
    int i=*(int*)arg;
    int j=0;
    double sum=0;
    int k;/**get values of row i in C matrix**/
    for(j; j<b_cols; ++j)
    {
        for(k=0; k<b_rows; ++k)
        {
            sum+=(a[i][k]*b[k][j]);
        }
        c[i][j]=sum;
        sum=0;
    }
    /**Exit the thread*/
    pthread_exit(NULL);
}
/**2nd method computes C[i][j] in the output C matrix**/
void *matrix_mulCell(void * arg)
{
    struct dimensions *d=arg;
    double sum=0;
    int k;
    for(k=0; k<b_rows; ++k)
    {
        sum+=(a[d->row][k]*b[k][d->col]);
    }
    c[d->row][d->col]=sum;
    /**Exit the thread*/
    pthread_exit(0);
}
/**write matrix C in file**/
void writeMatrix(char fileName[100])
{
    FILE * pFile;
    pFile = fopen (fileName,"w");
    int i,j;
    for(i=0; i<a_rows; ++i)
    {
        for(j=0; j<b_cols; ++j)
        {
            fprintf(pFile,"%.3f    ",c[i][j]);

        }
        fprintf(pFile,"\n");
    }
    fclose (pFile);
}
int main(int argc, char *argv[])
{
    char outputFile[100];
    char outputFile2[100];
    if(argc>1)/**read input from selected files**/
    {
         if(argv[1]==NULL)
         {
             printf("please enter name of the 1st file\n ");
             return 0;
         }
         a=readFile(argv[1]);
         a_rows=tmp.row;
         a_cols=tmp.col;
         if(argv[2]==NULL)
         {
             printf("please enter name of the 2nd file\n ");
             return 0;
         }
         /**read matrix b from file**/
        b=readFile(argv[2]);
        if(tmp.col==-1&&tmp.row==-1)
            return 0;
        b_rows=tmp.row;
        b_cols=tmp.col;
        strcpy(outputFile,argv[3]);
        strcpy(outputFile2,argv[3]);
        strcat(outputFile2,"_2");
    }
    else
    {
        strcpy(outputFile,"c.out");
        strcpy(outputFile2,"c_2.out");
        /**read matrix a from file**/
        a=readFile("a.txt");
        if(tmp.col==-1&&tmp.row==-1)
            return 0;
        a_rows=tmp.row;
        a_cols=tmp.col;
        /**read matrix b from file**/
        b=readFile("b.txt");
        if(tmp.col==-1&&tmp.row==-1)
            return 0;
        b_rows=tmp.row;
        b_cols=tmp.col;
    }
    if(a_cols!=b_rows)
    {
        /**reading file error**/
        printf ("Error: #of columns in matrix A != #of rows in matrix B\n");
         free(a);
        free(b);
        return 0;
    }
     int i,j;
        c = (double**) malloc(a_rows*sizeof(double*));
        for (i = 0; i < a_rows; i++)
            c[i] = (double*) malloc(b_cols*sizeof(double));
    /**execute 1st method a thread computes each row in the output C matrix**/
    struct timeval stop, start;
    /**to get start time**/
    gettimeofday(&start, NULL);
    pthread_t thread[a_rows];
    int tid[a_rows];
    for(i=0; i<a_rows; ++i)
    {
        tid[i]=i;
        /**create new thread to compute the elements in row number i**/
        if(pthread_create(&thread[i], NULL, matrix_mul, &tid[i]))
        {
            printf("Can not create a thread\n");
            free(a);
            free(b);
            free(c);
            exit(1);
        }
    }

    for(i=0; i<a_rows; ++i)
    {
        /**make main thread wait until all child threads finish their work**/
        pthread_join(thread[i],NULL);
    }
    /**to get stop time**/
    gettimeofday(&stop, NULL);
    /**print execution time for method 1**/
    printf("Method 1 : a thread computes each row in the output C matrix \nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\nnumber of threads : %d\n____________________________________________________\n", stop.tv_usec - start.tv_usec,a_rows);
    /**print result in the output file**/
    writeMatrix(outputFile);
//========================================================================================================
    /**execute 2nd method a thread computes each element in the output C matrix**/
    pthread_t threads2[a_rows][b_cols];
    struct dimensions *d;
    struct timeval stop2, start2;
    /**to get start time**/
    gettimeofday(&start2, NULL);
    for(i=0; i<a_rows; ++i)
    {
        for(j=0; j<b_cols; ++j)
        {
            /** allocate for each thread it own struct
               thread it self will free it*/
            d=malloc(sizeof(struct dimensions));
            d->row=i;
            d->col=j;
            /**create thread to compute the value of element c[i][j]**/
            if( pthread_create(&threads2[i][j], NULL, matrix_mulCell, d))
            {
                printf("Can not create a thread\n");
                free(a);
                free(b);
                free(c);
                exit(1);
            }
        }
    }
    for(i=0; i<a_rows; ++i)
    {
        for(j=0; j<b_cols; ++j)
        {
            /**make main thread wait for child threads**/
            pthread_join(threads2[i][j],NULL);
        }
    }
    /**to get stop time**/
    gettimeofday(&stop2, NULL);
    /**print execution time for method 2**/
    printf("Method 2 : a thread computes each row in the output C matrix \nSeconds taken %lu\n", stop2.tv_sec - start2.tv_sec);
    printf("Microseconds taken: %lu\nnumber of threads : %d\n____________________________________________________\n", stop2.tv_usec - start2.tv_usec,a_rows*b_cols);
    /**print result in the output file**/
    writeMatrix(outputFile2);
    free(a);
    free(b);
    free(c);
    return 0;
}
