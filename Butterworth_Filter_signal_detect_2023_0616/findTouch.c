#include <stdio.h>
#include <stdint.h>

// Define the constants
#define PHY_NODE_MAX 100
#define REAL_POS_TOUCH_TH  360
#define REAL_NEG_TOUCH_TH  -360

// Define the types
typedef int16_t S16;
typedef int8_t S8;
typedef uint8_t U8;
typedef int32_t S32;

// Function declaration
void removeRealTouch(S16* s16FilteredPtr, S16* s16pDiff, U8 u8Len, S16 s16TouchTh);

int main() {
    // Test data
    S16 s16FilteredPtr[PHY_NODE_MAX] = {0};
    S16 s16pDiff[PHY_NODE_MAX] = {0, 100, 200, 300, 400, 900, 600, 900, 800, 900};
    U8 u8Len = 10;
    S16 s16TouchTh = 500;

    // Call the function
    removeRealTouch(s16FilteredPtr, s16pDiff, u8Len, s16TouchTh);

    // Print the results
    for (U8 i = 0; i < u8Len; i++) {
        printf("s16pDiff[%d] = %d\n", i, s16pDiff[i]);
    }

    return 0;
}

// Function definition
void removeRealTouch(S16* s16FilteredPtr, S16* s16pDiff, U8 u8Len, S16 s16TouchTh) {

    S16 BackUpDif[PHY_NODE_MAX]; // only for test
    S8 s8PreTouchListPtr[PHY_NODE_MAX];
    S32 s16DiffAvg = 0;
    S32 s16DiffSum = 0;
    S8 s8cnt = 0;
    S16 s16CompDif = 0;
    memset((void *)s16FilteredPtr, 0x00, u8Len * 2);
    memcpy((void *)&BackUpDif[0], (void *)s16pDiff, u8Len * 2);
    //Find Average
    for (U8 i = 0; i < u8Len; i++)
    {
        if (s16pDiff[i] >= REAL_NEG_TOUCH_TH && s16pDiff[i] <= REAL_POS_TOUCH_TH)
        {
            s16DiffSum += s16pDiff[i];
            s8PreTouchListPtr[i] = 0;
            s8cnt++;
        }
        else
        {
            s8PreTouchListPtr[i] = 1;
        }
    }
    if (s8cnt != 0)
    {
        s16DiffAvg = s16DiffSum / s8cnt;
    }

    //recursive to remove touch
    s16DiffSum = 0;
    for (U8 i = 0; i < u8Len; i++)
    {
        if (s8PreTouchListPtr[i] == 0)
        {
            s16CompDif = (s16pDiff[i] - s16DiffAvg);
            if (s16CompDif >= s16TouchTh)
            {
                s16pDiff[i] = s16pDiff[i] - s16CompDif - s16CompDif / s8cnt - s16CompDif / (s8cnt * s8cnt);
            }
            s16DiffSum += s16pDiff[i];
        }
    }
    if (s8cnt != 0)
    {
        s16DiffAvg = s16DiffSum / s8cnt;
    }

    //replace real touched point
    if (s8cnt != 0)
    {
        for (U8 i = 0; i < u8Len; i++)
        {
            if (s8PreTouchListPtr[i] == 1)
            {
                s16pDiff[i] = s16DiffAvg;
            }
        }
    }

}