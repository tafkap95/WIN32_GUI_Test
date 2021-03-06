// testOfWin32Dialog.cpp
//

#include "stdafx.h"
#include <commctrl.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>
#include "testOfWin32Dialog.h"

#define MAX_LOADSTRING 100
#define MAX_LISTITEM 5
#define MAX_LISTSUBITEM 5
#define MAX_TABNUM 1

#define DEBUG_GET_DIR_LIST

using namespace std;

string g_path = "C:\\TEST_FILMS";

MSG msg;
HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
static HWND hListTab, hTableList;
//static TCHAR * tabString[MAX_TABNUM] = {_T("SmallIcon"), _T("NormalIcon")};
static TCHAR * tabString[MAX_TABNUM] = {_T("SmallIcon")};

BOOL				InitInstance(HINSTANCE, int);
BOOL CALLBACK	TableProc(HWND, UINT, WPARAM, LPARAM);
LRESULT TableDraw (LPARAM lParam);
void InitListTab(HWND);
void InitTableImageList(HWND);
void InitTableList(HWND);
void InsertTableList(HWND);
void InitTableDlg(HWND);
void GetItemText(HWND, const int &, TCHAR *);
void OnSelchangeListCtrlMode(HWND);
inline BOOL IsChecked(UINT uState);
string get_category(string im_str);
string get_name(string im_dir);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    HACCEL hAccelTable;

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTOFWIN32DIALOG);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
    return 0;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    DialogBox(hInst, (LPCTSTR)IDD_TABLEDLG, NULL, (DLGPROC)TableProc);
    return TRUE;
}

BOOL CALLBACK TableProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int iIndex;
    LPNMLISTVIEW pnm;
    TCHAR *pVarName = NULL;
    POINT pt;
    static RECT lstRect;
    switch(message)
    {
    case WM_INITDIALOG:
    {
        SendMessage(hDlg, WM_SETREDRAW, FALSE, 0);
        hListTab = GetDlgItem(hDlg, IDC_LISTTAB);
        InitListTab(hListTab);
        hTableList = GetDlgItem(hDlg, IDC_TABLELIST);
        //InitTableImageList(hTableList);
        InitTableList(hTableList);
        InitTableDlg(hDlg);
        SetFocus(hTableList);
        SendMessage(hTableList, WM_SETREDRAW, TRUE, 0);
        GetWindowRect(hTableList, &lstRect);
        return TRUE;
    }
    break;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            PostQuitMessage(0);
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    case WM_NCHITTEST:
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        if(pt.x >= lstRect.left && pt.x <= lstRect.right &&
                pt.y >= lstRect.top && pt.y <= lstRect.right)
        {
            return (LRESULT)HTERROR;
        }
        break;

    case WM_NOTIFY:
        switch(LOWORD(wParam))
        {
        case IDC_TABLELIST:
            pnm = (LPNMLISTVIEW)lParam;
            if(pnm->hdr.hwndFrom == hTableList &&pnm->hdr.code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, (LONG)TableDraw(lParam));
                return TRUE;
            }
            if(((LPNMHDR)lParam)->code == NM_CLICK)
            {
                // 1. get current selection
                iIndex = (int)SendMessage(hTableList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
                if(iIndex == -1)
                    return FALSE;
                TCHAR itemTotle[MAX_PATH] = {0};
                GetItemText(hTableList, iIndex, itemTotle);
                return FALSE;
            }
            // here you must use LVN_ITEMCHANGED not LVN_ITEMCHANGING
            // because LVN_ITEMCHANGING is before focu on you clicked item;
            // LVN_ITEMCHANGED is after focu on you clicked item but before disapear it
            if(((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
            {
                iIndex = (int)SendMessage(hTableList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
                if(iIndex == -1)
                    return FALSE;
                ListView_SetItemState(hTableList, iIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
                return TRUE;
            }
            break;
        case IDC_LISTTAB:
            if(((LPNMHDR)lParam)->code == TCN_SELCHANGE)
            {
                OnSelchangeListCtrlMode(hDlg);
                return TRUE;
            }
            break;
        }
        break;
    }
    return FALSE;
}

bool check_directory(string im_dir)
{
    std::string str2 = im_dir.substr (0,2);

#ifdef DEBUG
    cout << "\ncheck_directory: " << str2 << endl;
#endif // DEBUG

    if ( str2 == "@@" )
    {
#ifdef DEBUG
        cout << "\ncheck_directory: " << "TRUE" << endl;
#endif // DEBUG
        return true;
    }
    else
    {
#ifdef DEBUG
        cout << "\ncheck_directory: " << "FALSE" << endl;
#endif // DEBUG
        return false;
    }

};

vector<string> get_directory_list(string im_path)
{
    vector<string> vector_dir;

    const char* t_path = im_path.c_str();
    string str_tmp;

    DIR *dir = opendir(t_path);

    struct dirent *entry = readdir(dir);

    while (entry != NULL)
    {
        str_tmp = entry->d_name;

//        {
        if ( ( str_tmp != "." ) && ( str_tmp != ".." ) )
        {
#ifdef DEBUG
            cout << "Directory to check: " << entry->d_name << endl;
#endif // DEBUG

            if (check_directory(str_tmp) == true)
            {
                int i = str_tmp.size();

                string t_str = str_tmp;
                string t_str2 = t_str.substr(2,i-2);
                vector_dir.push_back(t_str2);
            }
        }

        entry = readdir(dir);
    }

    closedir(dir);


    return vector_dir;
}

void InitListTab(HWND hListTab)
{
    TCITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    for(int i = 0; i < MAX_TABNUM; i++)
    {
        tie.pszText = tabString[i];
        SendMessage(hListTab, TCM_INSERTITEM, 0, (LPARAM)&tie);
    }
}

void InitTableImageList(HWND hTableList)
{
    HIMAGELIST hImageList, hImageNormal;
    hImageNormal = ImageList_Create(32, 32, ILC_COLOR8 | ILC_MASK, (MAX_LISTITEM + 1), 1);
    hImageList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, (MAX_LISTITEM + 1), 1);
    // /*
    // ico
    HICON hIcon;
    for(int i = 0; i <= MAX_LISTITEM; i++)
    {
        hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FACE2 + i));
        ImageList_AddIcon(hImageNormal, hIcon);
        ImageList_AddIcon(hImageList, hIcon);
        DestroyIcon(hIcon);
    }
    // */
    //you can use bitmap too
//	 bitmap
//	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LISTIMAGE));
//	ImageList_Add(hImageList, hBmp, NULL);
//	DeleteObject(hBmp);

    SendMessage(hTableList, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)hImageNormal);
    SendMessage(hTableList, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hImageList);

}

void InsertTableList(HWND hTableList)
{
    vector<string> dir_list = get_directory_list(g_path);

    LVITEM listItem;
    memset(&listItem, 0, sizeof(LVITEM));
    // 1. one
    listItem.mask = LVIF_IMAGE | LVIF_TEXT;
    listItem.cchTextMax = 256;
    TCHAR Temp[MAX_PATH] = {0};

for(int j = 0; j<=MAX_LISTITEM; j++)
	{
		listItem.iItem = j;
		listItem.iSubItem = 0;
		listItem.iImage = j%(MAX_LISTITEM + 1);		// important: sign which image in imageList
		sprintf(Temp, "Knowledge %d", j);
		listItem.pszText = Temp;
		SendMessage(hTableList, LVM_SETITEMSTATE, j, (LPARAM)&listItem);
		SendMessage(hTableList,LVM_INSERTITEM,0,(LPARAM)&listItem); // Send info to the Listview
		memset(Temp, 0, MAX_PATH);
		for(int i=1;i<=MAX_LISTSUBITEM;i++) // Add SubItems in a loop
		{
			listItem.iSubItem=i;
			sprintf(Temp,"SubItem %d",i);
			listItem.pszText=Temp;
			SendMessage(hTableList,LVM_SETITEM,0,(LPARAM)&listItem); // Enter text to SubItems
		}
	}
    for(int i = 0; i <= MAX_LISTITEM; i++)
    {
        ListView_SetItemState(hTableList, i, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

inline BOOL IsChecked(UINT uState)
{
    return (uState ? ((uState & LVIS_STATEIMAGEMASK) >> 12) - 1 : FALSE);
}

void InitTableList(HWND hTableList)
{

    SendMessage(hTableList,LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, 0); // Set style
    LVCOLUMN listCol;
    memset(&listCol, 0, sizeof(LVCOLUMN));
    listCol.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
    listCol.pszText = "Netvisor";
    listCol.cx = 0x80;
    // Inserting Couloms as much as we want
    SendMessage(hTableList,LVM_INSERTCOLUMN,0,(LPARAM)&listCol); // Insert/Show the coloum
    listCol.cx = 0x119;
    listCol.pszText="Sub Item1";                            // Next coloum
//    SendMessage(hTableList,LVM_INSERTCOLUMN,1,(LPARAM)&listCol); // ...
//    listCol.pszText="Sub Item2";                            //
//	SendMessage(hTableList,LVM_INSERTCOLUMN,2,(LPARAM)&listCol); //
//	listCol.pszText="Sub Item3";                            //
//	SendMessage(hTableList,LVM_INSERTCOLUMN,3,(LPARAM)&listCol); //
//	listCol.pszText="Sub Item4";                            //
//	SendMessage(hTableList,LVM_INSERTCOLUMN,4,(LPARAM)&listCol); //
//	listCol.pszText="Sub Item5";                            //
//	SendMessage(hTableList,LVM_INSERTCOLUMN,5,(LPARAM)&listCol); // ...same as above
    InsertTableList(hTableList);
}

void InitTableDlg(HWND hDlg)
{
    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APP));
    SendMessage(hDlg, WM_SETICON, 1, (LPARAM)hIcon);
}

void GetItemText(HWND hList, const int &iSel, TCHAR * text)
{
    TCHAR item[MAX_PATH] = {0};
    LVITEM listItem;
    listItem.mask = LVIF_TEXT;
    listItem.iItem = iSel;
    listItem.pszText = item;
    listItem.cchTextMax = MAX_PATH;
    for(int i = 0; i<= 6; i++)
    {
        listItem.iSubItem = i;
        SendMessage(hList, LVM_GETITEMTEXT, iSel, (LPARAM)&listItem);
        strcat(text, item);
    }
}

LRESULT TableDraw (LPARAM lParam)
{
    int iRow;
    LPNMLVCUSTOMDRAW pListDraw = (LPNMLVCUSTOMDRAW)lParam;
    switch(pListDraw->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
    case CDDS_ITEMPREPAINT:
        iRow = (int)pListDraw->nmcd.dwItemSpec;
        if(iRow%2 == 0)
        {
            //pListDraw->clrText   = RGB(252, 177, 0);
            pListDraw->clrTextBk = RGB(202, 221,250);
            return CDRF_NEWFONT;
        }
    default:
        break;
    }
    return CDRF_DODEFAULT;
}

void OnSelchangeListCtrlMode(HWND hDlg)
{
    LONG lStyle;
    lStyle = GetWindowLong(hTableList, GWL_STYLE);
    lStyle &= ~LVS_TYPEMASK;	// delete the old style
    int iIndex = (int)SendMessage(hListTab, TCM_GETCURSEL, 0, 0);
    switch(iIndex)
    {
    case 0:
        lStyle |= LVS_ICON;		// icon style
        break;
    case 1:
        lStyle |= LVS_REPORT;	// detail style
        break;
    }
    SetWindowLong(hTableList, GWL_STYLE, lStyle);	// set the new style
}

string get_category(string im_str)
{
    string str_cat;
//    bool c_sp = false;

#ifdef DEBUG_CATEGORY
    cout << "STRING: " << im_str << endl;
#endif // DEBUG_CATEGORY

    size_t found = im_str.find("@@");

    str_cat = im_str.substr(0,found);

    return str_cat;
}

string get_name(string im_dir)
{
    string ret;

    size_t m_size = im_dir.size();
    size_t found = im_dir.find("@@");

    ret = im_dir.substr(found+2,m_size-found-2);

    return ret;
}
