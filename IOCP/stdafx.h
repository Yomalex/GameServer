// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
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



// TODO: mencionar aquí los encabezados adicionales que el programa necesita
#include "../PluginBase/Plugin.h"