
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
#include <thread>


#define GetCurrentDir _getcwd

DWORD pid = 0;
HANDLE hProcess;
std::string dllName;
size_t sz;

//Used to hide code from listbox 
CString wpnl = L"                                                                                                                                                                                                                                                                                         \n";

HHOOK hHook = NULL;

bool Annoinitialized = FALSE;

CWnd* gobtn;
CWnd* statuslabel;
CWnd* customvaluebox;
CWnd* commandbox;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION)
	{
		KBDLLHOOKSTRUCT* pkbhs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		if (pkbhs->vkCode == VK_F8) {
			HWND hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;
			ShowWindow(hWnd, SW_NORMAL);
			SetForegroundWindow(hWnd);
		}
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
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
	DDX_Control(pDX, IDC_COMBO1, ComboBox1);
}

BEGIN_MESSAGE_MAP(CAnnoPythonAPIToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CAnnoPythonAPIToolDlg::OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CAnnoPythonAPIToolDlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAnnoPythonAPIToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CAnnoPythonAPIToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CAnnoPythonAPIToolDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CAnnoPythonAPIToolDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CAnnoPythonAPIToolDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CAnnoPythonAPIToolDlg::OnBnClickedButton8)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAnnoPythonAPIToolDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CAnnoPythonAPIToolDlg message handlers
void annoInit() {
		pid = findProcessID();
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
}

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
	
	

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);

	//ts = TextSources.TextSourceRoots
	dllName = curDir("AnnoPythonInject.dll");
	sz = strlen(dllName.c_str());


	gobtn = GetDlgItem(IDC_BUTTON1);
	//statuslabel = GetDlgItem(IDC_STATIC);
	customvaluebox = GetDlgItem(IDC_EDIT2);
	commandbox = GetDlgItem(IDC_EDIT3);

	customvaluebox->SetWindowTextW(L"190547");

	//gobtn->EnableWindow(FALSE);
	//statuslabel->SetWindowTextW(L"Anno 1800 NOT initialized.");
	//Annoinitialized = FALSE;

	CWnd* cheatbtn = GetDlgItem(IDC_BUTTON3);
	cheatbtn->SendMessage(BM_CLICK, NULL, NULL);





	//ComboBox1.AddString(_T("Diamond") + wpnl + _T("+") + _T("190547"));
	//ComboBox1.AddString(_T("A Bottle of Champagne") + wpnl + _T("+") + _T("192213"));

	
	std::ifstream myfile("ItemList.txt");
	if (myfile.is_open())
	{
		while (myfile.good())
		{
			std::string line;
			getline(myfile, line);
			CStringW wideStr(line.c_str());

			CString sToken = _T("");
			int i = 0;
			CString GUID;
			bool guidset = false;
			while (AfxExtractSubString(sToken, wideStr, i, '@'))
			{
				if (guidset == false) {
					GUID = sToken;
					guidset = true;
				}
				else {
					ComboBox1.AddString(sToken + wpnl + "@" + GUID);
					guidset = false;
				}

				i++;
			}

			
		}
		myfile.close();
	}



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

		annoInit();
		
		if (cmd.find("{CVAL}") != std::string::npos) {
			CString sWindowText;
			customvaluebox->GetWindowText(sWindowText);
			std::string cval = CW2A(sWindowText.GetString());

			cmd.replace(cmd.find("{CVAL}"), sizeof("{CVAL}") - 1, cval);
		}

	
		std::ofstream outfile(curDir("script.lua"));
		outfile << cmd + "\r\n" << std::endl;
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
		MessageBox(NULL, L"Anno 1800 process not found!", L"", MB_OK);
	}
}

void CAnnoPythonAPIToolDlg::OnBnClickedButton1()
{
	CString sWindowText;
	commandbox->GetWindowText(sWindowText);
	std::string cval = CW2A(sWindowText.GetString());

	runCommand(cval);
}


void CAnnoPythonAPIToolDlg::OnLbnSelchangeList2()
{

	int nSel = ListBox1.GetCurSel();
	CString ItemSelected;
	if (nSel != LB_ERR)
	{
		ListBox1.GetText(nSel, ItemSelected);
	}

	ItemSelected.Replace(wpnl, _T("\r\n"));

	ItemSelected = L"--" + ItemSelected;

	commandbox->SetWindowText(ItemSelected);

}



void CAnnoPythonAPIToolDlg::OnBnClickedButton3()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Add 100k coins to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(1010017, 100000)"));
	ListBox1.AddString(_T("Substract 100k coins to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(1010017, -100000)"));
	ListBox1.AddString(_T("Add 100k goods to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(100000)"));
	ListBox1.AddString(_T("Substract 100k goods to current economy") + wpnl + _T("ts.Area.Current.Economy.AddAmount(-100000)"));
	ListBox1.AddString(_T("Add {CUSTOM VALUE}<-(GUID) to current selected ship") + wpnl + _T("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot({CVAL}, 1)"));
	ListBox1.AddString(_T("Toggle electricity") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleElectricity()"));
	ListBox1.AddString(_T("Toggle super ship speed") + wpnl + _T("ts.Cheat.GlobalCheats.ToggleSuperShipSpeed()"));
	ListBox1.AddString(_T("Add building materials to current selected ship") + wpnl + _T("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(1)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)"));
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
	//ListBox1.AddString(_T("Set UI scale to 60%") + wpnl + _T("ts.Interface.SetUiScreenScaling(0.6)"));
	//ListBox1.AddString(_T("Set UI scale back to 100%") + wpnl + _T("ts.Interface.SetUiScreenScaling(1.0)"));
	//ListBox1.AddString(_T("Toggle show console") + wpnl + _T("console.toggleVisibility()"));
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
	//ListBox1.AddString(_T("Infect current selected ship with ...") + wpnl + _T("ts.Selection.Object.ShipIncident.CheatInfect(102669)"));
}


void CAnnoPythonAPIToolDlg::OnBnClickedButton7()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("Toggle skip transit") + wpnl + _T("ts.Contracts.ToggleSkipTransit()"));
	ListBox1.AddString(_T("Fill pyramid") + wpnl + _T("ts.Contracts.FillPyramid()"));
}


void CAnnoPythonAPIToolDlg::OnBnClickedButton8()
{
	ListBox1.ResetContent();
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.ToggleInGameDebugCheatPage()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Unlock.ToggleCheatUnlockAll()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleSuperShipSpeed()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Weather.SetChangeWind()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Achievements.SetUnlockAll()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenScaling(0.5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.SetTriggerCheat(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Weather.SetChangeWind()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Weather.ToggleWindRender()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Weather.SetToggleMirageEffect()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Achievements.SetUnlockAll()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.SetSetCheatCategory(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleExtendedDebugging(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.DebugRender.SetSeedRandomArea({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.DebugRender.SetMainSizeRandomArea({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.DebugRender.SetLocalSizeRandomArea({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.DebugRender.SetLocalInfluenceRandomArea({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.DebugRender.SetChanceRandomArea({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Game.SetDebugDownloadSnapshot(True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Game.SetDebugUploadSnapshot(True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Game.SetPerformanceTimerGuid({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GameSetup.SetDifficultyPreset({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GameSetup.SetDifficultyNet(%d, {0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GameSetup.SetResetPlayerPossession({}, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GameSetup.SetSetPlayerPossession({}, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.HappyDayEventManager.SetUnixTriggerHappyDay('{0}')"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.HappyDayEventManager.SetTimeNotification({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Input.SetDebugModeEnabled(True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.ToggleDebugFlag(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.ResetDebugEmitterFilter()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.SetDebugEmitterFilter({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.ToggleGlobalPersistentEvents()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.ToggleAdvancedDebug()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Sound.ToggleProfiling()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Text.SetValidateAllTexts()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.TextSourceManager.DebugTextSource({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.ToolOneManager.SetDebugVariable({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Unlock.ToggleCheatUnlockAll()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Unlock.SetUnlockNet({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Unlock.SetRelockNet({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetFestivalManager(%u).SetStopFestival()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetFestivalManager(%u).SetTriggerFestival()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetFestivalManager(%u).SetIncreasePool(100)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetFestivalManager(%u).SetIncreasePool(-100)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.Railway.SetRandomSkin({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant(%i).ConstructionAI2.CheatEnterSession(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.Cheats.SetDebugCommand(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.Cheats.SetDebugParticipant(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionParticipants.SetCheatCreateSessionParticipant(10)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiUnit.SetDebugParticipant(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Animals.SetDebugCommand(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Animals.ToggleDebugOption(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Discovery.SetDebugCommand(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Discovery.ToggleDebugOption(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Discovery.HideParticipant(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Discovery.ShowParticipant(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.SetDebugObject({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.SetDebugObjectGUID({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SetEnableObjectGroup({}, {}, 1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SetEnableObjectGroup({}, {}, 0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.SetSelectedAreaID(%u)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.AreaObjects.SaveBuildingCountSnapshot();"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleIncidents()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.MetaIncidents.TogglePause()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.ToggleSpreading()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.ToggleDontSendResolver();"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.ToggleUnlockIncident(%i);"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.CheatUnlockAllIncidents();"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.SetDebugIncidentType({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.SetDebugMode(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Infectable.ResetProtection(%i);"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.SetDebugRenderMode(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.ToggleRenderFlag(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Irrigation.DebugReInitGrids();"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(600005, 1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(600005, 20)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetClearSlot(0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetEquipSlot(0, -1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetUnquipSocket(0, -1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSlot(1010192, 20)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetCheatItemInSocket(6000001, 0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetResetSocketState(0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.LoadingPier.SetDebugDrawing(%hs)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Path.SetDebugCommand(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Path.ToggleDebugOption(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Path.SetDebugLayer(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Season.SetCheatEndCurrentSeasonIn20s()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Season.SetCheatStartSeason({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Season.SetCheatToggleSkipVisualUpdates()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SetEnableDebugDrawPicking(%hs)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionCamera.SetDebugLookAtPosX({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionCamera.SetDebugLookAtPosZ({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Feedback.SetBehaviourFilter(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.SetCurrentParticipant(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionParticipants.SetCheatCreateSessionParticipant({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.StreetOverlay.ToggleEnableStatus({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Visitors.ToggleDebugFlag(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.MetaObjects.CheatLookAtPosition(%i, %.1f, %.1f, %.1f)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.MetaObjects.CheatLookAtObject({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.EconomyStatistic.SetDebugSessionGuid({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.EconomyStatistic.SetDebugAreaGuid({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.EconomyStatistic.SetFocussedItem({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Daytime.SetDaytimeSetting(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Daytime.SetDaytimePreset(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Daytime.SetDaytime({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.GlobalCheats.SetEconomySpeed({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Economy.MetaStorage.SetEconomyModifier(%d, {0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.SetRemoveParticipant(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.CheatCreateParticipant_IfNecessary({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.DebugQuestGUID({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.ResetTutorialQuestsNet()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.ResetScenario()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.ResetLastSeenPromotion()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.EcoSystem.SetFeaturePaused({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.SetMedalReached({0}, 0, {1})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.SetMedalReached({0}, 1, {1})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.SetMedalReached({0}, 2, {1})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Scenarios.SaveAccount()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SkipLoadingTime()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.ToggleSkipTransit()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.GlobalCheats.ToggleSuperShipSpeed()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.FillPyramid()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.DebugSelectGoodForMoreInfo({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.DebugSelectGoodForMoreInfo"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.DebugParticipant(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.ClearIslandStorage()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, 1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, 10)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, 50)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, -1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, -10)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, -50)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.SetSelectStorageGoodDebug({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Incidents.GetIncident(%i).CheatSpread();"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant(%i).ConstructionAI2.MetaShipHandler.ReleaseShip(%d, %d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant(%i).ConstructionAI2.MetaShipHandler.ToggleShipLog(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant(%i).ConstructionAI2.MetaShipHandler.FulfillRequest(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant(%i).ConstructionAI2.MetaShipHandler.SkipRequest(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().OverwriteNextConstructionRequest(%i, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.GetAreaFromID(%d).SetSellShare(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.GetAreaFromID(%d).SetBuyShare(%d, %d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.GetAreaFromID(%d).SetResetCooldown(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.GetAreaFromID(%d).SetHostileTakeover(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.GetAreaFromID(%d).SetOpenMilitaryResultScreen()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.DelayedConstruction.SetCheatReady()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SkipTransit({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.RemoveContract({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportGoodGUID({}, {}, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportAmount({}, {}, 0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportAmount({}, {}, 10)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportAmount({}, {}, 20)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportAmount({}, {}, 50)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetExportAmount({}, {}, 100)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetTraderGUID({}, {}, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.SetImportGoodGUID({}, {}, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.IncreaseGoodXP({}, 100)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.IncreaseGoodXP({}, 1000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.IncreaseGoodXP({}, 2000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Contracts.IncreaseGoodXP({}, 5000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Visitors.GetArea(%i).EndInterval()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Visitors.SetResetArea(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Visitors.GetArea(%i).SpawnSpecialist()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetDiscardExpedition()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetCheatEventCountdown()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetCheatEndExpedition()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionCamera.CheatToGameObject(%llu)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetEndExpedition()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetResolveDecision(%i, %i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetResolveDecision(0, 0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.GetByGUID(%i).SetCheatCurrentEvent(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Unlock.SetUnlock(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject({0}).RandomMapObject.SetFiniteResourceAmount({1})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.RecipeBuilding.ChangeRecipe({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject({}).River.SetMoveRiverLevel(4, 30000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject({}).River.SetMoveRiverLevel(-4, 30000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject({}).River.SetMoveRiverLevel(0, 30000)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).Sellable.CheatBuy()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatInfect(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatEndInfection(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatInfectRandom()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatEndAnyInfection()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatIgnoreIncidentPriority(%hs)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.ToggleDebugRenderRadius()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).Trader.ClearSellCooldown(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).Trader.ForceBuild()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.GetQuest({}).SetActiveNet()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.GetQuest({}).SetReachedNet()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.GetQuest({}).SetAbortedNet(True, {})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.GetQuest({}).ToggleForceQuestTrackerVisibility()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.GetQuest({}).ToggleQuestConditionTreeVisibility()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.CheatResetQuestNet({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.MetaObjects.GetObject(%d).BuyShares.SetCalcPrio()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.SetChangeParticipantReputationTo(%i, %i, -5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.SetChangeParticipantReputationTo(%i, %i,  5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetDistributeExpedition(5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.CheatSetCurrentDecision({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.AddExpedition(%d, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthMinLayers({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthMaxLayers({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthSizeX({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthSizeY({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthMaxRoomSize({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthMinRoomAmount({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinthMaxRoomAmount({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetClearLabyrinth()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.AddLabyrinthRoom()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetRemoveLabyrinthRoom()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.AddLabyrinthConnection()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Expedition.SetGenerateLabyrinth()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Participants.GetParticipant({}).Profile"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.EnableQuestPoolForCurrentPlayer({}, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.CheatEndQuestBlockingNet(%d, %d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.CheatEndPoolCooldownNet(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.CheatEndQuestTimerNet(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Quests.CheatEndQuestTimerNet({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetTransferCraftedItems()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetSkipCraftingTimeCheat()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetResearchCenterWorkforceAmount({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetRemoveQueuedEntry({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetAddToDonationStorage({},{})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetDonateItemsInStorage()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetClearDonationStorage()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetSelectRecipes({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetResearchRecipes({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetCraftOrAddToQueue({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Research.SetCheatAllRecipes({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject(%llu).SetChangeSkin(%u, False)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GetGameObject({}).SetCycleSkins(1, False)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.LoadingPier.UpdateHarborQueuePointsDebug(%i, %llu)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.FreeArea.SetCheatReady()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).FreeArea.SetToggleGridDebugDraw()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.Attractivity.SetCheatChangeAttractivityNet(True, False)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.Attractivity.SetCheatChangeAttractivityNet(False, False)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.Attractivity.SetCheatChangeAttractivityNet(True, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AreaManager.Attractivity.SetCheatChangeAttractivityNet(False, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).IncidentResolver.ClearSpecialActionCooldown()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject({}).Mesh.SetVisible({})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.AICheats.SetDebugBuildMode(%i); TextSources.TextSourceRoots.Cheat.AICheats.ToggleDebugOption(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.AICheats.ToggleDebugOption(%i)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiUnit.SetDebugCommand(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiUnit.ToggleDebugOption(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Monument.SetPauseUpgrade()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Monument.SetCheatUpgradeMicro()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Monument.SetCheatMicroPhase()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Monument.SetCheatSkipEventTime()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.PalaceMinistry.SetMinistryType({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.PalaceMinistry.SetDecreeTier({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Objects.GetObject(%llu).Pirate.DebugIncreaseAreaOfActivityRange(5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.AICheats.SetDebugBuildMode(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.AICheats.ToggleDebugOption(%i)%hs"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.MetaObjects.CheatLookAtPosition(%i, %f, %f, %f)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Cheat.AICheats.ToggleBenchmarkMode()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().ResetQuickBuildFilter()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().SetQuickBuildFilter({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().ResetBuildUntilFilter()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().SetBuildUntilFilter({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().ResetFakeNoSpaceFilter()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().SetFakeNoSpaceFilter({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.GetCheats().%hs(%i, True)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.AiConstruction.Cheats.ToggleIslandRole(%d, %d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionCamera.ToWorldPos(%.2f, %.2f)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.SessionCamera.ToWorldPos(%.2f, %.2f, %.2f)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenScaling({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenHorizontalSpacing({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenVerticalSpacing({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenHorizontalPosition({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.SetUiScreenVerticalPosition({0})"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.ResetUiScreenConfig()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Area.Current.Economy.AddAmount({}, 1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ShipIncident.CheatInfect(%d)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.ItemContainer.SetEquipSlot(0, -1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Game.SetSaveGameWithPopup()"));
	ListBox1.AddString(_T("\r\ngame.takeScreenshot()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Pause.DecreaseGameSpeed()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Pause.IncreaseGameSpeed()"));
	ListBox1.AddString(_T("\r\ncamera.Up()"));
	ListBox1.AddString(_T("\r\ncamera.Down()"));
	ListBox1.AddString(_T("\r\ncamera.Left()"));
	ListBox1.AddString(_T("\r\ncamera.Right()"));
	ListBox1.AddString(_T("\r\ncamera.RotateLeft()"));
	ListBox1.AddString(_T("\r\ncamera.RotateRight()"));
	ListBox1.AddString(_T("\r\ncamera.ZoomIn()"));
	ListBox1.AddString(_T("\r\ncamera.ZoomOut()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.ToggleBuildmenu()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.ToggleShipList()"));
	ListBox1.AddString(_T("\r\ngame.toggleBlueprintMode()"));
	ListBox1.AddString(_T("\r\ngame.startMouseMode(2001042)"));
	ListBox1.AddString(_T("\r\ngame.startMouseMode(2001043)"));
	ListBox1.AddString(_T("\r\ngame.startMouseMode(2001044)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.GUI.SetTogglePause()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.StreetOverlay.ToggleInfoLayerVisibility()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.ToggleUI()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.DestructSelected()"));
	ListBox1.AddString(_T("\r\ngame.startBuild(1000178)"));
	ListBox1.AddString(_T("\r\ngame.startBuild(1010035)"));
	ListBox1.AddString(_T("\r\ngame.startBuild(1010343)"));
	ListBox1.AddString(_T("\r\ngame.startBuild(1010371)"));
	ListBox1.AddString(_T("\r\ngame.changeRotation(1)"));
	ListBox1.AddString(_T("\r\ngame.changeRotation(-1)"));
	ListBox1.AddString(_T("\r\ngame.changeVariation(1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectIslandKontor()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.UpgradeSelected(0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.UpgradeSelected(1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.Object.Pausable.TogglePause()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.TogglePauseForBuildingsOfSelectedType()"));
	ListBox1.AddString(_T("\r\nsession.selectNextObject(True)"));
	ListBox1.AddString(_T("\r\nsession.selectNextObject(False)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.ToggleMetaUI()"));
	ListBox1.AddString(_T("\r\nsession.jumpToSelection()"));
	ListBox1.AddString(_T("\r\nsession.togglePostcardView()"));
	ListBox1.AddString(_T("\r\nsession.deactivateSpecialCameraView()"));
	ListBox1.AddString(_T("\r\ngame.enableResidentView()"));
	ListBox1.AddString(_T("\r\ngame.rotateCameraAroundLookAt(10)"));
	ListBox1.AddString(_T("\r\nsession.setDefaultRotation()"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraView(0)"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(0)"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraView(1);"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(1)"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraView(2);"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(2)"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraView(3);"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(3)"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraViewAndRotate(4,10)"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(4)"));
	ListBox1.AddString(_T("\r\ngame.restoreCameraViewAndRotate(5,10)"));
	ListBox1.AddString(_T("\r\ngame.storeCameraView(5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(1)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(2)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(2)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(3)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(3)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(4)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(4)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(5)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(6)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(6)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(7)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(7)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(8)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(8)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(9)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(9)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetRestore(0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Selection.SelectionGroup.SetStore(0)"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Interface.PopUI()"));
	ListBox1.AddString(_T("\r\nTextSources.TextSourceRoots.Options.SetToggleFullscreen()"));
}


void CAnnoPythonAPIToolDlg::OnCbnSelchangeCombo1()
{
	CString sInput;
	ComboBox1.GetWindowText(sInput);
	//std::string guid = CW2A(sWindowText.GetString());
	CString sToken = _T("");
	int i = 0; // substring index to extract
	while (AfxExtractSubString(sToken, sInput, i, '@'))
	{
		customvaluebox->SetWindowText(sToken);
		i++;
	}
}
