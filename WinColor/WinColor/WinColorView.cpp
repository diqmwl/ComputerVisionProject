
// WinColorView.cpp: CWinColorView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "WinColor.h"
#endif
#include "PuzzleDlg.h"
#include "WinColorDoc.h"
#include "WinColorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinColorView

IMPLEMENT_DYNCREATE(CWinColorView, CView)

BEGIN_MESSAGE_MAP(CWinColorView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_COPY, &CWinColorView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CWinColorView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CWinColorView::OnUpdateEditPaste)
	ON_COMMAND(IDM_CARTOON, &CWinColorView::OnCartoon)
	ON_COMMAND(ID_32773, &CWinColorView::OnCoding)
	ON_COMMAND(ID_32774, &CWinColorView::OnDecoding)

	ON_COMMAND(ID_32776, &CWinColorView::OnSuktest)
	ON_COMMAND(ID_APP_ABOUT, &CWinColorView::OnAppAbout)
	ON_COMMAND(ID_Puzzle, &CWinColorView::OnPuzzle)
END_MESSAGE_MAP()

// CWinColorView 생성/소멸

CWinColorView::CWinColorView()
{
	BmInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	for (int i = 0; i < 256; i++)
	{
		BmInfo->bmiColors[i].rgbRed
			= BmInfo->bmiColors[i].rgbGreen
			= BmInfo->bmiColors[i].rgbBlue
			= i;
		BmInfo->bmiColors[i].rgbReserved = 0;
	}
	// TODO: 여기에 생성 코드를 추가합니다.

}

CWinColorView::~CWinColorView()
{
	if (BmInfo) delete BmInfo;
}

BOOL CWinColorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CWinColorView 그리기

void CWinColorView::OnDraw(CDC* pDC)
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	height = pDoc->dibHi.biHeight;
	width = pDoc->dibHi.biWidth;
	rwsize = WIDTHBYTES(pDoc->dibHi.biBitCount*pDoc->dibHi.biWidth);
	BmInfo->bmiHeader = pDoc->dibHi;

	SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, width, height,
		0, 0, 0, height, pDoc->m_InImg, BmInfo, DIB_RGB_COLORS);
	SetDIBitsToDevice(pDC->GetSafeHdc(), pDoc->height, 0, width, height, 0, 0, 0, height, pDoc->m_OutImg, BmInfo, DIB_RGB_COLORS);




	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CWinColorView 인쇄

BOOL CWinColorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CWinColorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CWinColorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CWinColorView 진단

#ifdef _DEBUG
void CWinColorView::AssertValid() const
{
	CView::AssertValid();
}

void CWinColorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinColorDoc* CWinColorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinColorDoc)));
	return (CWinColorDoc*)m_pDocument;
}
#endif //_DEBUG


// CWinColorView 메시지 처리기


void CWinColorView::OnEditCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->CopyClipboard(pDoc->m_InImg, height, width, pDoc->dibHi.biBitCount);

}


void CWinColorView::OnEditPaste()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!IsClipboardFormatAvailable(CF_DIB)) return;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);

}


void CWinColorView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable(IsClipboardFormatAvailable(CF_DIB));

}


void CWinColorView::OnCartoon()
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i, j;


	for (i =0 ; i < height; i++)
	{
		for (j = 0; j < width*3+3;j += 3)
		{
			if (pDoc->m_InImg[i * width * 3 + j] % 61 == 0)  //R 
			{

				int temp = pDoc->m_InImg[i * width * 3 + j];

				pDoc->m_InImg[i*width * 3 + j] = (unsigned char)temp;  //R

			}
			else
			{
				int temp = pDoc->m_InImg[i * width * 3 + j] - pDoc->m_InImg[i * width * 3 + j] % 61;

				pDoc->m_InImg[i*width * 3 + j] = (unsigned char)temp;  //R

			}
		}
	}

	for (i = 0; i < height; i++)
	{
		for (j = 1; j < width*3+3; j += 3)
		{
			if (pDoc->m_InImg[i * width * 3 + j+1] % 61 == 0)  //R 
			{

				int temp = pDoc->m_InImg[i * width * 3 + j];

				pDoc->m_InImg[i*width * 3 + j+1] = (unsigned char)temp;  //R

			}
			else
			{
				int temp = pDoc->m_InImg[i * width * 3 + j] - pDoc->m_InImg[i * width * 3 + j ] % 61;

				pDoc->m_InImg[i*width * 3 + j] = (unsigned char)temp;  //R

			}
		}
	}
	for (i = 0; i < height; i++)
	{
		for (j = 2; j < width*3+3; j += 3)
		{
			if (pDoc->m_InImg[i * width * 3 + j ] % 61 == 0)  //R 
			{

				int temp = pDoc->m_InImg[i * width * 3 + j ];

				pDoc->m_InImg[i*width * 3 + j ] = (unsigned char)temp;  //R

			}
			else
			{
				int temp = pDoc->m_InImg[i * width * 3 + j ] - pDoc->m_InImg[i * width * 3 + j ] % 61;

				pDoc->m_InImg[i*width * 3 + j ] = (unsigned char)temp;  //R

			}
		}
	}

	

	Invalidate(FALSE);
}

void CWinColorView::OnCoding()
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i, j;
	int k = 0;


	for (j = 0; j < width*height; j++)
	{

		pDoc->m_OutImg[k] = pDoc->m_InImg[j];
		k++;
		pDoc->m_OutImg[k] = pDoc->m_InImg[height*width + j];
		k++;
		pDoc->m_OutImg[k] = pDoc->m_InImg[height*width * 2 + j];
		k++;

	}
	k = 0;
	for (j = 0; j < width * 3 * height; j++)
	{



		pDoc->m_InImg[j] = pDoc->m_OutImg[k];

		k++;

	}

	Invalidate(FALSE);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CWinColorView::OnDecoding()
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i, j;
	int k = 0;


	for (j = 0; j < width*height; j++)
	{

		pDoc->m_OutImg[j] = pDoc->m_InImg[k];
		k++;
		pDoc->m_OutImg[(height*width) + j] = pDoc->m_InImg[k];
		k++;
		pDoc->m_OutImg[(height*width * 2) + j] = pDoc->m_InImg[k];
		k++;

	}
	k = 0;
	for (j = 0; j < width * 3 * height; j++)
	{
		pDoc->m_InImg[j] = pDoc->m_OutImg[k];
		k++;

	}

	Invalidate(FALSE);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

}


void CWinColorView::OnSuktest()
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i, j;
	int tempR = 0;
	int tempG = 0;
	int tempB = 0;
	int k = 0;
	int l = 0;
	for (k = 0; k < 20; k++) {
		for (l = 0; l < 20; l++) {
			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l* width * 3 / 20; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{
					tempR += pDoc->m_InImg[i * width * 3 + j];
				}
			}

			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l * width * 3 / 20 + 1; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{

					tempG += pDoc->m_InImg[i * width * 3 + j];
				}
			}
			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l* width * 3 / 20 + 2; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{
					tempB += pDoc->m_InImg[i * width * 3 + j];
				}
			}


			tempR = tempR * 400 / (width*height);
			tempG = tempG * 400 / (width*height) ;
			tempB = tempB * 400 / (width*height) ;
			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l * width * 3 / 20; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{
					pDoc->m_InImg[i * width * 3 + j] = tempR;
				}
			}

			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l * width * 3 / 20 + 1; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{

					pDoc->m_InImg[i * width * 3 + j] = tempG;
				}
			}
			for (i = k * height / 20; i < (k + 1)*height / 20; i++)
			{
				for (j = l * width * 3 / 20 + 2; j < (l + 1)*width * 3 / 20 + 3; j += 3)
				{
					pDoc->m_InImg[i * width * 3 + j] = tempB;
				}
			}
			tempR = 0;
			tempB = 0;
			tempG = 0;
		}
	}
	Invalidate(FALSE);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CWinColorView::OnAppAbout()
{
	CWinColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Chanecolor(pDoc->m_InImg, pDoc->m_OutImg, pDoc->dibHi.biWidth, pDoc->dibHi.biHeight);

	Invalidate(FALSE);
}


void CWinColorView::OnPuzzle()
{
	PuzzleDlg dlg;
	dlg.DoModal();
}
