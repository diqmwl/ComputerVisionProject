#include "stdafx.h"
#include "WinColor.h"
#include "PuzzleDlg.h"


PuzzleDlg::PuzzleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PuzzleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void PuzzleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(PuzzleDlg, CDialog) //이벤트 만든것들
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECT_IMAGE, OnSelectImageBtn)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_APPLY, OnApplyBtn)
	ON_BN_CLICKED(IDC_RESET, OnResetBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL PuzzleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 큰아이콘
	SetIcon(m_hIcon, FALSE);		// 작은아이콘

	SetDlgItemInt(IDC_X, 3); //X,Y값을 기본 3으로 정한다.
	SetDlgItemInt(IDC_Y, 3);

	
	m_image_path.Empty(); //처음 문자열 비우고 0으로 선언한다.
	m_is_selected = 0;

	CRect r;
	GetDlgItem(IDC_IMAGE)->GetWindowRect(r);
	ScreenToClient(r); //전체 윈도우 좌표가아니라 내가 실행한퍼즐다이얼로그로 영역을 얻는다.

	// 얻은 영역으로 작업 윈도우를 생성한다.
	control.CreateEx(WS_EX_STATICEDGE, NULL, NULL, WS_CHILD | WS_VISIBLE, r, this, 20010);




	return TRUE;
}


void PuzzleDlg::OnPaint() //그리는것
{
	if (IsIconic())
	{
		CPaintDC dc(this); //그리기가 가능하다고 하는것.

		int cxIcon = GetSystemMetrics(SM_CXICON);//화면의 가로길이
		int cyIcon = GetSystemMetrics(SM_CYICON);//화면의 세로길이 반환
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon); //x,y로 아이콘그림<<이거없으면 그림안그려짐
	}
	else
	{
		CDialog::OnPaint();
	}
}


void PuzzleDlg::OnSelectImageBtn()
{
	TCHAR work_path[MAX_PATH] = { 0 };
	// 현재 작업경로를 가져온다.
	GetCurrentDirectory(MAX_PATH, work_path);
	// 파일 선택 다이얼로그에 BMP만 보이게 한다
	static char szFilter[] = "BMP(*.bmp)|""|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, "*.bmp", "*.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);


	if (dlg.DoModal() == IDOK) {
		
		m_image_path = dlg.GetPathName();
		//bmp 파일을 선택한경우에 파일의 이름을 출력한다.
		SetDlgItemText(IDC_IMAGE_PATH_EDIT, dlg.GetFileName());

		SetCurrentDirectory(work_path);

		// 기본 3X3인데 지금 몇X몇인지 확인한다.
		int x_count = GetDlgItemInt(IDC_X);
		int y_count = GetDlgItemInt(IDC_Y);

		OnApplyBtn();
	}
}

void PuzzleDlg::OnStart()
{
	if (!control.IsGameing()) {
		// 이미지가 선택된 경우 이미지를 섞는다.
		if (m_is_selected) control.MixImageData();
		else AfxMessageBox("이미지를 선택해 주세요");
		
	}
	else {
		if (::MessageBox(m_hWnd, "다시 시작하시겠습니까?", "게임이 진행중입니다.", MB_ICONSTOP | MB_YESNO) == IDYES) {
			// 이미지를 섞는다.
			control.MixImageData();
		}
	}
}

void PuzzleDlg::OnApplyBtn()
{
	//입력한 X, Y 값을 저장한다.
	int x_count = GetDlgItemInt(IDC_X);
	int y_count = GetDlgItemInt(IDC_Y);

	// 3보다 작으면 에러
	if ((x_count < 3) || (y_count < 3)) {
		AfxMessageBox("3X3 이상이어야합니다.");
		if (x_count < 3) x_count = 3;
		if (y_count < 3) y_count = 3;
	}
	else if ((x_count > 10) || (y_count > 10)) {
		AfxMessageBox("7X7보다 클 수 없습니다.");
		if (x_count > 3) x_count = 7;
		if (y_count > 3) y_count = 7;
	}
	// 바뀐 숫자를 다시 저장.
	SetDlgItemInt(IDC_X, x_count);
	SetDlgItemInt(IDC_Y, y_count);
	// 이미지 출력 Puzzlecontrol클래스에서 함수 호출
	m_is_selected = control.SetLoadBmp(m_image_path, x_count, y_count);
}

void PuzzleDlg::OnResetBtn()
{
	if (control.IsGameing()) {
		//원래상태로 돌아간다.
		control.InitImageData();
	}
}
