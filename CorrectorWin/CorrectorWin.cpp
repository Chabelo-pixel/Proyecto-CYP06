
// CorrectorWin.cpp : define los comportamientos de las clases para la aplicaci�n.
//

#include "stdafx.h"
#include "CorrectorWin.h"
#include "CorrectorWinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCorrectorWinApp

BEGIN_MESSAGE_MAP(CCorrectorWinApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Construcci�n de CCorrectorWinApp

CCorrectorWinApp::CCorrectorWinApp()
{
	// admite Administrador de reinicio
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: agregar aqu� el c�digo de construcci�n,
	// Colocar toda la inicializaci�n importante en InitInstance
}


// El �nico objeto CCorrectorWinApp

CCorrectorWinApp theApp;


// Inicializaci�n de CCorrectorWinApp

BOOL CCorrectorWinApp::InitInstance()
{
	// Windows XP requiere InitCommonControlsEx() si un manifiesto de
	// aplicaci�n especifica el uso de ComCtl32.dll versi�n 6 o posterior para habilitar
	// estilos visuales. De lo contrario, se generar� un error al crear ventanas.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Establecer para incluir todas las clases de control comunes que desee utilizar
	// en la aplicaci�n.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Crear el administrador de shell, en caso de que el cuadro de di�logo contenga
	// alguna vista de �rbol de shell o controles de vista de lista de shell.
	CShellManager *pShellManager = new CShellManager;

	// Inicializaci�n est�ndar
	// Si no utiliza estas caracter�sticas y desea reducir el tama�o
	// del archivo ejecutable final, debe quitar
	// las rutinas de inicializaci�n espec�ficas que no necesite
	// Cambie la clave del Registro en la que se almacena la configuraci�n
	// TODO: debe modificar esta cadena para que contenga informaci�n correcta
	// como el nombre de su compa��a u organizaci�n
	SetRegistryKey(_T("Aplicaciones generadas con el Asistente para aplicaciones local"));

	CCorrectorWinDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: insertar aqu� el c�digo para controlar
		//  cu�ndo se descarta el cuadro de di�logo con Aceptar
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: insertar aqu� el c�digo para controlar
		//  cu�ndo se descarta el cuadro de di�logo con Cancelar
	}

	// Eliminar el administrador de shell creado anteriormente.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Dado que el cuadro de di�logo se ha cerrado, devolver FALSE para salir
	//  de la aplicaci�n en vez de iniciar el suministro de mensajes de dicha aplicaci�n.
	return FALSE;
}

