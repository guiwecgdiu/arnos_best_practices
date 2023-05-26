//linear_bytter_worth_filt.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <string.h>

//define constant
#define NZEROS 2
#define NPOLES 2
// #define backward_filt
//#define forward_filt


//debug function
void write_matrix_to_csv(const char* filename, double* matrix, int rows, int cols) {
    FILE *file = fopen(filename,"w");
    if (file == NULL) {
        printf("Error opening file \n");
        return;
    }
    
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            fprintf(file, "%f", matrix[i*cols + j]);
            if(j < cols - 1){
                fprintf(file,",");
            }
        }
        fprintf(file,"\n");
    }
    
    fclose(file);
}

void write_to_csv(const char* filenmae, double* number, int num)
{
    FILE *file = fopen(filenmae,"w");
    if (file == NULL)
    {
        printf("Error openning file \n");
        return;
    }
    for(int i=0; i<num; i++) {
        fprintf(file, "%f", number[i]);
        if(i < num -1){
            fprintf(file,",");
        }
    }
    fprintf(file,"\n");
    fclose(file);
}



void subtract_arrays(double* test_data, double* filter_data, double* cancel_data, int data_length) {
    for (int i = 0; i < data_length; i++) {
        cancel_data[i] = test_data[i] - filter_data[i];
    }
}


//process function

void lfilter_zi(double* b, int len_b, double* a, int len_a, double* zi) {
    // Normalize the coefficients so a[0] == 1
    for(int i = 0; i < len_a; i++) {
        a[i] = a[i] / a[0];
    }
    for(int i = 0; i < len_b; i++) {
        b[i] = b[i] / a[0];
    }

    int n;
    if(len_a>len_b){
        n = len_a;
    }else{
        n=len_b;
    }
   double* A = (double*) malloc(sizeof(double) * (n - 1) * (n - 1)); // Companion matrix of a transposed
    double* B = (double*) malloc(sizeof(double) * (n - 1)); // b[1:] - a[1:] * b[0]


    // Initialize A to be companion matrix 
    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - 1; j++) {
            if(i == 0){
                A[i*(n-1)+j] = -a[j + 1];
            }else if(j == i - 1){
                A[i*(n-1)+j] = 1;
            }else{
                A[i*(n-1)+j] = 0;
            }
    }
    //transpose A


        if(i < len_b - 1) {
            B[i] = b[i + 1] - a[i + 1] * b[0];
        } else {
            B[i] = -a[i + 1] * b[0];
        }
    }

    double* IminusA = (double*) malloc(sizeof(double) * (n - 1) * (n - 1)); // I - A

    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - 1; j++) {
            if(i == j) {
                IminusA[i*(n-1)+j] = 1 - A[i*(n-1)+j];
            } else {
                IminusA[i*(n-1)+j] = -A[i*(n-1)+j];
            }
        }
    }
    // write_matrix_to_csv("aM.csv",A, 2,2);
    write_to_csv("a.csv",A,4);
    write_matrix_to_csv("IminusAM.csv",IminusA, 2,2);
    write_to_csv("IminusA.csv",IminusA,4);
    // write_to_csv("b.csv",B,2);

    //solve_linear_system(A, B, zi, n - 1);

    free(A);
    free(B);
}

//##butter 
//To-DO:

double* butter_hipass_filter(double* data, double* filtered_data, int len, int order, int sample_rate ){
    // define  Butterworth state array, and init to 0 or use static
    
  
    //to-do:
    //check the validity input


    //butterworth process 
    // define coefficient a and b
    double bCoefficients[NZEROS+1] = {0.91496914, -1.82993829, 0.91496914};
    double aCoefficients[NPOLES+1] = {1.0, -1.82269493, 0.83718165};


    //to-do:
    //odd padding, avoid the edge effect
    //edge = 3* max(len(a),len(b)));
    int len_a = sizeof(aCoefficients) / sizeof(aCoefficients[0]); // 数组a的长度
    int len_b = sizeof(bCoefficients) / sizeof(bCoefficients[0]); // 数组b的长度

    int edge = 3 * fmax(len_a, len_b);
    
    int extendSize = len + 2*edge;

    double* extendedArray = (double*) malloc(sizeof(double) * extendSize);
    double* extendedArray_filt = (double*) malloc(sizeof(double) * extendSize);
    memset(extendedArray_filt,0,sizeof(double)*extendSize);

    for(int i=0; i<extendSize; i++)
    {
        if(i < edge){
        // Left extension: flip the sign and mirror the array
        // add the value of the left end twice before flipping the sign
        extendedArray[i] = (2 * data[0] - data[edge - i]);
        } else if(i >= len + edge) {
        // Right extension: flip the sign and mirror the array
        // add the value of the right end twice before flipping the sign
        extendedArray[i] = (2 * data[len - 1] - data[len - 2 - (i - len - edge)]);
        } else {
        // Middle part: just copy the original array
        extendedArray[i] = data[i - edge];
        }
    }
    write_to_csv("paddinged_data.csv",extendedArray,extendSize);

    //to-do
    //calculate the filter initial
    double zi[NZEROS] ={-0.91496914 ,0.91496914};
    lfilter_zi(bCoefficients,len_b, aCoefficients,len_a,zi);
    double x0 = extendedArray[0];

#ifdef forward_filt
    //to-do
    //Forward Filtering differential equation
    for(int n = 0; n < extendSize; n++) {
        for(int i = 0; i < order+1; i++) {
            if(n - i >= 0) {  // Ensure index is not negative
                extendedArray_filt[n] += bCoefficients[i] * extendedArray[n - i];
            }
        }
        for(int j = 1; j < order+1; j++) {  // Start from 1 to avoid self-loop
            if(n - j >= 0) {  // Ensure index is not negative
                extendedArray_filt[n] -= aCoefficients[j] * extendedArray_filt[n - j];
            }
        }
    }

    for(int i=0; i<len; i++)
    {
        filtered_data[i] = extendedArray_filt[i+edge];
    }
#endif
#ifdef backward_filt
    //Backword filtering
    // Reverse the original array
    double extendedArray_rev[extendSize];
    for(int n = 0; n < extendSize; n++) {
        extendedArray_rev[n] = extendedArray[extendSize - 1 - n];
    }
    double temp[len];
    memset(temp,0,len * sizeof(double));
    
    for(int i=0; i<len; i++)
    {
        temp[i] = extendedArray_rev[i+edge];
    }

    write_to_csv("rev.csv",temp,len);


    // Forward filtering on the reversed array
    double* extendedArray_rev_filt = (double*) malloc(sizeof(double) * extendSize);
    memset(extendedArray_filt,0,sizeof(double)*extendSize);
    for(int n = 0; n < extendSize; n++) {
        for(int i = 0; i < order+1; i++) {
            if(n - i >= 0) {  // Ensure index is not negative
                extendedArray_rev_filt[n] += bCoefficients[i] * extendedArray_rev[n - i];
            }
        }
        for(int j = 1; j < order+1; j++) {  // Start from 1 to avoid self-loop
            if(n - j >= 0) {  // Ensure index is not negative
                extendedArray_rev_filt[n] -= aCoefficients[j] * extendedArray_rev_filt[n - j];
            }
        }
    }

    // Reverse the filtered reversed array to get the backward filtering result
    double extendedArray_rev_filt_rev[extendSize];
    for(int n = 0; n < extendSize; n++) {
        extendedArray_rev_filt_rev[n] = extendedArray_rev_filt[extendSize - 1 - n];
    }

    write_to_csv("back.csv",extendedArray_rev,extendSize);

    // double extendedArray_filt_final[extendSize];
    // for(int n = 0; n < extendSize; n++) {
    //     extendedArray_filt_final[n] = (extendedArray_filt[n] + extendedArray_rev_filt_rev[n]) / 2;
    // }
    

    for(int i=0; i<len; i++)
    {
        filtered_data[i] = extendedArray_rev_filt_rev[i+edge];
    }
#endif

    //to-do
    //Forward Filtering differential equation
    for(int n = 0; n < extendSize; n++) {
        for(int i = 0; i < order+1; i++) {
            if(n - i >= 0) {  // Ensure index is not negative
                extendedArray_filt[n] += bCoefficients[i] * extendedArray[n - i];
            }
        }
        for(int j = 1; j < order+1; j++) {  // Start from 1 to avoid self-loop
            if(n - j >= 0) {  // Ensure index is not negative
                extendedArray_filt[n] -= aCoefficients[j] * extendedArray_filt[n - j];
            }
        }
    }

    double extendedArray_rev[extendSize];
    for(int n = 0; n < extendSize; n++) {
        extendedArray_rev[n] = extendedArray_filt[extendSize - 1 - n];
    }

    // Forward filtering on the reversed array
    double* extendedArray_rev_filt = (double*) malloc(sizeof(double) * extendSize);
    memset(extendedArray_filt,0,sizeof(double)*extendSize);
    for(int n = 0; n < extendSize; n++) {
        for(int i = 0; i < order+1; i++) {
            if(n - i >= 0) {  // Ensure index is not negative
                extendedArray_rev_filt[n] += bCoefficients[i] * extendedArray_rev[n - i];
            }
        }
        for(int j = 1; j < order+1; j++) {  // Start from 1 to avoid self-loop
            if(n - j >= 0) {  // Ensure index is not negative
                extendedArray_rev_filt[n] -= aCoefficients[j] * extendedArray_rev_filt[n - j];
            }
        }
    }

    // Reverse the filtered reversed array to get the backward filtering result
    double extendedArray_rev_filt_rev[extendSize];
    for(int n = 0; n < extendSize; n++) {
        extendedArray_rev_filt_rev[n] = extendedArray_rev_filt[extendSize - 1 - n];
    }

    for(int i=0; i<len; i++)
    {
        filtered_data[i] = extendedArray_rev_filt_rev[i+edge];
    }




    free(extendedArray);
    free(extendedArray_filt);
}


void diff(double* arr, int size) {
    for(int i = 0; i<size - 1; i++)
    {
        arr[i] = arr[i+1] - arr[i];
    }
}

void diff_restore(double arr[], int size, double first_element){
    for(int i=size -1; i>0; i--)
    {
        arr[i] = arr[i-1] + arr[i];
    }
    arr[0] = first_element;
}

#include <stdio.h>

void diff2(double* arr, int size) {
    double prev = arr[0];
    double curr = arr[1];
    double next;
    for (int i = 0; i < size - 2; i++) {
        next = arr[i + 2];
        arr[i] = next - 2 * curr + prev;
        prev = curr;
        curr = next;
    }
}

void restore2(double* arr, int size, double first_element, double second_element) {
    for (int i = size - 3; i >= 0; i--) {
        arr[i] = arr[i] + 2 * arr[i + 1] - arr[i + 2];
    }
    arr[0] = first_element;
    arr[1] = second_element;
}

void lower_cutoff_data(double output_data [], int data_length, double pos_th, double neg_th) {
    for (int i = 0; i < data_length; i++) {
        
        double first_element = output_data[0];
        // write_to_csv("beforediff.csv",output_data,data_length);
        // diff(output_data,data_length);
        if (output_data[i] < pos_th && output_data[i] > neg_th) {
            output_data[i] = 0;
        }
        // diff_restore(output_data,data_length,first_element);
    }
}




//##main
//To-Do:
int main() {
    double test_data[] = {1315-400,1318,1333,1416,1441,1091,1109,1184,1146,1185,1267,1330,1292,1329,1409,1427,1429,1465,1512,1560,1636,1677,1630,1691,1769,1811,1771,1809,1845,1921,1944,1969,2034,2026,2074,2143,2545,2611,2636,2644-400};
    //double test_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150,90,60 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int len = sizeof(test_data)/sizeof(test_data[0]);
    write_to_csv("test_data.csv",test_data,len);
    
    double filtered_data[len];
    memset(filtered_data,0,len * sizeof(double));

    butter_hipass_filter(test_data, filtered_data,len,2.0,100);
    write_to_csv("butter_filt_data.csv",filtered_data,len);
    
    double first_element = filtered_data[0];
    double second_element =filtered_data[1];
    // diff2(filtered_data,len);
    lower_cutoff_data(filtered_data,len,+80,-80);
    // restore2(filtered_data,len,first_element,second_element);
    write_to_csv("lowcut_filt_data.csv",filtered_data,len);

    double cancel_data[len];
    memset(cancel_data,0,len * sizeof(double));
    
    subtract_arrays(test_data,filtered_data,cancel_data,len);
    write_to_csv("cancel_data.csv",cancel_data,len);

    return 0;
}