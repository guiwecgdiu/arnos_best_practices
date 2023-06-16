#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//TO-DO:
//Define the basic types
typedef unsigned char U8;
typedef signed char S8;
typedef signed char  *S8P;
typedef signed short S16;
typedef signed short *S16P;
typedef signed long S32;
typedef signed long    *S32P;
typedef signed long long S64;
typedef signed long long *S64P;



//To-DO:
//Define the Constants
// // Initialize the arrays with the given coefficients, scaled by the scale factor
#define col_num_used             40
#define BUTTERWORTH_FILT_SCALE_FACTOR 65526
#define BUTTERWORTH_FILT_SHIFT_FACTOR 16
#define NZEROS 2
#define NPOLES 2
#define POS_TH -5
#define NEG_TH +5
#define COEFICIENT_LEN       3
#define MAX_EDGE     3*COEFICIENT_LEN
#define MAXEXTEND_SIZE  (col_num_used +2 *MAX_EDGE)
#define PHY_NODE_MAX 84

#define REAL_NEG_TOUCH_TH -20
#define REAL_POS_TOUCH_TH 20


//debug
#define SAMPLES 40
#define FREQUENCY 2


//cutoff =2.0
S32 bCoefficients[NZEROS + 1] = {59963, -119927,   59963};
S32 aCoefficients[NPOLES + 1] = {65536, -119452,   54866};

// cutoff =1.0 better for the double finger
// S32 bCoefficients[NZEROS + 1] = {62688, -125376, 62688};
// S32 aCoefficients[NPOLES + 1] = {65536, -125252, 59963};


//TO-DO:
//Define the Debug Function
//Output to csv for the debug
void write_to_csv_S64(const char* filename, S16P number, U8 num) 
{
    FILE *file = fopen(filename,"w");
    if (file == NULL) {
        printf("Error opening file \n");
        return;
    }
    for(U8 i = 0; i < num; i++) {
        fprintf(file, "%hd", number[i]); // %hd is used for short int
        if(i < num -1){
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");
    fclose(file);
}

void generate_square_wave(S16P signal, U8 samples, U8 frequency)
{
    U8 i;
    for (i = 0; i < samples; i++)
    {
        if ((i / frequency) % 2 == 0)
        {
            signal[i] = 32; // 最大的16位有符号整数
        }
        else
        {
            signal[i] = -32; // 最小的16位有符号整数
        }
    }
}




//TO-DO:
//Define the Filter Function:
//filt
void filter(S16P extendedArray, S16P extendedArray_filt, S32P bCoefficients, S32P aCoefficients, U8 extendSize)
{
    U8 n, i, j;

    for (n = 0; n < extendSize; n++)
    {
        for (i = 0; i < NZEROS + 1; i++)
        {
            if (n >= i)   // Ensure index is not negative
            {
                // extendedArray_filt[n] += (bCoefficients[i] * extendedArray[n - i]);
                extendedArray_filt[n] += (bCoefficients[i] * extendedArray[n - i]) >> BUTTERWORTH_FILT_SHIFT_FACTOR;
            }
        }
        for (j = 1; j < NPOLES + 1; j++) // Start from 1 to avoid self-loop
        {
            if (n >= j)   // Ensure index is not negative
            {
                // extendedArray_filt[n] -= (aCoefficients[j] * extendedArray_filt[n - j]);
                extendedArray_filt[n] -= (aCoefficients[j] * extendedArray_filt[n - j]) >> BUTTERWORTH_FILT_SHIFT_FACTOR;
            }
        }
    }
}


void filter2_transposed(S16P extendedArray, S16P extendedArray_filt, S32P bCoefficients, S32P aCoefficients, U8 extendSize)
{
    U8 n, i, j;
    S32 d[NPOLES] = {0}; // 初始化状态变量数组

    for (n = 0; n < extendSize; n++)
    {
        S32 w = extendedArray[n]; // 当前输入

        // 应用前馈（FIR）部分
        for (i = 0; i < NZEROS+1; i++)
        {
            if (i > 0) // 避免自环
            {
                w += (bCoefficients[i] * d[i-1]);
            }
            else
            {
                w += (bCoefficients[i] * extendedArray[n]);
            }
        }

        // 移动状态变量数组
        for (j = NPOLES-1; j > 0; j--)
        {
            d[j] = d[j-1] - ((aCoefficients[j+1] * w));
        }

        // 更新最新的状态变量
        d[0] = w;

        // 保存滤波后的结果
        extendedArray_filt[n] = w;
    }
}


//TO-DO:
//Define the butterworth function
// // Call the function for forward and backward filtering
//ButterworthFilt
S16 Butter_hipass_filter_tolowcut(S16P S16pData, S16P S16pFilteredData, S16P s16pCancelData, U8 u8Len)
{   


    U8 edge = MAX_EDGE;
    U8 extendSize = u8Len + 2 * edge;
    S16 extendedArray[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_filt[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_rev_filt[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_rev[MAXEXTEND_SIZE] = {0};
    S16 extendedArrayMirr[MAXEXTEND_SIZE] = {0};
    U8 i, n, j;




    //TO-DO:
    //Mirror Padding , avoid the edge
    for (i = 0; i < extendSize; i++)
    {
        if (i < edge)
        {
            // Reflect the start of the original array
            extendedArray[i] = S16pData[edge - i];
        }
        else if (i >= u8Len + edge)
        {
            // Reflect the end of the original array
            extendedArray[i] = S16pData[u8Len - 1 - (i - (u8Len + edge))];
        }
        else
        {
            // Use the original array
            extendedArray[i] = S16pData[i - edge];
        }
    }
    

    filter(extendedArray,extendedArray_filt,bCoefficients,aCoefficients,extendSize);
    for (n = 0; n < extendSize; n++)
    {
        extendedArray_rev[n] = extendedArray_filt[extendSize - 1 - n];
    }
    filter(extendedArray_rev,extendedArray_rev_filt,bCoefficients,aCoefficients,extendSize);
    // Reverse the filtered reversed array to get the backward filtering result
    S16 extendedArray_rev_filt_rev[extendSize];
    for (n = 0; n < extendSize; n++)
    {
        extendedArray_rev_filt_rev[n] = extendedArray_rev_filt[extendSize - 1 - n];
    }

    // Copy the result to the filtered_data array
    for (i = 0; i < u8Len; i++)
    {
        S16pFilteredData[i] = extendedArray_rev_filt_rev[i + edge];
    }

    write_to_csv_S64("filterPad.csv", extendedArray_rev_filt_rev,extendSize);
    write_to_csv_S64("filter.csv", S16pFilteredData,u8Len);

    // Copy the result to the filtered_data array
    for (i = 0; i < u8Len; i++)
    {
        //Lowcut part , kill the noise between -80->+80
        if (S16pFilteredData[i] < POS_TH && S16pFilteredData[i] > NEG_TH)
        {
            // S16pFilteredData[i] = 0;
        }
    }

    

    for (i = 0; i < u8Len; i++)
    {
        s16pCancelData[i] = S16pData[i] - S16pFilteredData[i];
    }

    return 0;
}


S16 PreprocessTheEdge (S16P S16pData, S16P S16pFilteredData, S16P s16pCancelData, U8 u8Len)
{
    U8 edge = MAX_EDGE;
    U8 extendSize = u8Len + 2 * edge;
    //Coefficient cutoff 1.0
    S32 bCoefficients[NZEROS + 1] = {62688, -125376, 62688};
    S32 aCoefficients[NPOLES + 1] = {65536, -125252, 59963};

    S16 extendedArray[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_filt[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_rev_filt[MAXEXTEND_SIZE] = {0};
    S16 extendedArray_rev[MAXEXTEND_SIZE] = {0};
    S16 extendedArrayMirr[MAXEXTEND_SIZE] = {0};
    U8 i, n, j;

    //5789160
    // //mirror Padding the array
    for (i = 0; i < extendSize; i++)
    {
        if (i < edge)
        {
            // Reflect the start of the original array
            extendedArray[i] = S16pData[edge - i];
        }
        else if (i >= u8Len + edge)
        {
            // Reflect the end of the original array
            extendedArray[i] = S16pData[u8Len - 1 - (i - (u8Len + edge))];
        }
        else
        {
            // Use the original array
            extendedArray[i] = S16pData[i - edge];
        }
    }

    for (i = 0; i < NZEROS; i++)
    {
        // extendedArray_filt[i] = extendedArray[0]*aCoefficients[2] >> BUTTERWORTH_FILT_SHIFT_FACTOR;
        extendedArray_filt[i] = 0;
    }
    

    //Forward Filtering differential equation
    filter(extendedArray,extendedArray_filt,bCoefficients,aCoefficients,extendSize);
    for (n = 0; n < extendSize; n++)
    {
        extendedArray_rev[n] = extendedArray_filt[extendSize - 1 - n];
    }
    // Reverse the order of the array, then pass to the forward filtering differential equaion
    filter(extendedArray,extendedArray_filt,bCoefficients,aCoefficients,extendSize);
    // Reverse the filtered reversed array to get the backward filtering result
    S16 extendedArray_rev_filt_rev[extendSize];
    for (n = 0; n < extendSize; n++)
    {
        extendedArray_rev_filt_rev[n] = extendedArray_rev_filt[extendSize - 1 - n];
    }
    // Copy the result to the filtered_data array
    for (i = 0; i < u8Len; i++)
    {
        S16pFilteredData[i] = extendedArray_rev_filt_rev[i + edge];
    }
     write_to_csv_S64("filterPad.csv", extendedArray_rev_filt_rev,extendSize);
    write_to_csv_S64("filter.csv", S16pFilteredData,u8Len);
    // Copy the result to the filtered_data array
    for (i = 0; i < u8Len; i++)
    {
        //Lowcut part , kill the noise between -80->+80
        if (S16pFilteredData[i] < POS_TH && S16pFilteredData[i] > NEG_TH)
        {
            S16pFilteredData[i] = 0;
        }
    }
    for (i = 0; i < u8Len; i++)
    {
            s16pCancelData[i] = S16pData[i]-S16pFilteredData[i];
    }

    for(i=0 ; i< 1;i++){
        if(S16pData[i]<s16pCancelData[i])
        {
            s16pCancelData[i] = S16pData[i];
        }

        if(S16pData[u8Len-i-1]<s16pCancelData[u8Len-i-1])
        {
            s16pCancelData[u8Len-i-1] = S16pData[u8Len-i-1];
        }
    }

    for(i=1 ; i< u8Len-1;i++)
    {   

        s16pCancelData[i]=S16pData[i];
    }

    return 0;
}


//TO-DO:
//TOUCH REVEAL and remove
S8 removeRealTouch(S16P s16pDiff, S16P s16pCancel,S16P s16pNoiseSample,U8 u8Len)
{   

    //declare
    S8 s8PreTouchListPtr[PHY_NODE_MAX];
    U8 i;
    //..

    //initialize
    S8 s8cnt =0;  //used for calcuale the number of noise

    //Find the Touch
    for(i = 0; i < u8Len; i++)
    {
        if(s16pDiff[i] - s16pCancel[i] < 0)
        {
            s8PreTouchListPtr[i] = 0;
            
        }   
        else
        {
            s8PreTouchListPtr[i] = 1;
        }
    }

    //Throughly drop the real touch

    for(i = 0; i < u8Len; i++)
    {
        if(s8PreTouchListPtr[i] == 0)
        {
            s16pNoiseSample[s8cnt]=s16pDiff[i];
            s8cnt++;
        }
        else
        {
            s16pNoiseSample[s8cnt]=-1;
            s8cnt++;
        }
    }

    return s8cnt;
}




int main()
{
    //S16 s16Signal[SAMPLES] = {2215,1318,1333,1416,1441,1091,1109,1184,1146,1185,1267,1330,1292,1329,1409,1427,1429,1465,1512,1560,1636,1677,1630,1691,1769,1811,1771,1809,1845,1921,1944,1969,2034,2026,2074,2143,2545,2611,2636,2244};
    //S16 s16Signal[SAMPLES] = {915+400,1318-400,1330-400,1416-400,1441-400,1091,1109,1184,1146,1185,1267,1330,1292,1329,1409,1427,1429,1465,1512,1560,1636,1677,1630,1691,1769,1811,1771,1809,1845,1921,1944,1969,2034,2026,2074,2143,2545-400,2611-400,2636-400,2244};
    S16 s16Signal[SAMPLES] = {39,934,937,40,725,638,40,738,398,29,935,42,842,37,732,33,31,34,32,17,28,40,33,26,28,20,14,24,21,14,14,5,17,15,23,16,26,16,};
    S16 s16Filtered_data[col_num_used]={0};
    S16 s16Filtered_data_temp[col_num_used]={0};
    S16 s16Cancel_data[col_num_used]={0};
    //S16 s16Preprocess_data[col_num_used]={0};
    S16 s16NoiseSample[col_num_used]={39,934,937,40,725,638,40,738,398,29,935,42,842,37,732,33,31,34,32,17,28,40,33,26,28,20,14,24,21,14,14,5,17,15,23,16,26,16,};
    U8 u8Len = col_num_used;
    S8 s8cnt = 0;
    U8 i;
    write_to_csv_S64("test.csv",s16Signal,u8Len);


    //PreprocessTheEdge(s16Signal,s16Filtered_data_temp,s16Preprocess_data,u8Len);
    Butter_hipass_filter_tolowcut(s16Signal,s16Filtered_data,s16Cancel_data,u8Len);
    s8cnt=removeRealTouch(s16Signal,s16Cancel_data,s16NoiseSample,u8Len);
    
    write_to_csv_S64("noisesample.csv",s16NoiseSample,s8cnt);
    write_to_csv_S64("ori.csv",s16Signal,u8Len);
    write_to_csv_S64("baked.csv",s16Filtered_data,u8Len);
    write_to_csv_S64("cancel.csv",s16Cancel_data,u8Len);
    return 0;
}