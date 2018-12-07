#pragma once
#define MAX_SWAP_COUNT   50
#define MOVE_STEP        10

class Puzzlecontrol : public CWnd
{
private:
	CRect m_base_rect;
	CDC *mp_display_dc, *mp_image_dc;
	CBitmap *mp_display_bitmap, *mp_image_bitmap; //메모리 기본정보 변수들.

	char m_x_count, m_y_count;//x칸 y칸

	int m_empty_x, m_empty_y;//비어있는칸

	int m_select_x, m_select_y; //클릭된 칸

	int m_source_x_pos, m_source_y_pos; //원본이미지 출력 위치

	int m_empty_x_pos, m_empty_y_pos;// 빈위치 값

	int m_unit_width, m_unit_height;

	//y값
	POINT **m_image_data; // 퍼즐값저장하기위해 선언 2차원 배열로 선언해도 되는거 같은데...이중포인터로 선언.. 아마 동적할당때문인거 같음.

	char m_is_clicked; //마우스 클릭 드래그 하기위한 변수
	POINT m_click_pos;

	int m_remain_x, m_remain_y;
	char m_timer_id;

	char m_start_game;
	// Construction
public:
	Puzzlecontrol();
	int SetLoadBmp(CString parm_path, int parm_x_count, int parm_y_count);
	void InitImageData();
	void MixImageData();
	void CheckGame();
	void DestroyMemory();
	void AllocData();
	char IsGameing();
	void SwapData(int *parm_data1, int *parm_data2);

	void OnTimer(UINT_PTR nIDEvent);

public:
	virtual ~Puzzlecontrol();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
