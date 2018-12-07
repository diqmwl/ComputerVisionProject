
// WinColorDoc.cpp: CWinColorDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "WinColor.h"
#endif

#include "WinColorDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWinColorDoc

IMPLEMENT_DYNCREATE(CWinColorDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinColorDoc, CDocument)
END_MESSAGE_MAP()


// CWinColorDoc 생성/소멸

CWinColorDoc::CWinColorDoc() 
{
	m_InImg = NULL;
	m_OutImg = NULL;
	for (int i = 0; i < 256; i++)
	{
		palRGB[i].rgbBlue = palRGB[i].rgbGreen = palRGB[i].rgbRed = i;
		palRGB[i].rgbReserved = 0;
	}

	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CWinColorDoc::~CWinColorDoc()
{
	if (m_InImg) delete[]m_InImg;
	if (m_OutImg) delete[]m_OutImg;
}

BOOL CWinColorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	::OpenClipboard(NULL);
	if (!IsClipboardFormatAvailable(CF_DIB)) return FALSE;
	HGLOBAL m_hImage = ::GetClipboardData(CF_DIB);
	::CloseClipboard();

	LPSTR pDIB = (LPSTR) ::GlobalLock((HGLOBAL)m_hImage);

	memcpy(&dibHi, pDIB, sizeof(BITMAPINFOHEADER));
	height = dibHi.biHeight; width = dibHi.biWidth;
	int rwsize = WIDTHBYTES(dibHi.biBitCount*width);
	DWORD dwBitsSize =
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256
		+ rwsize * height * sizeof(char);
	m_InImg = new unsigned char[dibHi.biSizeImage];
	m_OutImg = new unsigned char[dibHi.biSizeImage];


	if (dibHi.biBitCount == 8)
	{
		memcpy(palRGB, pDIB + sizeof(BITMAPINFOHEADER), sizeof(RGBQUAD) * 256);
		memcpy(m_InImg, pDIB + dwBitsSize-dibHi.biSizeImage, dibHi.biSizeImage);

	}

	else memcpy(m_InImg, pDIB + sizeof(BITMAPINFOHEADER), dibHi.biSizeImage);


	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.


	dibHf.bfType = 0x4d42;
	dibHf.bfSize = dwBitsSize + sizeof(BITMAPFILEHEADER);
	if (dibHi.biBitCount == 24) dibHf.bfSize -= sizeof(char);
	dibHf.bfReserved1 = dibHf.bfReserved2 = 0;
	return TRUE;


}




// CWinColorDoc serialization

void CWinColorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CWinColorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CWinColorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CWinColorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CWinColorDoc 진단

#ifdef _DEBUG
void CWinColorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinColorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWinColorDoc 명령


BOOL CWinColorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;


	int i, j;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	CFile hFile;
	hFile.Open(lpszPathName, CFile::modeRead | CFile::typeBinary);
	hFile.Read(&dibHf, sizeof(BITMAPFILEHEADER)); 
	if (dibHf.bfType != 0x4D42)
	{
		AfxMessageBox("Not BMP file!!");
		return FALSE;
	}

	//이 파일이 BMP인지 검사 0x4d42=='BM'
	hFile.Read(&dibHi, sizeof(BITMAPINFOHEADER)); //영상정보의 헤더를 읽음
	if (dibHi.biBitCount != 8 && dibHi.biBitCount != 24)
	{
		AfxMessageBox("Gray/True Color Possible!!");
		return FALSE;
	}
	if (dibHi.biBitCount == 8)
		hFile.Read(palRGB, sizeof(RGBQUAD) * 256);

	int ImgSize;
	if (dibHi.biBitCount == 8)
	{
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - 256 * sizeof(RGBQUAD);
	}
	else if (dibHi.biBitCount == 24)
	{
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	}

	m_InImg = new unsigned char[ImgSize];  //dibHi.biSizeImage
	m_OutImg = new unsigned char[ImgSize];
	hFile.Read(m_InImg, ImgSize);
	hFile.Close();
	height = dibHi.biHeight; width = dibHi.biWidth;


	//영상데이터 대입
	int index;
	int rwsize = WIDTHBYTES(dibHi.biBitCount*width);
	for (i = 0; i < height; i++)
	{
		index = i * rwsize;
		for(j=0; j<width; j++)
			m_InImg[index + j] = (unsigned char)palRGB[(int)m_InImg[index + j]].rgbBlue;
	}


	return TRUE;
}


BOOL CWinColorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CFile hFile;
	if (!hFile.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) return FALSE;
	hFile.Write(&dibHf, sizeof(BITMAPFILEHEADER));
	hFile.Write(&dibHi, sizeof(BITMAPFILEHEADER));
	return CDocument::OnSaveDocument(lpszPathName);
}


void CWinColorDoc::CopyClipboard
	(unsigned char *m_CpyImg, int height, int width, int biBitCount)
{
	//
	int rwsize = WIDTHBYTES(biBitCount*width);
	DWORD dwBitsSize = sizeof(BITMAPINFOHEADER)
		+ sizeof(RGBQUAD) * 256 + rwsize * height * sizeof(char);

	HGLOBAL m_hImage =
		(HGLOBAL)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	LPSTR pDIB = (LPSTR) :: GlobalLock((HGLOBAL)m_hImage);

	BITMAPINFOHEADER dibCpyHi;
	memcpy(&dibCpyHi, &dibHi, sizeof(BITMAPINFOHEADER));
	dibCpyHi.biBitCount = biBitCount;
	dibCpyHi.biHeight = height;
	dibCpyHi.biWidth = width;
	dibCpyHi.biSizeImage = height * rwsize;
	if (biBitCount == 8) dibCpyHi.biClrUsed = dibCpyHi.biClrImportant = 256;

	memcpy(pDIB, &dibCpyHi, sizeof(BITMAPINFOHEADER));
	if (biBitCount == 8)
	{
		memcpy(pDIB+sizeof(BITMAPINFOHEADER), palRGB, sizeof(RGBQUAD)*256);
		memcpy(pDIB+dwBitsSize-dibCpyHi.biSizeImage, m_CpyImg,dibCpyHi.biSizeImage);
	}
	else memcpy(pDIB + sizeof(BITMAPINFOHEADER), m_CpyImg, dibCpyHi.biSizeImage);

	::OpenClipboard(NULL);
	::SetClipboardData(CF_DIB, m_hImage);
	::CloseClipboard();

	::GlobalUnlock((HGLOBAL)m_hImage);
	GlobalFree(m_hImage);

	
}
void CWinColorDoc::Chanecolor(unsigned char *InImg, unsigned char *OutImg, int height, int width) {
	CColorDialog ins_dlg;
	ins_dlg.m_cc.Flags |= CC_FULLOPEN;
	if (ins_dlg.DoModal() != IDOK)
		return;
	COLORREF color = ins_dlg.GetColor();

	int rwsize = WIDTHBYTES(dibHi.biBitCount * width);

	int i, j;

	float r, g, b;

	for (i = 0; i<height / 3; i++)
	{
		for (j = 0; j<width; j++)
		{

			OutImg[i * width * 3 + j * 3] = InImg[i * width * 3 + j * 3];
			OutImg[i * width * 3 + j * 3 + 1] = InImg[i * width * 3 + j * 3 + 1];
			OutImg[i * width * 3 + j * 3 + 2] = InImg[i * width * 3 + j * 3 + 2];





			/*else {
			OutImg[index + j] = GetBValue(color);
			OutImg[index + j+1] = GetBValue(color);
			OutImg[index + j+2] = GetBValue(color);
			}*/
		}
	}
	for (i = height / 3; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			OutImg[i * width * 3 + j * 3] = InImg[i * width * 3 + j * 3];
			OutImg[i * width * 3 + j * 3 + 1] = InImg[i * width * 3 + j * 3 + 1];
			OutImg[i * width * 3 + j * 3 + 2] = InImg[i * width * 3 + j * 3 + 2];
			if (OutImg[i * width * 3 + j * 3 + 2] < 70 && OutImg[i * width * 3 + j * 3 + 1] < 70 && OutImg[i * width * 3 + j * 3 + 0] <70)
			{
				OutImg[i * width * 3 + j * 3 + 2] = GetRValue(color);
				OutImg[i * width * 3 + j * 3 + 1] = GetGValue(color);
				OutImg[i * width * 3 + j * 3 + 0] = GetBValue(color);
			}
		}
	}
}