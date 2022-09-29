#pragma once

class CSprite2d
{
public:
	struct RwTexture* m_pRwTexture;
	
	CSprite2d();
	~CSprite2d();

	void Draw(float x, float y, float width, float height, CRGBA& color);
	void Draw(RECT const& rect, CRGBA const& color);
	void DrawRotated(RECT const& rect, float d, CRGBA const& color);

	template <typename Tex>
	void SetTexture(Tex tex) { m_pRwTexture = (RwTexture*)tex; }
	void Delete();

	static void InitPerFrame();
};