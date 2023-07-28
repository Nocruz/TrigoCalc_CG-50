#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/misc.h>
#include <stdlib.h>
#include <stdio.h>

// int active_view;
// 0 : Select option
// 1 : Solve Button
// 2 : Graph triangle
int active_view = 0;

// int active_fkeys;
// 0 : Select option
// 1 : Side or angle
// 2 : Input Side
// 3 : Input Angle
int active_fkeys = 0;

// Side values
int sideA = 0;
int sideB = 0;
int sideC = 0;

// Angle values
int angleA = 0;
int angleB = 0;
int angleC = 0;

char *statusMessage;
char statusColor = TEXT_COLOR_WHITE;

void DrawMain ();
void DrawFKeys (int fkeys);

void DrawFKeySet (int ptr, int fkeys[]);
void DrawInputPopup (char* fieldName, int *value);
void DrawPopup (char* message);

void SolveTriangle();
int ValidateInput();

int main(void)
{
    int key;
    DefineStatusAreaFlags (3, SAF_BATTERY | SAF_ALPHA_SHIFT | SAF_TEXT | SAF_SETUP_ANGLE, &statusColor, &statusColor);

    DrawMain ();
    DrawFKeys (0);

    while (1)
    {
        GetKey (&key);

        switch (active_fkeys)
        {
            case 0:
                if (key == KEY_CTRL_F1)
                    DrawFKeys (1);
                if (key == KEY_CTRL_F2)
                    SolveTriangle ();
                break;
            case 1:
                if (key == KEY_CTRL_F1)
                    DrawFKeys (2);
                if (key == KEY_CTRL_F2)
                    DrawFKeys (3);

                if (key == KEY_CTRL_EXIT)
                    DrawFKeys (0);
                if (key == KEY_CTRL_QUIT)
                    DrawFKeys (0);
                break;
            case 2:
                if (key == KEY_CTRL_F1)
                    DrawInputPopup ("--A:", &sideA);
                if (key == KEY_CTRL_F2)
                    DrawInputPopup ("--B:", &sideB);
                if (key == KEY_CTRL_F3)
                    DrawInputPopup ("--C:", &sideC);

                if (key == KEY_CTRL_EXIT)
                    DrawFKeys (1);
                if (key == KEY_CTRL_QUIT)
                    DrawFKeys (0);
                break;
            case 3:
                if (key == KEY_CTRL_F1)
                    DrawInputPopup ("--A:", &angleA);
                if (key == KEY_CTRL_F2)
                    DrawInputPopup ("--B:", &angleB);
                if (key == KEY_CTRL_F3)
                    DrawInputPopup ("--C:", &angleC);
                    
                if (key == KEY_CTRL_EXIT)
                    DrawFKeys (1);
                if (key == KEY_CTRL_QUIT)
                    DrawFKeys (0);
                break;
            default:
                break;
        }

        Bdisp_PutDisp_DD ();
    }
 
    return 0;
}

void DrawMain()
{
    active_view = 0;

    Bdisp_AllClr_VRAM();
    DisplayStatusArea();

    PrintXY(1, 1, "--TRIGOCALC", 0, TEXT_COLOR_BLUE);
    PrintXY(1, 2, "--Side values:", 0, TEXT_COLOR_BLACK);
    PrintXY(1, 5, "--Angle values:", 0, TEXT_COLOR_BLACK);

    char *value = (char*)malloc(128);
    value[0] = '\0';

    int x = 0; int y = 47; itoa(sideA, (unsigned char*)value);
    PrintMini(&x, &y, "a = ", 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);
    x = 0; y += 15; itoa(sideB, (unsigned char*)value);
    PrintMini(&x, &y, "b = ", 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);
    x = 0; y += 15; itoa(sideC, (unsigned char*)value);
    PrintMini(&x, &y, "c = ", 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_RED, COLOR_GRAY, 1, 0);

    x = 0; y = 120; itoa(angleA, (unsigned char*)value);
    PrintMini(&x, &y, "alpha = ", 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);
    x = 0; y += 15; itoa(angleB, (unsigned char*)value);
    PrintMini(&x, &y, "beta  = ", 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);
    x = 0; y += 15; itoa(angleC, (unsigned char*)value);
    PrintMini(&x, &y, "gamma = ", 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);
    PrintMini(&x, &y, value, 0x02, 0xffffffff, 0,0, COLOR_BLUE, COLOR_GRAY, 1, 0);

    free(value);
}

#define FKEY_BLANK 0x0000

#define FKEY_INSERT 0x000B
#define FKEY_SOLVE 0x039F
#define FKEY_GRAPH 0x0001

#define FKEY_SIDE 0x0059
#define FKEY_ANGLE 0x0133

#define FKEY_A 0x0260
#define FKEY_B 0x014E
#define FKEY_C 0x0261

int iresult;
int fkey_set[6];
void DrawFKeys (int fkeys)
{
    active_fkeys = fkeys;

    if (active_fkeys == 0)
    {
        fkey_set[0] = FKEY_INSERT;
        fkey_set[1] = FKEY_SOLVE;
        fkey_set[2] = FKEY_GRAPH;

        fkey_set[3] = FKEY_BLANK;
        fkey_set[4] = FKEY_BLANK;
        fkey_set[5] = FKEY_BLANK;
    }
    else if (active_fkeys == 1)
    {
        fkey_set[0] = FKEY_SIDE;
        fkey_set[1] = FKEY_ANGLE;

        fkey_set[2] = FKEY_BLANK;
        fkey_set[3] = FKEY_BLANK;
        fkey_set[4] = FKEY_BLANK;
        fkey_set[5] = FKEY_BLANK;
    }
    else if (active_fkeys == 2 || active_fkeys == 3)
    {
        fkey_set[0] = FKEY_A;
        fkey_set[1] = FKEY_B;
        fkey_set[2] = FKEY_C;

        fkey_set[3] = FKEY_BLANK;
        fkey_set[4] = FKEY_BLANK;
        fkey_set[5] = FKEY_BLANK;
    }
    else if (active_fkeys == -1)
    {
        fkey_set[1] = FKEY_BLANK;
        fkey_set[2] = FKEY_BLANK;
        fkey_set[3] = FKEY_BLANK;
        fkey_set[4] = FKEY_BLANK;
        fkey_set[5] = FKEY_BLANK;
    }

    DrawFKeySet (iresult, fkey_set);
}

void DrawFKeySet (int ptr, int fkeys[])
{
    for (int i = 0; i < 6; i++)
    {
        GetFKeyPtr(fkeys[i], &ptr);
        FKey_Display(i, (int*)ptr);
    }
}

void DrawInputPopup (char* fieldName, int *value)
{
    MsgBoxPush(3);
    PrintXY(3, 3, "--Enter Value", 0, TEXT_COLOR_BLACK);
    PrintXY(6, 5, fieldName, 0, TEXT_COLOR_BLACK);

    char *inputBuffer = (char*)malloc(128);
    int start = 0;
    int cursor = 0;
    inputBuffer[0] = '\0';

    DisplayMBString2(0, (unsigned char*)inputBuffer, start, cursor, 0, 8, 96, 18, 0);

    int boxInput;

    while (1)
    {
        GetKey (&boxInput);
        if (boxInput == KEY_CTRL_EXE || boxInput == KEY_CTRL_EXIT || boxInput == KEY_CTRL_QUIT || boxInput == KEY_CTRL_MENU)
            break;

        
        if(boxInput && (boxInput >= 0x0030 && boxInput <= 0x0039))
        {   
            cursor = EditMBStringChar((unsigned char*)inputBuffer, 128, cursor, boxInput);
            DisplayMBString2(0, (unsigned char*)inputBuffer, start, cursor, 0, 8, 96, 18, 0);
        }
        else
            EditMBStringCtrl2((unsigned char*)inputBuffer, 128, &start, &cursor, &boxInput, 8, 96, 1, 18);
    }

    MsgBoxPop();
    Cursor_SetFlashOff();

    *value = atoi(inputBuffer);
    free(inputBuffer);
    DrawMain();
    DrawFKeys(0);
}

void DrawPopup (char* message)
{
    MsgBoxPush(3);
    PrintXY(3, 3, message, 0, TEXT_COLOR_BLACK);
    PrintXY(6, 5, "--Press: [EXIT]", 0, TEXT_COLOR_BLACK);

    int boxInput;
    while (1)
    {
        GetKey (&boxInput);
        if (boxInput == KEY_CTRL_EXE || boxInput == KEY_CTRL_EXIT || boxInput == KEY_CTRL_QUIT || boxInput == KEY_CTRL_MENU)
            break;
    }

    MsgBoxPop();
    DrawMain();
    DrawFKeys(0);
}

char* INVALID_ANGLES_ERROR = "--INVALID ANGLES!";

void SolveTriangle()
{
    switch (ValidateInput())
    {
        case 0:
            DrawPopup(INVALID_ANGLES_ERROR);
            angleA = 0;
            angleB = 0;
            angleC = 0;
            break;
    
        default:
            break;
    }
    
    DrawMain();
    DrawFKeys(0);
}

int ValidateInput()
{
    if (angleA >= 179 || angleB >= 179 || angleC >= 179)
        return 0;

    return 1;
}