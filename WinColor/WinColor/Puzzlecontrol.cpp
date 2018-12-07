// Puzzlecontrol.cpp : implementation file
//책과 구글링 참고함.

#include "stdafx.h"
#include "PuzzleDlg.h"
#include "Puzzlecontrol.h"


/////////////////////////////////////////////////////////////////////////////
// Puzzlecontrol

Puzzlecontrol::Puzzlecontrol()
{

	m_start_game = 0;// 게임 시작했는지 안했는지 변수 0=시작x 1= 시작
	m_timer_id = 0;	// 퍼즐방향설정 변수
	m_select_x = -1;// 내가 클릭한 퍼즐의 인덱스
	m_select_y = -1;
	m_is_clicked = 0;	// 클릭한 상태를 알려줌
	memset(&m_click_pos, 0, sizeof(POINT));	// 내가 클릭한 위치를 저장


	m_image_data = NULL; 

	m_empty_x = m_empty_y = 0; //초기값들 0으로 선언

	m_empty_x_pos = m_empty_y_pos = 0;
}

Puzzlecontrol::~Puzzlecontrol()
{
}


BEGIN_MESSAGE_MAP(Puzzlecontrol, CWnd)
	//{{AFX_MSG_MAP(ImageWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int Puzzlecontrol::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetClientRect(m_base_rect);	// 다이얼로그 출력 위치 얻음

	mp_image_dc = new CDC();
	mp_image_bitmap = new CBitmap();

	// 출력될 이미지를 만들 메모리 DC
	mp_display_dc = new CDC();
	mp_display_bitmap = new CBitmap();

	CClientDC dc(this); //그림을 그리기위해 dc얻어옴
	mp_image_dc->CreateCompatibleDC(&dc);
	mp_image_bitmap->CreateCompatibleBitmap(&dc, m_base_rect.Width(), m_base_rect.Height());
	// 하나는 전체 나머지는 조각조각
	mp_display_dc->CreateCompatibleDC(&dc);
	mp_display_bitmap->CreateCompatibleBitmap(&dc, m_base_rect.Width(), m_base_rect.Height());

	return 0;
}


void Puzzlecontrol::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBitmap *old_bitmap = mp_display_dc->SelectObject(mp_display_bitmap);
	CBitmap *old_image_bitmap = mp_image_dc->SelectObject(mp_image_bitmap);

	// 처음칸 흰색으로 채워놓는다. 이거 안하면 게임 시작후 9칸중에 1칸이 빈칸이여야하는데 빈칸이 처음에 안보임
	mp_display_dc->FillSolidRect(m_base_rect, RGB(255, 255, 255));

	int x, y;
	for (y = 0; y < m_y_count; y++) {
		for (x = 0; x < m_x_count; x++) {
			//BitBlt 함수로 퍼즐을 한조각씩 출력한다.
			mp_display_dc->BitBlt(x*m_unit_width, y*m_unit_height, m_unit_width, m_unit_height,
				mp_image_dc, m_image_data[y][x].x * m_unit_width, m_image_data[y][x].y * m_unit_height, SRCCOPY);
		}
	}

	// 조각조각난 사진을 dc에 출력한다
	dc.BitBlt(0, 0, m_base_rect.right, m_base_rect.bottom, mp_display_dc, 0, 0, SRCCOPY);

}

int Puzzlecontrol::SetLoadBmp(CString parm_path, int parm_x_count, int parm_y_count)
{
	if (parm_path.IsEmpty()) AfxMessageBox("이미지 파일을 선택해주세요");
	else {
		// 비트맵 파일이 선택되었는지 확인

		if (parm_path.Right(3) != "bmp") AfxMessageBox("비트맵 파일을 선택해주세요");
		else {
			HBITMAP h_load_bmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), parm_path, IMAGE_BITMAP,
				0, 0, LR_LOADFROMFILE); //LoadImage로 선택한 비트맵을 변수에 저장.
			if (h_load_bmp == NULL) AfxMessageBox("이미지 로드에 실패했습니다.");
			else {
				DestroyMemory();

				m_x_count = parm_x_count; //x,y개수 저장
				m_y_count = parm_y_count;

				m_unit_width = m_base_rect.Width() / m_x_count; //한퍼즐 가로 세로 길이 정함 전체/퍼즐개수
				m_unit_height = m_base_rect.Height() / m_y_count;

				BITMAP bmp_info; //로드이미지로 불러온 정보를 비트맵 정보에 저장함 수업때 하나하나 저장하던게 이런식으로 한번에 저장
				GetObject(h_load_bmp, sizeof(BITMAP), &bmp_info);

				CClientDC dc(this);

				CDC temp_mem_dc; //이미지 저장할 변수 선언
				CBitmap temp_bmp;

				// 메모리가 처음에는 1개의 픽셀처럼 작은데 dc를 연결함으로서 내가 만든 퍼즐이 들어갈 곳의 크기만큼 화장된다.
				temp_mem_dc.CreateCompatibleDC(&dc);
				// 비트맵파일을 연결한다.
				temp_bmp.Attach(h_load_bmp);

				CBitmap *p_old_temp_bmp = temp_mem_dc.SelectObject(&temp_bmp);
				CBitmap *p_old_bmp = mp_image_dc->SelectObject(mp_image_bitmap);

				// 전체 흰색으로
				mp_image_dc->FillSolidRect(0, 0, m_base_rect.Width(), m_base_rect.Height(), RGB(255, 255, 255));

				// 윈도우 영역과 비트맵 크기의 차이값을 구한다.
				int x_interval = (m_base_rect.Width() - bmp_info.bmWidth) / 2;
				int y_interval = (m_base_rect.Height() - bmp_info.bmHeight) / 2;

				// 비트맵이 중앙에 오도록 복사하는과정이다.
				mp_image_dc->BitBlt(x_interval, y_interval, bmp_info.bmWidth, bmp_info.bmHeight, &temp_mem_dc, 0, 0, SRCCOPY);

				// 그림사이사이 에 세로줄 가로줄을 그리기위한 작업
				CPen *p_old_pen = (CPen *)mp_image_dc->SelectStockObject(WHITE_PEN);
				CBrush *p_old_brush = (CBrush *)mp_image_dc->SelectStockObject(NULL_BRUSH);

				// 퍼즐의 조각을 실행하는사람이 쉽게 보기위해 사각형을 그린다 이 아래부분이 없어도 출력에는 문제가 없으나 보기 불편함
				for (int y = 0; y < m_y_count; y++) {
					for (int x = 0; x < m_x_count; x++) {
						mp_image_dc->Rectangle(x*m_unit_width, y*m_unit_height, (x + 1)*m_unit_width + 1, (y + 1)*m_unit_height + 1);
					}
				}

				// 위에선언한 색깔의 선을 사용한다는 의미
				mp_image_dc->SelectObject(p_old_brush);
				mp_image_dc->SelectObject(p_old_pen);

				/* 메모리 할당을 취소하는 부분이라는데 없어도 잘 돌아감
				temp_mem_dc.SelectObject(p_old_temp_bmp);
				mp_image_dc->SelectObject(p_old_bmp);

				// 메모리 DC와 비트맵을 제거한다.
				temp_mem_dc.DeleteDC();
				temp_bmp.DeleteObject();*/

				// 이미지 데이터를 할당한 후 값을 초기화한다.
				
				AllocData();//함수호출

				return 1;
			}
		}
	}
	return 0;
}

void Puzzlecontrol::AllocData()
{
	int i;
	// 이미지 저장할 배열 동적할당 이중포인터라 저렇게 선언함.이중포인터 동적할당 방법임
	m_image_data = new POINT *[m_y_count];
	for (i = 0; i < m_y_count; i++) {
		m_image_data[i] = new POINT[m_x_count];
	}

	InitImageData();
}

void Puzzlecontrol::InitImageData()
{
	int x, y;
	for (y = 0; y < m_y_count; y++) {
		for (x = 0; x <m_x_count; x++) {
			m_image_data[y][x].x = x; //초기값 설정.
			m_image_data[y][x].y = y;
		}
	}

	m_empty_x = x-1; //마지막 제일 오른쪽 아래 구석을 빈칸으로 두기위해 선언
	m_empty_y = y-1;

	m_image_data[m_empty_y][m_empty_x].x = -1; //오른쪽 구석아래를 빈칸으로 둔다.
	m_image_data[m_empty_y][m_empty_x].y = -1;

	// 선택된 퍼즐 인덱스를 초기화한다.
	m_select_x = -1;
	m_select_y = -1;

	m_start_game = 0; //게임시작 x

	Invalidate(FALSE);
}

char Puzzlecontrol::IsGameing()
{
	return m_start_game; //게임중인지
}

void Puzzlecontrol::MixImageData()
{
	srand((unsigned)time(NULL)); //srand 하기전에 랜덤값을 얻겠다라고 선언하는것(시드설정이라고함)

	int x_index1, y_index1, x_index2, y_index2, x, y;
	// MAX_SWAP_COUNT 횟수만큼 숫자를 섞는다.
	for (int i = 0; i < MAX_SWAP_COUNT; i++) {
		// 자리를 바꿀 두 위치를 임의로 구한다.
		x_index1 = rand() % m_x_count; //%100하면 100이하 나오듯 내가정한 몇x몇 으로 나눠서 값을 난수로 얻는다
		y_index1 = rand() % m_y_count;
		x_index2 = rand() % m_x_count;
		y_index2 = rand() % m_y_count;

		
		SwapData((int *)&m_image_data[y_index1][x_index1].x, (int *)&m_image_data[y_index2][x_index2].x);
		// m_image_data[y_index1][x_index1], m_image_data[y_index2][x_index2] 값을 교환한다
		SwapData((int *)&m_image_data[y_index1][x_index1].y, (int *)&m_image_data[y_index2][x_index2].y);
	}

	// 빈 칸을 찾는다.
	for (y = 0; y < m_y_count; y++) {
		for (x = 0; x < m_x_count; x++) {
			if ((m_image_data[y][x].x == -1) && (m_image_data[y][x].y == -1)) {
				m_empty_x = x;
				m_empty_y = y; //아까 오른쪽아래 구석이 빈칸이였는데 지금 섞었으니까 다시정해진걸 찾아서 빈칸이라고 등록해주는것
				break;
			}
		}
	}

	// 게임 시작
	m_start_game = 1;
	Invalidate();

}

void Puzzlecontrol::DestroyMemory()
{
	if (m_image_data != NULL) {
		// 동적할당한 2차원 배열을 삭제(없어도 돌아감 근데 메모리 효율성을위해 삭제해준다함)
		for (int i = 0; i < m_y_count; i++) {
			delete[] m_image_data[i];
		}
		delete[] m_image_data;
		m_image_data = NULL;
	}
}


void Puzzlecontrol::OnDestroy()
{
	CWnd::OnDestroy();

	DestroyMemory();

	mp_display_bitmap->DeleteObject();
	mp_display_dc->DeleteDC();

	mp_image_bitmap->DeleteObject();
	mp_image_dc->DeleteDC();

	// 동적할당한 자원들의 메모리를 삭제(위와 마찬가지 없어도 돌아가나 메모리 삭제해줌) 
	delete mp_display_bitmap;
	delete mp_image_bitmap;
	delete mp_display_dc;
	delete mp_image_dc;
}

void Puzzlecontrol::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_is_clicked && m_start_game) {
		// 클릭안하고 게임이 시작되었을경우 이벤트가 실행됨 위에 이미 0으로 해놨음
		GetCapture(); //내가 지금 작동하고있는곳에서만 마우스 실행 확인함.
		// 클릭한 좌표 저장한다.
		m_click_pos = point;
		// 클릭 플래그에 1을 저장한다.
		m_is_clicked = 1;
	}
	else if (!m_start_game) AfxMessageBox("게임 시작을 눌러 주세요");

	CWnd::OnLButtonDown(nFlags, point);
}

void Puzzlecontrol::OnMouseMove(UINT nFlags, CPoint point)
{
	// 클릭한 상태이고, 타이머가 작동하지 않는 경우
	if (m_is_clicked && m_timer_id == 0) {
		// 클릭한 값과 움직인 값의 차를 구한다
		int x_interval = point.x - m_click_pos.x;
		int y_interval = point.y - m_click_pos.y;

		// 선택한 퍼즐 조각의 좌표에서 선택한 몇x몇으로 나누어 인덱스를 찾는다
		m_select_x = (m_click_pos.x / m_unit_width);
		m_select_y = (m_click_pos.y / m_unit_height);

		//복사될 위치를 찾는다.
		m_source_x_pos = m_image_data[m_select_y][m_select_x].x * m_unit_width;
		m_source_y_pos = m_image_data[m_select_y][m_select_x].y * m_unit_height;


		// 움직임의차이가 x가 더큰경우
		if (abs(x_interval) > abs(y_interval)) {
			// 오른쪽의 빈칸으로 그림을 움직인 경우
			if ((point.x > m_click_pos.x) && (m_empty_y == m_select_y) && (m_empty_x == (m_select_x + 1))) {
				m_timer_id = 1;
				// 왼쪽의 빈칸으로 그림을 움직인 경우
			}
			else if ((point.x < m_click_pos.x) && (m_empty_y == m_select_y) && (m_empty_x == (m_select_x - 1))) {
				m_timer_id = 2;
			}
		}
		else if (abs(x_interval) < abs(y_interval)) {//y의 차이가 더큰경우
			// 아래의 빈칸으로 그림을 움직인 경우
			if ((point.y > m_click_pos.y) && (m_empty_x == m_select_x) && (m_empty_y == (m_select_y + 1))) {
				m_timer_id = 3;
				// 위의 빈칸으로 그림을 움직인 경우
			}
			else if ((point.y < m_click_pos.y) && (m_empty_x == m_select_x) && (m_empty_y == (m_select_y - 1))) {
				m_timer_id = 4;
			}
		}

		if (m_timer_id != 0) { //timer값이 0 이 아니면 움직여야한다.
			// 빈칸 영역 시작 좌표
			m_empty_x_pos = m_empty_x * m_unit_width;
			m_empty_y_pos = m_empty_y * m_unit_height;

			m_remain_x = m_unit_width - (m_unit_width % MOVE_STEP); //앞에 선언한 10으로 나눈나머지를 빼서 좌표값정한다
			m_remain_y = m_unit_height - (m_unit_height % MOVE_STEP);

		//온타이머 함수 사용하기위해 필요함
			SetTimer(m_timer_id, 10, NULL);
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void Puzzlecontrol::OnLButtonUp(UINT nFlags, CPoint point)
{
//한번 옮기고나서 계속 클릭값을 0으로 바꿔줘야 다른 퍼즐도 옮겨진다
	if (m_is_clicked && m_start_game) {
		m_is_clicked = 0;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void Puzzlecontrol::SwapData(int *parm_data1, int *parm_data2)
{
	int temp = 0;
	// 두 개의 데이터 값을 교환

	temp = *parm_data1;
	*parm_data1 = *parm_data2;
	*parm_data2 = temp;
}
// 여기서부터 다시
void Puzzlecontrol::OnTimer(UINT_PTR nIDEvent)
{
	// 움직일 이미지가 출력될 위치 초기화
	int move_x_pos = 0, move_y_pos = 0;

	// 오른쪽으로 이동하는 경우
	if (nIDEvent == 1) {
		// 움직일 좌표값 수정
		m_remain_x -= MOVE_STEP;

		// 움직일 이미지가 출력될 위치 지정
		move_x_pos = m_empty_x_pos - m_remain_x;
		move_y_pos = m_empty_y_pos;
		// 왼쪽으로 이동하는 경우
	}
	else if (nIDEvent == 2) {
		// 움직일 좌표값 수정
		m_remain_x -= MOVE_STEP;
		// 움직일 이미지가 출력될 위치 지정
		move_x_pos = m_empty_x_pos + m_remain_x;
		move_y_pos = m_empty_y_pos;
		// 아래로 이동하는 경우
	}
	else if (nIDEvent == 3) {
		// 움직일 좌표값 수정
		m_remain_y -= MOVE_STEP;
		// 움직일 이미지가 출력될 위치 지정
		move_y_pos = m_empty_y_pos - m_remain_y;
		move_x_pos = m_empty_x_pos;
		// 위로 이동하는 경우
	}
	else if (nIDEvent == 4) {
		// 움직일 좌표값 수정
		m_remain_y -= MOVE_STEP;
		// 움직일 이미지가 출력될 위치 지정
		move_y_pos = m_empty_y_pos + m_remain_y;
		move_x_pos = m_empty_x_pos;
	}
	else {
		return;
	}

	// 움직일 영역이 남아있지 않은 경우
	if ((m_remain_x < 0) || (m_remain_y < 0)) {
		// 타이머를 해제한다.
		KillTimer(m_timer_id);
		// 타이머 아이디 초기화
		m_timer_id = 0;

		// 이미지 데이터 값을 바꾼다.
		// x 값 교환
		SwapData((int *)&m_image_data[m_empty_y][m_empty_x].x, (int *)&m_image_data[m_select_y][m_select_x].x);

		// y 값 교환
		SwapData((int *)&m_image_data[m_empty_y][m_empty_x].y, (int *)&m_image_data[m_select_y][m_select_x].y);

		// 클릭한 퍼즐 인덱스와 빈칸 인덱스를 바꾼다.
		// x 인덱스 교환
		SwapData(&m_empty_x, &m_select_x);

		// y 인덱스 교환
		SwapData(&m_empty_y, &m_select_y);

		// 게임이 이겼는지 판단한다.
		CheckGame();
		// 움직일 영역이 남아있는 경우
	}
	else {
		// 이미지 메모리 DC에 비트맵을 연결한다.
		CBitmap *old_image_bitmap = mp_image_dc->SelectObject(mp_image_bitmap);
		CClientDC dc(this);

		// 빈칸과 클릭한 영역을 흰색으로 칠한다.
		dc.FillSolidRect(m_empty_x_pos, m_empty_y_pos, m_unit_width, m_unit_height, RGB(255, 255, 255));
		dc.FillSolidRect(m_select_x*m_unit_width, m_select_y*m_unit_height,
			m_unit_width, m_unit_height, RGB(255, 255, 255));
		// 이미지를 출력한다.
		dc.BitBlt(move_x_pos, move_y_pos, m_unit_width, m_unit_height, mp_image_dc, m_source_x_pos, m_source_y_pos, SRCCOPY);

		// DC와 비트맵의 연결을 해제한다.
		mp_image_dc->SelectObject(old_image_bitmap);
	}

	CWnd::OnTimer(nIDEvent);
}

// 게임이 이겼는지 판단하는 함수
void Puzzlecontrol::CheckGame()
{
	int x, y, is_break = 0;
	// 각 퍼즐 인덱스 값이 원본 이미지가 갖는 값과 같은지 비교한다.
	for (y = 0; y < m_y_count; y++) {
		for (x = 0; x < m_x_count; x++) {
			// 값이 같지 않은 경우
			if ((x != m_image_data[y][x].x) || (y != m_image_data[y][x].y)) {
				// 플래그 변수에 1을 저장한다.
				is_break = 1;
				// 내부 for문을 빠져나간다.
				break;
			}
		}
		// 내부 for문을 break에 의해 빠져나온 경우 외부 for문을 빠져나온다.
		if (is_break) break;
	}
	// 마지막 인덱스 값이 달라서 for문을 빠져나온 경우
	// 게임을 완성한 경우 마지막 인덱스 값에는 -1이 있으므로 인덱스 값이 다르다.
	// 앞의 인덱스 값과 이미지 값이 모두 일치하고, 마지막만 다른 경우 게임에서 이긴 것이다.
	if (x == (m_x_count - 1) && y == (m_y_count - 1)) {
		AfxMessageBox("게임에서 이겼습니다.");
		m_start_game = 0;
	}

}
