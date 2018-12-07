#include "resource.h" //다이얼로그가 resource.h에 선언되있어서 이거해야 값읽어와짐
#include "Puzzlecontrol.h"

class PuzzleDlg : public CDialog
{
private:
	Puzzlecontrol control; //뷰클래스에 선언하여 뷰클래스를 선언하고 싶었으나 다이얼로그에서 사용할수없다고 에러가 생겨서 새로운 퍼즐컨트롤이라는 클래스 생성후 선언.
	CString m_image_path; //CString 이미지 경로 선언  다이얼로그의 이미지선택한 파일의 경로를 읽어오기 위한 변수선언.
	int m_is_selected; // 비트맵을 불러오고저장할 변수 선언
	// Construction
public:
	PuzzleDlg(CWnd* pParent = NULL);	// standard constructor

											// Dialog Data
	enum { IDD = IDD_DIALOG2 }; //이부분을 위해 resource.h include함
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
														//}}AFX_VIRTUAL

														// Implementation
protected:
	HICON m_hIcon; // 아이콘 선언

	// Generated message map functions
	//{{AFX_MSG(CExamPuzzleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelectImageBtn();
	afx_msg void OnStart();
	afx_msg void OnApplyBtn();
	afx_msg void OnResetBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
