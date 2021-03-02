#pragma once

using CImagePtr = std::shared_ptr<CImage>;

/*
封装线程安全的 CImage，并记录每个图片所在的区域，比如4态 button 对应的图片为：
+------+------+------+------+
|state1|state2|state3|state4|
+------+------+------+------+
int w = m_spImage->GetWidth() / 4, h = m_spImage->GetHeight();
对应的4个 m_rect 分别为：CRect(0, 0, w, h)、CRect(w, 0, w * 2, h)、CRect(w * 2, 0, w * 3, h)、CRect(w * 3, 0, w * 4, h)
*/

class UILIB_API CImagex
{
public:
	CImagex() = default;
	CImagex(const CImagePtr &spImage);
	CImagex(CImagePtr &&spImage);
	CImagex(const CImagex &_Right) = default;
	CImagex(CImagex &&_Right);
	CImagex &operator=(const CImagePtr &spImage);
	CImagex &operator=(CImagePtr &&spImage);
	CImagex &operator=(const CImagex &_Right);
	CImagex &operator=(CImagex &&_Right);

	bool operator==(const CImagex &_Right) const;
	bool operator!=(const CImagex &_Right) const;
	operator HBITMAP() const;
	const CImage &Image() const;
	const CRect &Rect() const;
	void Swap(CImagex &_Other);
	void Reset(const CImagePtr &spImage, const CRect &rect);
	UINT GetFrameCount() const;
	void SetFrameIndex(UINT nIndex);

	void BitBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, DWORD dwROP = SRCCOPY) const;
	void BitBlt(HDC hdcDst, const CRect &rcDst, const CPoint &ptSrc, DWORD dwROP = SRCCOPY) const;
	void BitBlt(HDC hdcDst, int xDst, int yDst, DWORD dwROP = SRCCOPY) const;
	void BitBlt(HDC hdcDst, const CPoint &ptDst, DWORD dwROP = SRCCOPY) const;
	void Draw(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const;
	void Draw(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc) const;
	void Draw(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight) const;
	void Draw(HDC hdcDst, const CRect &rcDst) const;
	void Draw(HDC hdcDst, int xDst, int yDst) const;
	void Draw(HDC hdcDst, const CPoint &ptDst) const;
	void AlphaBlend(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE nSrcAlpha = 255) const;
	void AlphaBlend(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc, BYTE nSrcAlpha = 255) const;
	void AlphaBlend(HDC hdcDst, int xDst, int yDst, BYTE nSrcAlpha = 255) const;
	void AlphaBlend(HDC hdcDst, const CPoint &ptDst, BYTE nSrcAlpha = 255) const;
	void StretchBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP = SRCCOPY) const;
	void StretchBlt(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc, DWORD dwROP = SRCCOPY) const;
	void StretchBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, DWORD dwROP = SRCCOPY) const;
	void StretchBlt(HDC hdcDst, const CRect &rcDst, DWORD dwROP = SRCCOPY) const;
	void Scale9Draw(HDC hdcDst, const CRect &rcDst, bool bAlpha = true) const;	// 使用九宫格缩放

private:
	void SetRect();

	CImagePtr m_spImage;
	CRect     m_rect;
};
