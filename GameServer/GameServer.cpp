// GameServer.cpp: define el punto de entrada de la aplicación.
//

#include "stdafx.h"
#include "GameServer.h"
#include <iostream>
#include <string>
#include <regex>

CLoader * Container;

#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE hInst;                                // Instancia actual
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: colocar código aquí.

    // Inicializar cadenas globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMESERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realizar la inicialización de la aplicación:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMESERVER));

	Container = new CLoader();

	std::cmatch cm;
	std::regex e("(.+?)(->)(.+?)(\\()(.+?)(\\))(\\n?)");
	

	FILE* fp;
	fopen_s(&fp, "Main.script", "r");

	if (fp != nullptr) {
		char szLine[1024];
		while (fgets(szLine, sizeof(szLine), fp) != nullptr)
		{
			std::string isComm(szLine);
			if (isComm.find(';') != std::string::npos)
			{
				continue;
			}
			if (std::regex_match(szLine, cm, e))//"Plugin->Function(ArgumentList)"
			{
				if (cm.size() < 6)
				{
					// syntax error
				}
				if (cm[1].compare("Container") == 0)
				{
					if (cm[3].compare("Load") == 0)
					{
						std::string arguments(cm[5].str());
						std::string filename;
						filename.assign(arguments.begin() + 1, arguments.end() - 1);
						Container->Load(filename.c_str());
					}
				}
				else
				{
					CVar Args[10];
					int argCount = 10;
					size_t lastFind = 0, prevFind = 0;

					//parse args
					std::string arguments(cm[5].str());
					std::string argList[10];
					std::cmatch cmArg;
					std::regex eArgs("(?:\\s*)'(.+?)'");
					std::regex eArgs2("(?:\\s*)''");

					for (register int i = 0; i < 10; i++)
					{
						if ((lastFind = arguments.find(',', lastFind+1)) != std::string::npos)
						{
							argList[i].assign(arguments.begin() + prevFind, arguments.begin() + lastFind);
							prevFind = lastFind+1;
						}
						else
						{
							argList[i].assign(arguments.begin() + prevFind, arguments.end());
							argCount = i + 1;
							break;
						}
					}

					std::vector<void*> GlobalPointer;

					for (register int i = 0; i < argCount; i++)
					{
						if (std::regex_match(argList[i].c_str(), cmArg, eArgs))
						{
							char * pLine = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, 1024);
							strcpy_s(pLine, 1024, cmArg[1].str().c_str());
							Args[i] = pLine;
							GlobalPointer.push_back(pLine);
						}
						else if (std::regex_match(argList[i].c_str(), cmArg, eArgs2))
						{
							Args[i] = "";
						}
						else
						{
							Args[i] = (float)atof(argList[i].c_str());
						}
					}

					Container->invoke(cm[1].str().c_str(), cm[3].str().c_str(), Args, argCount);
					for (register unsigned int i = 0; i < GlobalPointer.size(); i++)
					{
						HeapFree(GetProcessHeap(), 0, GlobalPointer[i]);
					}
				}
			}
		}
	}

	fclose(fp);

    MSG msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	Container->Free();

    return (int) msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: registrar la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMESERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMESERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: guardar el identificador de instancia y crear la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Almacenar identificador de instancia en una variable global

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO:  procesar mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizar las selecciones de menú:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Agregar cualquier código de dibujo que use hDC aquí...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
