#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <stdlib.h>

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

long double sideA = 0; long double sideB = 0; long double sideC = 0;
double angleA = 0; double angleB = 0; double angleC = 0;

void SetState(int new);
void DrawState();
void DrawFKeys();
void SolveTriangle();
int InitialTriangleValidation();
void DrawPopup (char* message);
void DrawPopupBig (char* message, char* message2);

double Sqrt(double x);
double Pow(double x, int t);
double Sine (double x);
double Cosine (double x);
double Tangent (double x);
double Bhaskara (double a, double b, double c);
double ASine (double x);
double ACosine (double x);

char* itoa(int num);
char* dtoa(double num, int precision);
double atod(const char* str);
void reverseString(char* str, int length);

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
                    else if (key == KEY_CTRL_EXE || key == KEY_CTRL_F1 || (key >= 0x0030 && key <= 0x0039) || key == KEY_CTRL_RIGHT)
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
        PrintXY(1, 1, "--Input Values", 0, (sideA == 0 && sideB == 0 && sideC == 0 && angleA == 0 && angleB == 0 && angleC == 0) ? TEXT_COLOR_RED : TEXT_COLOR_BLACK);

        char *value = dtoa(sideA, 4);
        PrintXY(1, 2, "--    A:               ", (selectionCursor + 1) == 2 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 2, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 2 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        value = dtoa(sideB, 4);
        PrintXY(1, 3, "--    B:               ", (selectionCursor + 1) == 3 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 3, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 3 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        value = dtoa(sideC, 4);
        PrintXY(1, 4, "--    C:               ", (selectionCursor + 1) == 4 && !isSelected? 1 : 0, TEXT_COLOR_BLUE);
        PrintXY(7, 4, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 4 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_BLUE);

        value = dtoa(angleA, 4);
        PrintXY(1, 5, "--ALPHA:              ", (selectionCursor + 1) == 5 && !isSelected? 1 : 0, TEXT_COLOR_PURPLE);
        PrintXY(7, 5, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 5 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_PURPLE);

        value = dtoa(angleB, 4);
        PrintXY(1, 6, "-- BETA:              ", (selectionCursor + 1) == 6 && !isSelected? 1 : 0, TEXT_COLOR_PURPLE);
        PrintXY(7, 6, value, TEXT_MODE_TRANSPARENT_BACKGROUND, (selectionCursor + 1) == 6 && !isSelected? TEXT_COLOR_WHITE : TEXT_COLOR_PURPLE);

        value = dtoa(angleC, 4);
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
            else if (key && ((key >= 0x0030 && key <= 0x0039) || key == 0x02E))
            {   
                cursor = EditMBStringChar((unsigned char*)inputBuffer, 128, cursor, key);
                DisplayMBString((unsigned char*)inputBuffer, start, cursor, 7, selectionCursor + 1);
            }
                EditMBStringCtrl((unsigned char*)inputBuffer, 128, &start, &cursor, &key, 7, selectionCursor + 1);
        }

        switch (selectionCursor)
        {
            case 1:
                sideA = atod(inputBuffer);
                break;
            case 2:
                sideB = atod(inputBuffer);
                break;
            case 3:
                sideC = atod(inputBuffer);
                break;
            case 4:
                angleA = atod(inputBuffer);
                break;
            case 5:
                angleB = atod(inputBuffer);
                break;
            case 6:
                angleC = atod(inputBuffer);
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
    for (int iteration = 0; iteration < 5; iteration++)
    {
        if (InitialTriangleValidation() != 1)
            return;

        //// SIDES ////

        /// SIDE A ///
        if (sideA == 0)
        {
            if (angleA == 90) // HYPOTENUSE
            {
                if (sideB != 0)
                {
                    if (sideC != 0) // PYTHAGORAS
                        sideA = Sqrt(sideB*sideB + sideC*sideC);
                    if (angleB != 0) // SINE (B)
                        sideA = sideB / Sine(angleB);
                    if (angleC != 0) // COSINE (C)
                        sideA = sideB / Cosine(angleC);
                }
                if (sideC != 0)
                {
                    if (angleC != 0) // SINE (C)
                        sideA = sideC / Sine(angleC);
                    if (angleB != 0) // COSINE (B)
                        sideA = sideC / Cosine(angleB);
                }
            }
            else
            if (angleB == 90)
            {
                if (sideB != 0)
                {
                    if (sideC != 0) // PYTHAGORAS
                        sideA = Sqrt(sideB*sideB - sideC*sideC);
                    if (angleA != 0) // SINE (A)
                        sideA = Sine(angleA) * sideB;
                    if (angleC != 0) // COSINE (C)
                        sideA = Cosine(angleC) * sideB;
                }
                if (sideC != 0)
                {
                    if (angleA != 0) // TANGENT (A)
                        sideA = Tangent(angleA) * sideC;
                    if (angleC != 0) // TANGENT (C)
                        sideA = sideC / Tangent(angleC);
                }
            }
            else
            if (angleC == 90)
            {
                if (sideC != 0)
                {
                    if (sideB != 0) // PYTHAGORAS
                        sideA = Sqrt(sideC*sideC - sideB*sideB);
                    if (angleA != 0) // SINE (A)
                        sideA = Sine(angleA) * sideC;
                    if (angleB != 0) // COSINE (B)
                        sideA = Cosine(angleB) * sideC;
                }
                if (sideB != 0)
                {
                    if (angleA != 0) // TANGENT (A)
                        sideA = Tangent(angleA) * sideB;
                    if (angleB != 0) // TANGENT (B)
                        sideA = sideB / Tangent(angleB);
                }
            }
            else
            {
                if (angleB != 0 && angleA != 0 && sideB != 0) // SINE THEOREM (A & B)
                    sideA = (sideB / Sine(angleB)) * Sine(angleA);
                if (angleC != 0 && angleA != 0 && sideC != 0) // SINE THEOREM (A & C)
                    sideA = (sideC / Sine(angleC)) * Sine(angleA);
                if (sideB != 0 && sideC != 0) // COSINE THEOREM
                {
                    if (angleA != 0) // COSINE (A)
                        sideA = Sqrt(sideB*sideB + sideC*sideC - 2*sideB*sideC*Cosine(angleA));
                    if (angleB != 0) // COSINE (B)
                        sideA = Bhaskara(1, -2*sideC*Cosine(angleB), sideC*sideC - sideB*sideB);
                    if (angleC != 0) // COSINE (C)
                        sideA = Bhaskara(1, -2*sideB*Cosine(angleC), sideB*sideB - sideC*sideC);
                }
            }
        }

        /// SIDE B ///
        if (sideB == 0)
        {
            if (angleB == 90) // HYPOTENUSE
            {
                if (sideA != 0)
                {
                    if (sideC != 0) // PYTHAGORAS
                        sideB = Sqrt(sideA*sideA + sideC*sideC);
                    if (angleA != 0) // SINE (A)
                        sideB = sideA / Sine(angleA);
                    if (angleC != 0) // COSINE (C)
                        sideB = sideA / Cosine(angleC);
                }
                if (sideC != 0)
                {
                    if (angleC != 0) // SINE (C)
                        sideB = sideC / Sine(angleC);
                    if (angleA != 0) // COSINE (A)
                        sideB = sideC / Cosine(angleA);
                }
            }
            else
            if (angleA == 90)
            {
                if (sideA != 0)
                {
                    if (sideC != 0) // PYTHAGORAS
                        sideB = Sqrt(sideA*sideA - sideC*sideC);
                    if (angleB != 0) // SINE (B)
                        sideB = Sine(angleB) * sideA;
                    if (angleC != 0) // COSINE (C)
                        sideB = Cosine(angleC) * sideA;
                }
                if (sideC != 0)
                {
                    if (angleB != 0) // TANGENT (B)
                        sideB = Tangent(angleB) * sideC;
                    if (angleC != 0) // TANGENT (C)
                        sideB = sideC / Tangent(angleC);
                }
            }
            else
            if (angleC == 90)
            {
                if (sideC != 0)
                {
                    if (sideA != 0) // PYTHAGORAS
                        sideB = Sqrt(sideC*sideC - sideA*sideA);
                    if (angleB != 0) // SINE (B)
                        sideB = Sine(angleB) * sideC;
                    if (angleA != 0) // COSINE (B)
                        sideB = Cosine(angleA) * sideC;
                }
                if (sideA != 0)
                {
                    if (angleB != 0) // TANGENT (B)
                        sideB = Tangent(angleB) * sideA;
                    if (angleA != 0) // TANGENT (A)
                        sideB = sideA / Tangent(angleA);
                }
            }
            else
            {
                if (angleB != 0 && angleA != 0 && sideA != 0) // SINE THEOREM (B & A)
                    sideB = (sideA / Sine(angleA)) * Sine(angleB);
                if (angleC != 0 && angleB != 0 && sideC != 0) // SINE THEOREM (B & C)
                    sideB = (sideC / Sine(angleC)) * Sine(angleB);
                if (sideA != 0 && sideC != 0) // COSINE THEOREM
                {
                    if (angleB != 0) // COSINE (B)
                        sideB = Sqrt(sideA*sideA + sideC*sideC - 2*sideA*sideC*Cosine(angleB));
                    if (angleA != 0) // COSINE (A)
                        sideB = Bhaskara(1, -2*sideC*Cosine(angleA), sideC*sideC - sideA*sideA);
                    if (angleC != 0) // COSINE (C)
                        sideB = Bhaskara(1, -2*sideA*Cosine(angleC), sideA*sideA - sideC*sideC);
                }
            }
        }

        /// SIDE C
        if (sideC == 0)
        {
            if (angleC == 90) // HYPOTENUSE
            {
                if (sideB != 0)
                {
                    if (sideA != 0) // PYTHAGORAS
                        sideC = Sqrt(sideB*sideB + sideA*sideA);
                    if (angleB != 0) // SINE (B)
                        sideC = sideB / Sine(angleB);
                    if (angleA != 0) // COSINE (A)
                        sideC = sideB / Cosine(angleA);
                }
                if (sideA != 0)
                {
                    if (angleA != 0) // SINE (A)
                        sideC = sideA / Sine(angleA);
                    if (angleB != 0) // COSINE (B)
                        sideC = sideA / Cosine(angleB);
                }
            }
            else
            if (angleB == 90)
            {
                if (sideB != 0)
                {
                    if (sideA != 0) // PYTHAGORAS
                        sideC = Sqrt(sideB*sideB - sideA*sideA);
                    if (angleC != 0) // SINE (C)
                        sideC = Sine(angleC) * sideB;
                    if (angleA != 0) // COSINE (A)
                        sideC = Cosine(angleA) * sideB;
                }
                if (sideA != 0)
                {
                    if (angleC != 0) // TANGENT (C)
                        sideC = Tangent(angleC) * sideA;
                    if (angleA != 0) // TANGENT (A)
                        sideC = sideA / Tangent(angleA);
                }
            }
            else
            if (angleA == 90)
            {
                if (sideA != 0)
                {
                    if (sideB != 0) // PYTHAGORAS
                        sideC = Sqrt(sideA*sideA - sideB*sideB);
                    if (angleC != 0) // SINE (C)
                        sideC = Sine(angleC) * sideA;
                    if (angleB != 0) // COSINE (B)
                        sideC = Cosine(angleB) * sideA;
                }
                if (sideB != 0)
                {
                    if (angleC != 0) // TANGENT (C)
                        sideC = Tangent(angleC) * sideB;
                    if (angleB != 0) // TANGENT (B)
                        sideC = sideB / Tangent(angleB);
                }
            }
            else
            {
                if (angleB != 0 && angleC != 0 && sideB != 0) // SINE THEOREM (C & B)
                    sideC = (sideB / Sine(angleB)) * Sine(angleC);
                if (angleA != 0 && angleC != 0 && sideA != 0) // SINE THEOREM (C & A)
                    sideC = (sideA / Sine(angleA)) * Sine(angleC);
                if (sideB != 0 && sideA != 0) // COSINE THEOREM
                {
                    if (angleC != 0) // COSINE (C)
                        sideC = Sqrt(sideB*sideB + sideA*sideA - 2*sideB*sideA*Cosine(angleC));
                    if (angleB != 0) // COSINE (B)
                        sideC = Bhaskara(1, -2*sideA*Cosine(angleB), sideA*sideA - sideB*sideB);
                    if (angleA != 0) // COSINE (A)
                        sideC = Bhaskara(1, -2*sideB*Cosine(angleA), sideB*sideB - sideA*sideA);
                }
            }
        }

        /// ANGLES ///

        // ALPHA //
        if (angleA == 0)
        {
            if (angleB != 0 && angleC != 0)
                angleA = 180 - angleB - angleC;
            
            if (angleB == 90)
            {
                if (sideB != 0 && sideA != 0)
                    angleA = ASine(sideA / sideB);
                if (sideB != 0 && sideC != 0)
                    angleA = ACosine(sideC / sideB);
            }
            else
            if (angleC == 90)
            {
                if (sideC != 0 && sideA != 0)
                    angleA = ASine(sideA / sideC);
                if (sideC != 0 && sideB != 0)
                    angleA = ACosine(sideB / sideC);
            }
            else
            {
                if (sideA != 0) // SINE THEOREM
                {
                    if (sideB != 0 && angleB != 0) // B SIDE AND ANGLE
                        angleA = ASine(sideA * Sine(angleB) / sideB);
                    if (sideC != 0 && angleC != 0) // C SIDE AND ANGLE
                        angleA = ASine(sideA * Sine(angleC) / sideC);
                }

                if (sideA != 0 && sideB != 0 && sideC != 0) // COSINE THEOREM
                    angleA = ACosine((sideB*sideB + sideC*sideC - sideA*sideA) / (2*sideB*sideC));
            }
        }

        // BETA //
        if (angleB == 0)
        {
            if (angleA != 0 && angleC != 0)
                angleB = 180 - angleA - angleC;
            
            if (angleA == 90)
            {
                if (sideA != 0 && sideB != 0)
                    angleB = ASine(sideB / sideA);
                if (sideA != 0 && sideC != 0)
                    angleB = ACosine(sideC / sideA);
            }
            else
            if (angleC == 90)
            {
                if (sideC != 0 && sideB != 0)
                    angleB = ASine(sideB / sideC);
                if (sideC != 0 && sideA != 0)
                    angleB = ACosine(sideA / sideC);
            }
            else
            {
                if (sideB != 0) // SINE THEOREM
                {
                    if (sideA != 0 && angleA != 0) // A SIDE AND ANGLE
                        angleB = ASine(sideB * Sine(angleA) / sideA);
                    if (sideC != 0 && angleC != 0) // C SIDE AND ANGLE
                        angleB = ASine(sideB * Sine(angleC) / sideC);
                }

                if (sideA != 0 && sideB != 0 && sideC != 0) // COSINE THEOREM
                    angleB = ACosine((sideA*sideA + sideC*sideC - sideB*sideB) / (2*sideA*sideC));
            }
        }

        // GAMMA //
        if (angleC == 0)
        {
            if (angleA != 0 && angleB != 0)
                angleC = 180 - angleA - angleB;
            
            if (angleA == 90)
            {
                if (sideA != 0 && sideC != 0)
                    angleC = ASine(sideC / sideA);
                if (sideA != 0 && sideB != 0)
                    angleC = ACosine(sideB / sideA);
            }
            else
            if (angleB == 90)
            {
                if (sideB != 0 && sideC != 0)
                    angleC = ASine(sideC / sideB);
                if (sideB != 0 && sideA != 0)
                    angleC = ACosine(sideA / sideB);
            }
            else
            {
                if (sideC != 0) // SINE THEOREM
                {
                    if (sideA != 0 && angleA != 0) // A SIDE AND ANGLE
                        angleC = ASine(sideC * Sine(angleA) / sideA);
                    if (sideB != 0 && angleB != 0) // B SIDE AND ANGLE
                        angleC = ASine(sideC * Sine(angleB) / sideB);
                }

                if (sideA != 0 && sideB != 0 && sideC != 0) // COSINE THEOREM
                    angleC = ACosine((sideA*sideA + sideB*sideB - sideC*sideC) / (2*sideA*sideB));
            }
        }
    }


    DrawPopup("--Done!");
    SetState(1);
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

    if (dataCount < 3)
    {
        DrawPopupBig("--Please enter at", "--least three values");
        SetState(1);
        return 0;
    }

    if (angleA >= 179 || angleB >= 179 || angleC >= 179 || angleA + angleB + angleC > 180)
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

// CHAT GPT CODE WITH PERSONAL IMPROVEMENTS

#define MAX_DIGITS 20
void reverseString(char* str, int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int num) 
{
    int length = 0;
    int temp = num;
    while (temp != 0) 
    {
        length++;
        temp /= 10;
    }

    if (num == 0) 
        length = 1;
    char* str = (char*)malloc((length + 4) * sizeof(char));

    int index = 0;

    if (num == 0) 
        str[index++] = '0';

    while (num != 0) 
    {
        int digit = num % 10;
        str[index++] = '0' + digit;
        num /= 10;
    }

    str[index++] = '-';
    str[index++] = '-';
    str[index] = '\0';

    reverseString(str, index);
    return str;
}

char* dtoa(double num, int precision)
{
    int integralPart = (int)num;
    double fractionalPart = num - integralPart;

    // Handle negative numbers
    if (num < 0 && fractionalPart != 0) {
        integralPart = -integralPart;
        fractionalPart = -fractionalPart;
    }

    // Convert integral part to string
    char integralStr[MAX_DIGITS];
    int integralLen = 0;
    while (integralPart > 0) {
        integralStr[integralLen++] = (integralPart % 10) + '0';
        integralPart /= 10;
    }
    if (integralLen == 0) {
        integralStr[integralLen++] = '0';
    }
    reverseString(integralStr, integralLen);

    // Convert fractional part to string
    char fractionalStr[MAX_DIGITS];
    int fractionalLen = 0;

    // Calculate scaling factor based on precision
    int scalingFactor = 1;
    for (int i = 0; i < precision; i++) {
        scalingFactor *= 10;
    }

    // Scale the fractional part to an integer
    int scaledFraction = (int)(fractionalPart * scalingFactor);

    // Round up if the next digit is greater than or equal to 5
    int nextDigit = (int)(fractionalPart * scalingFactor * 10) % 10;
    if (nextDigit >= 5) {
        scaledFraction += 1;
    }

    while (precision-- > 0) {
        scaledFraction *= 10;
        int digit = scaledFraction / scalingFactor;
        fractionalStr[fractionalLen++] = digit + '0';
        scaledFraction %= scalingFactor;
    }
    
    // Concatenate the integral and fractional parts
    int totalLen = integralLen + fractionalLen + 1; // +1 for the decimal point
    char* result = (char*)malloc((totalLen + 3) * sizeof(char));
    result[0] = '-', result[1] = '-';

    int index = 2;
    for (int i = 0; i < integralLen; i++) {
        result[index++] = integralStr[i];
    }
    result[index++] = '.';
    for (int i = 0; i < fractionalLen; i++) {
        result[index++] = fractionalStr[i];
    }
    result[index] = '\0';

    return result;
}

double atod(const char* str) 
{
    double result = 0.0;
    int sign = 1;
    int decimalPart = 0;
    double decimalFactor = 1.0;

    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert integral part
    while (*str >= '0' && *str <= '9') {
        result = result * 10.0 + (*str - '0');
        str++;
    }

    // Convert decimal part
    if (*str == '.') {
        str++;

        while (*str >= '0' && *str <= '9') {
            decimalPart = decimalPart * 10 + (*str - '0');
            decimalFactor /= 10.0;
            str++;
        }
    }

    result += decimalPart * decimalFactor;
    result *= sign;

    if (result > 10000.0)
        result = 10000.0;

    return result;
}

// END OF CHATGPT

double Pow (double x, int y)
{
    double temp;
    if (y == 0)
        return 1;
    temp = Pow(x, y / 2);
    if (y % 2 == 0)
        return temp * temp;
    else 
    {
        if (y > 0)
            return x * temp * temp;
        else
            return (temp * temp) / x;
    }
}

double Sqrt(double x)
{
    double y;
    int p, square, c;

    p = 0;
    do
    {
        p++;
        square = (p+1) * (p+1);
    }
    while( x > square );

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

double Sine (double x)
{
    return (-4*x*x + 720*x) / (x*x - 180*x + 40500);
}

double Cosine (double x)
{
    return (x > 90) ? (4*x*x - 1440*x + 97200) / (x*x - 360*x + 64800) : (-4*x*x + 32400) / (x*x + 32400);
}

double Tangent (double x)
{
    return (x != 90) ? Sine(x) / Cosine(x) : 0;
}

double Bhaskara (double a, double b, double c)
{
    return (-b + Sqrt(b*b -4*a*c)) / 2;
}

double ASine (double x)
{
    if (x < -1 || 1 < x)
        return 9999;
    if (x == 1 || x == -1)
        return 90 * x;
    if (x < 0)
    {
        x *= -1;
        return -(90 * (x -2*Sqrt(-x*x -3*x+4) + 4)) / (x + 4);   
    }
    return (90 * (x -2*Sqrt(-x*x -3*x+4) + 4)) / (x + 4);
}

double ACosine (double x)
{
    if (x < -1 || 1 < x)
        return 9999;
    if (x == 1)
        return 0;
    if (x == -1)
        return 180;
    if (x <= 0)
        return (180 * (-x -Sqrt(-x*x + 3*x + 4) + 4)) / (4 - x);
    x *= -1;
    return -((180 * (-x -Sqrt(-x*x + 3*x + 4) + 4)) / (4 - x)) + 180;
}