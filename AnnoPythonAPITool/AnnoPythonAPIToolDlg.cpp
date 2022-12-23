
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

DWORD pid = 0;
HANDLE hProcess;
std::string dllName;
size_t sz;

//Used to hide code from listbox 
CString wpnl = L"                                                                                                                                                                       \n";


bool Annoinitialized = FALSE;

CWnd* gobtn;
CWnd* statuslabel;
CWnd* customvaluebox;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	ON_BN_CLICKED(IDC_BUTTON2, &CAnnoPythonAPIToolDlg::OnBnClickedButton2)
	ON_LBN_SELCHANGE(IDC_LIST2, &CAnnoPythonAPIToolDlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAnnoPythonAPIToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CAnnoPythonAPIToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CAnnoPythonAPIToolDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CAnnoPythonAPIToolDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CAnnoPythonAPIToolDlg::OnBnClickedButton7)
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

	//ts = TextSources.TextSourceRoots




	gobtn = GetDlgItem(IDC_BUTTON1);
	statuslabel = GetDlgItem(IDC_STATIC);
	customvaluebox = GetDlgItem(IDC_EDIT2);

	customvaluebox->SetWindowTextW(L"190547");

	gobtn->EnableWindow(FALSE);
	statuslabel->SetWindowTextW(L"Anno 1800 NOT initialized.");
	Annoinitialized = FALSE;



	pid = findProcessID();


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	dllName = curDir("AnnoPythonInject.dll");
	sz = strlen(dllName.c_str());

	if (pid != 0) {
		statuslabel->SetWindowTextW(L"Anno 1800 initialized!");
		if (ListBox1.GetCurSel() != -1) {
			gobtn->EnableWindow(TRUE);
		}
		Annoinitialized = TRUE;
	}


	CWnd* cheatbtn = GetDlgItem(IDC_BUTTON3);
	cheatbtn->SendMessage(BM_CLICK, NULL, NULL);

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




void runCommand(std::string cmd)
{
	if (FindWindowA("Anno 1800","Anno 1800") != NULL) {


		if (cmd.find("{CVAL}") != std::string::npos) {
			CString sWindowText;
			customvaluebox->GetWindowText(sWindowText);
			std::string cval = CW2A(sWindowText.GetString());

			cmd.replace(cmd.find("{CVAL}"), sizeof("{CVAL}") - 1, cval);
		}
	
	std::ofstream outfile(curDir("script.lua"));
	outfile << "--" + cmd + "\n" << std::endl;
	outfile.close();

	HANDLE hThread = NULL;

	LPVOID lpBaseAddress = VirtualAllocEx(hProcess, NULL, sz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, lpBaseAddress, dllName.c_str(), sz, NULL);
	HMODULE hModule = GetModuleHandle(L"kernel32.dll");
	LPVOID lpStartAddress = GetProcAddress(hModule, "LoadLibraryA");
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpStartAddress, lpBaseAddress, 0, NULL);

	CloseHandle(hThread);
	VirtualFreeEx(hProcess, lpBaseAddress, sz, MEM_RELEASE);

	}
	else {
		gobtn->EnableWindow(FALSE);
		statuslabel->SetWindowTextW(L"Anno 1800 NOT initialized.");
		Annoinitialized = FALSE;
	}
}

void CAnnoPythonAPIToolDlg::OnBnClickedButton1()
{
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


void CAnnoPythonAPIToolDlg::OnBnClickedButton2()
{
	

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	dllName = curDir("AnnoPythonInject.dll");
	sz = strlen(dllName.c_str());

	if (pid != 0) {
		statuslabel->SetWindowTextW(L"Anno 1800 initialized!");
		if (ListBox1.GetCurSel() != -1) {
			gobtn->EnableWindow(TRUE);
		}
		Annoinitialized = TRUE;
	}
	
}


void CAnnoPythonAPIToolDlg::OnLbnSelchangeList2()
{
	if (ListBox1.GetCurSel() == -1) {
		gobtn->EnableWindow(FALSE);
	}
	else {
		if (Annoinitialized == TRUE) {
			gobtn->EnableWindow(TRUE);
		}
	}
}



void CAnnoPythonAPIToolDlg::OnBnClickedButton3()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Add 100k coins to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(1010017, 100000)"));
	ListBox1.AddString(_T("Substract 100k coins to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(1010017, -100000)"));
	ListBox1.AddString(_T("Add 100k goods to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(100000)"));
	ListBox1.AddString(_T("Substract 100k goods to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(-100000)"));
	ListBox1.AddString(_T("Add {CUSTOM VALUE}<-(GUID) to current selected ship") + wpnl + _T("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\nts.Selection.Object.ItemContainer.SetClearSlot(0)\nts.Selection.Object.ItemContainer.SetCheatItemInSlot({CVAL}, 1)"));
	ListBox1.AddString(_T("Toggle electricity") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleElectricity()"));
	ListBox1.AddString(_T("Toggle super ship speed") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleSuperShipSpeed()"));
	ListBox1.AddString(_T("Add building materials to current selected ship") + wpnl + _T("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\nts.Selection.Object.ItemContainer.SetClearSlot(0)\nts.Selection.Object.ItemContainer.SetClearSlot(1)\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)"));
	ListBox1.AddString(_T("Toggle ignore fertilities") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleIgnoreFertilities()"));
	ListBox1.AddString(_T("Toggle ignore building costs") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleIgnoreBuildingCosts()"));
	ListBox1.AddString(_T("Toggle productivity") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleProductivity()"));
	ListBox1.AddString(_T("Toggle defer expensive ecomony") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleDeferExpensiveEconomy()"));
	ListBox1.AddString(_T("Toggle fluid resident settle behaviour") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleFluidResidentSettleBehaviour()"));
	ListBox1.AddString(_T("Toggle defer expensive quest system") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleDeferExpensiveQuestSystem()"));

}


void CAnnoPythonAPIToolDlg::OnBnClickedButton4()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Set UI scale to 60%") + wpnl + _T("ts.Interface.SetUiScreenScaling(0.6)"));
	ListBox1.AddString(_T("Set UI scale back to 100%") + wpnl + _T("ts.Interface.SetUiScreenScaling(1.0)"));

}


void CAnnoPythonAPIToolDlg::OnBnClickedButton5()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Toggle wind direction") + wpnl + _T("ts.Weather.SetChangeWind()"));



}


void CAnnoPythonAPIToolDlg::OnBnClickedButton6()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Toggle incidents") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleIncidents()"));
	ListBox1.AddString(_T("Toggle incident spreading") + wpnl + _T("ts.Incidents.ToggleSpreading()"));
	ListBox1.AddString(_T("Unlock all incidents") + wpnl + _T("ts.Incidents.CheatUnlockAllIncidents()"));
}


void CAnnoPythonAPIToolDlg::OnBnClickedButton7()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Toggle skip transit") + wpnl + _T("ts.Contracts.ToggleSkipTransit()"));
	ListBox1.AddString(_T("Fill pyramid") + wpnl + _T("ts.Contracts.FillPyramid()"));
}
