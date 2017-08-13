// stdafx.h: archivo de inclusi�n de los archivos de inclusi�n est�ndar del sistema
// o archivos de inclusi�n espec�ficos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Archivos de encabezado de Windows:
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")



// TODO: mencionar aqu� los encabezados adicionales que el programa necesita
#include "../PluginBase/Plugin.h"