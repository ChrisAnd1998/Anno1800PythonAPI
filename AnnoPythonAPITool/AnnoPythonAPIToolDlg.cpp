
// AnnoPythonAPIToolDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AnnoPythonAPITool.h"
#include "AnnoPythonAPIToolDlg.h"
#include "afxdialogex.h"
#include <string>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#define GetCurrentDir _getcwd

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAnnoPythonAPIToolDlg dialog



CAnnoPythonAPIToolDlg::CAnnoPythonAPIToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANNOPYTHONAPITOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnnoPythonAPIToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, ListBox1);
}

BEGIN_MESSAGE_MAP(CAnnoPythonAPIToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CAnnoPythonAPIToolDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CAnnoPythonAPIToolDlg message handlers

BOOL CAnnoPythonAPIToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	ListBox1.AddString(L"TextSources.TextSourceRoots.Area.Current.Economy.AddAmount(1010017, 100000)");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Area.Current.Economy.AddAmount(100000)");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetClearSlot(0)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(192336, 500)");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleElectricity()");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Weather.SetChangeWind()");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleExtendedDebugging(1)");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleSuperShipSpeed()");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetClearSlot(0)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetClearSlot(1)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleIgnoreFertilities()");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleIgnoreBuildingCosts()");
	ListBox1.AddString(L"TextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleProductivity()");


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAnnoPythonAPIToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAnnoPythonAPIToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAnnoPythonAPIToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD findProcessID()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return(FALSE);
	}

	do {
		if (!wcscmp(pe32.szExeFile, L"Anno1800.exe")) {
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hProcessSnap, &pe32));
	return 0;
}

HMODULE GetRemoteModuleHandle(DWORD lpProcessId, LPCSTR lpModule)
{
	HMODULE hResult = NULL;
	HANDLE hSnapshot;
	MODULEENTRY32 me32;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, lpProcessId);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		me32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &me32))
		{
			do
			{

				std::wstring wide(me32.szModule);
				std::string str(wide.begin(), wide.end());

				if (!_stricmp(str.c_str(), lpModule))
				{
					hResult = me32.hModule;
					break;
				}
			} while (Module32Next(hSnapshot, &me32));
		}
		CloseHandle(hSnapshot);
	}
	return hResult;
}


std::string curDir(std::string file)
{
	char buffer[MAX_PATH];
	GetCurrentDir(buffer, sizeof(buffer));
	std::string::size_type pos = std::string(buffer).find_last_of("\\/") - 1;
	return std::string(buffer) + "\\" + file;
}


void runCommand(std::string cmd)
{
	std::ofstream outfile(curDir("script.lua"));
	outfile << cmd << std::endl;
	outfile.close();

	DWORD pid = 0;
	HANDLE hProcess;
	LPVOID lpBaseAddress;
	std::string dllName = curDir("AnnoPythonInject.dll");
	size_t sz = strlen(dllName.c_str());

	while (pid == 0) {
		pid = findProcessID();
		Sleep(500);
	}

	pid = findProcessID();
	std::cout << pid;
	std::cout << "\n";
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	HMODULE AnnoPythonInjectModule = GetRemoteModuleHandle(pid, "annopythoninject.dll");

	HANDLE hThread = NULL;

	if (AnnoPythonInjectModule == 0000000000000000)
	{
		lpBaseAddress = VirtualAllocEx(hProcess, NULL, sz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(hProcess, lpBaseAddress, dllName.c_str(), sz, NULL);
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		LPVOID lpStartAddress = GetProcAddress(hModule, "LoadLibraryA");
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpStartAddress, lpBaseAddress, 0, NULL);

		CloseHandle(hThread);
		VirtualFreeEx(hProcess, lpBaseAddress, sz, MEM_RELEASE);
	}
}

void CAnnoPythonAPIToolDlg::OnBnClickedButton1()
{
	ListBox1.GetItemData(ListBox1.GetCurSel());

	int nSel = ListBox1.GetCurSel();
	CString ItemSelected;
	if (nSel != LB_ERR)
	{
		ListBox1.GetText(nSel, ItemSelected);
	}

	CString s(ItemSelected);
	CStringA s2(s);
	const char* c = s2;

	runCommand(c);
}
