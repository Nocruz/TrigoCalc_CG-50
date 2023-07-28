#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/misc.h>
#include <stdlib.h>
#include <math.h>

#define FKEY_BLANK 0x0000
#define FKEY_INSERT 0x000B
#define FKEY_SOLVE 0x039F
#define FKEY_GRAPH 0x0001
#define FKEY_EDIT 0x002A
#define FKEY_CLEAR 0x0149

char statusColor = TEXT_COLOR_WHITE;

// 0: Main Screen
// 1: Enter Input
int currentState = 0;
int activeFKeys[6];

int isSelected = 0;
int selectionCursor = 0;

int sideA = 0; int sideB = 0; int sideC = 0;
int angleA = 0; int angleB = 0; int angleC = 0;

void SetState(int new);
void DrawState();
void DrawFKeys();
void SolveTriangle();
int InitialTriangleValidation();
void DrawPopup (char* message);
void DrawPopupBig (char* message, char* message2);

double Sqrt(double x);

int main(void)
{
    int key;
    DefineStatusAreaFlags (3, SAF_BATTERY | SAF_ALPHA_SHIFT | SAF_TEXT | SAF_SETUP_ANGLE, &statusColor, &statusColor);

    SetState(0);
    isSelected = 0;
    selectionCursor = 1;

    while (1)
    {
        GetKey(&key);

        switch (currentState)
        {
            case 0:
                if (key == KEY_CTRL_F1)
                    SetState(1);
                if (key == KEY_CTRL_F2)
                    SolveTriangle();
                break;
            case 1:
                if (key == KEY_CTRL_EXIT || key == KEY_CTRL_QUIT)
                    SetState(0);
                if (isSelected)
                {
                    if (key == KEY_CTRL_EXIT || key == KEY_CTRL_QUIT)
                    {
                        isSelected = 0;
                        SetState(1);
                    }
                    if (key == KEY_CTRL_EXE)
                    {
                        isSelected = 0;
                        SetState(1);
                    }
                }
                else
                {
                    if (key == KEY_CTRL_DOWN)
                    {
                        if (selectionCursor == 6)
                            selectionCursor = 1;
                        else
                            selectionCursor++;
                        SetState(1);
                    } 
                    else if (key == KEY_CTRL_UP)
                    {
                        if (selectionCursor == 1)
                            selectionCursor = 6;
                        else
                            selectionCursor--;
                        SetState(1);
                    }
                    else if (key == KEY_CTRL_EXE || key == KEY_CTRL_F1 || (key >= 0x0030 && key <= 0x0039) || key == KEY_CTRL_LEFT || key == KEY_CTRL_RIGHT)
                    {
                        isSelected = 1;
                        SetState(1);
                    }
                    else if (key == KEY_CTRL_F2)
                    {
                        switch (selectionCursor)
                        {
                            case 1:
                                sideA = 0;
                                    break;
                                case 2:
                                sideB = 0;
                                    break;
                                case 3:
                                    sideC = 0;
                                    break;
                                case 4:
                                    angleA = 0;
                                    break;
                                case 5:
                                    angleB = 0;
                                    break;
                                case 6:
                                    angleC = 0;
                                    break;
                                default:
                                    break;
                        }
                        SetState(1);
                    }
                }
                break;
            default:
                break;
        }
    }
    
    return 0;
}

void SetState(int new)
{
    currentState = new;
    DrawState();
}

void DrawState()
{
    Bdisp_AllClr_VRAM();
    DisplayStatusArea();
    DrawFKeys ();

    if (currentState == 0)
    {
        PrintXY(1, 1, "--TRIGOCALC", 0, TEXT_COLOR_BLUE);

        PrintXY(1, 4, "--Select Option", 0, TEXT_COLOR_BLACK);
        PrintXY(1, 5, "--F1:Input Values", 0, TEXT_COLOR_BLACK);
        PrintXY(1, 6, "--F2:Solve Triangle", 0, TEXT_COLOR_BLACK);
        PrintXY(1, 7, "--F3:Graph", 0, TEXT_COLOR_BLACK);
    } else if (currentState == 1)
    {
        PrintXY(1, 1, "--Input Values", 0, TEXT_COLOR_BLACK);

        char *value = (char*)malloc(128);
        value[0] = '\0';

        itoa(sideA, (unsigned char*)value + 2);
        PrintXY(1, 2, "--    A:               ", (selectionCursor + 1) == 2 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 2, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 2 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        itoa(sideB, (unsigned char*)value + 2);
        PrintXY(1, 3, "--    B:               ", (selectionCursor + 1) == 3 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 3, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 3 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        itoa(sideC, (unsigned char*)value + 2);
        PrintXY(1, 4, "--    C:               ", (selectionCursor + 1) == 4 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 4, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 4 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        itoa(angleA, (unsigned char*)value + 2);
        PrintXY(1, 5, "--ALPHA:              ", (selectionCursor + 1) == 5 && !isSelected? 1 : 0, TEXT_COLOR_PURPLE);
        PrintXY(7, 5, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 5 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_PURPLE);

        itoa(angleB, (unsigned char*)value + 2);
        PrintXY(1, 6, "-- BETA:              ", (selectionCursor + 1) == 6 && !isSelected? 1 : 0, TEXT_COLOR_PURPLE);
        PrintXY(7, 6, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 6 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_PURPLE);

        itoa(angleC, (unsigned char*)value + 2);
        PrintXY(1, 7, "--GAMMA:              ", (selectionCursor + 1) == 7 && !isSelected? 1 : 0, TEXT_COLOR_PURPLE);
        PrintXY(7, 7, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 7 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_PURPLE);

        free(value);

        if (!isSelected)
            return;

        char *inputBuffer = (char*)malloc(128);
        int start = 0;
        int cursor = 0;
        inputBuffer[0] = '\0';

        DisplayMBString((unsigned char*)inputBuffer, start, cursor, 7, selectionCursor + 1);

        int key;
        while (1)
        {
            GetKey (&key);
            if (key == KEY_CTRL_EXIT || key == KEY_CTRL_QUIT || key == KEY_CTRL_EXE)
            {
                isSelected = 0;
                Cursor_SetFlashOff();
                if (key != KEY_CTRL_EXE)
                {
                    free(inputBuffer);
                    SetState(1);
                    return;
                } else break;
            } 
            else if (key && (key >= 0x0030 && key <= 0x0039))
            {   
                cursor = EditMBStringChar((unsigned char*)inputBuffer, 128, cursor, key);
                DisplayMBString((unsigned char*)inputBuffer, start, cursor, 7, selectionCursor + 1);
            }
                EditMBStringCtrl((unsigned char*)inputBuffer, 128, &start, &cursor, &key, 7, selectionCursor + 1);
        }

        switch (selectionCursor)
        {
            case 1:
                sideA = atoi(inputBuffer);
                break;
            case 2:
                sideB = atoi(inputBuffer);
                break;
            case 3:
                sideC = atoi(inputBuffer);
                break;
            case 4:
                angleA = atoi(inputBuffer);
                break;
            case 5:
                angleB = atoi(inputBuffer);
                break;
            case 6:
                angleC = atoi(inputBuffer);
                break;
            default:
                break;
        }

        Cursor_SetFlashOff();
        free(inputBuffer);
        SetState(1);
    }
    
}

void DrawFKeys ()
{
    if (currentState == 0)
    {
        activeFKeys[0] = FKEY_INSERT;
        activeFKeys[1] = FKEY_SOLVE;
        activeFKeys[2] = FKEY_GRAPH;

        activeFKeys[3] = FKEY_BLANK;
        activeFKeys[4] = FKEY_BLANK;
        activeFKeys[5] = FKEY_BLANK;
    }
    else
    {
        activeFKeys[0] = FKEY_EDIT;
        activeFKeys[1] = FKEY_CLEAR;
        activeFKeys[2] = FKEY_BLANK;
        activeFKeys[3] = FKEY_BLANK;
        activeFKeys[4] = FKEY_BLANK;
        activeFKeys[5] = FKEY_BLANK;
    }

    int ptr;
    for (int i = 0; i < 6; i++)
    {
        GetFKeyPtr(activeFKeys[i], &ptr);
        FKey_Display(i, (int*)ptr);
    }
}

void SolveTriangle()
{
    if (InitialTriangleValidation() != 1)
        return;
}

int InitialTriangleValidation()
{
    int dataCount = 0;

    if (sideA != 0) dataCount++;
    if (sideB != 0) dataCount++;
    if (sideC != 0) dataCount++;
    if (angleA != 0) dataCount++;
    if (angleB != 0) dataCount++;
    if (angleC != 0) dataCount++;

    if (dataCount != 3)
    {
        DrawPopupBig("--Please only enter", "--three values");
        SetState(1);
        return 0;
    }

    if (angleA >= 179 || angleB >= 179 || angleC >= 179)
    {
        DrawPopup("--Invalid angles");
        SetState(1);
        return 0;
    }

    if (sideA == 0 && sideB == 0 && sideC == 0)
    {
        DrawPopupBig("--Enter at least", "--one side");
        SetState(1);
        return 0;
    }


    // CALCULATIONS //

    //// SIDES ////
    // HYPOTHENUSE //
    /*
    if (sideA == 0 && (sideB != 0 && sideC != 0) && angleA == 90)
        sideA = Sqrt(pow(sideB, 2) + pow(sideC, 2));
    else if (sideB == 0 && (sideA != 0 && sideC != 0) && angleB == 90)
        sideB = Sqrt(pow(sideA, 2) + pow(sideC, 2));
    else if (sideC == 0 && (sideA != 0 && sideB != 0) && angleC == 90)
        sideC = Sqrt(pow(sideA, 2) + pow(sideB, 2));
    */

    DrawPopup("--Done!");
    SetState(1);
    return 1;
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
}

void DrawPopupBig (char* message, char* message2)
{
    MsgBoxPush(4);
    PrintXY(3, 2, message, 0, TEXT_COLOR_BLACK);
    PrintXY(3, 3, message2, 0, TEXT_COLOR_BLACK);
    PrintXY(6, 5, "--Press: [EXIT]", 0, TEXT_COLOR_BLACK);

    int boxInput;
    while (1)
    {
        GetKey (&boxInput);
        if (boxInput == KEY_CTRL_EXE || boxInput == KEY_CTRL_EXIT || boxInput == KEY_CTRL_QUIT || boxInput == KEY_CTRL_MENU)
            break;
    }

    MsgBoxPop();
}

double Sqrt(double x)
{
    double y;
    int p,square,c;

    /* find the surrounding perfect squares */
    p = 0;
    do
    {
        p++;
        square = (p+1) * (p+1);
    }
    while( x > square );

    /* process the root */
    y = (double)p;
    c = 0;
    while(c<10)
    {
        /* divide and average */
        y = (x/y + y)/2;
        /* test for success */
        if( y*y == x)
            return(y);
        c++;
    }
    return(y);
}