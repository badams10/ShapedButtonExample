#include <wx/wx.h>
#include <wx/bitmap.h>

#define wxNORESIZE_FRAME_STYLE \
            (wxSYSTEM_MENU | \
             0000 | \
             0000 | \
             0000 | \
             wxCLOSE_BOX | \
             wxCAPTION | \
             wxCLIP_CHILDREN)

class MyFrame :public wxFrame {
public:
	MyFrame();
private:
	void OnExit(wxCommandEvent& event) { Close(true); };
};

class wxBitmappedButton : public wxButton {
	wxBitmap m_image_normal;
	wxBitmap m_image_pushed;
	wxRegion m_rgn = wxRegion(m_image_normal, *wxBLACK);
	bool is_pushed = false;
public:
	bool SetShape(wxWindow* victim, wxRegion region);
	wxBitmappedButton(wxWindow* parent, wxWindowID id, wxBitmap image_normal, wxBitmap image_pushed, wxPoint pos);
	DECLARE_EVENT_TABLE()
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& event);
};
BEGIN_EVENT_TABLE(wxBitmappedButton, wxButton)
EVT_LEFT_DOWN(wxBitmappedButton::OnMouseDown)
EVT_LEFT_UP(wxBitmappedButton::OnMouseUp)
EVT_PAINT(wxBitmappedButton::OnPaint)
END_EVENT_TABLE()

void wxBitmappedButton::OnMouseDown(wxMouseEvent& event)
{
	if (m_rgn.Contains(event.GetPosition()) == wxOutRegion)
	{
		event.Skip();
		return;
	}
	is_pushed = true;
	Refresh();
}

void wxBitmappedButton::OnMouseUp(wxMouseEvent& event)
{
	is_pushed = false;
	Refresh();
}

void wxBitmappedButton::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	is_pushed ? dc.DrawBitmap(m_image_pushed, 0, 0, false): dc.DrawBitmap(m_image_normal, 0, 0, false);

}
wxBitmappedButton::wxBitmappedButton(wxWindow* parent, wxWindowID id, wxBitmap image_normal, wxBitmap image_pushed, wxPoint pos) : wxButton(parent, id, wxEmptyString, pos), 
	m_image_normal(image_normal), m_image_pushed(image_pushed)
{
	SetSize(m_image_normal.GetWidth(), m_image_normal.GetHeight());
	SetShape(this, m_rgn);
	//wxMask* myMask = new wxMask(m_image, *wxBLACK);
	//m_image.SetMask(myMask);

}

class MyApp :public wxApp {
	MyFrame* Frame = new MyFrame();
public:
	bool OnInit() {
		wxPanel* blackPanel = new wxPanel(Frame, wxID_ANY, wxDefaultPosition, wxSize(320, 480));
		blackPanel->SetBackgroundColour(*wxBLACK);
		Frame->Show();
		return true;
	}
};
DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "", wxPoint(0, 0), wxSize(200, 200), wxNORESIZE_FRAME_STYLE) {
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap image_normal;
	wxBitmap image_pushed;
	image_normal.LoadFile(wxT("hold80.PNG"), wxBITMAP_TYPE_PNG);
	image_pushed.LoadFile(wxT("hold80red.PNG"), wxBITMAP_TYPE_PNG);

	wxBitmappedButton* m_btn = new wxBitmappedButton(this, wxID_ANY, image_normal, image_pushed, wxPoint(80,70));
}

bool wxBitmappedButton::SetShape(wxWindow* victim, wxRegion region)
{
	DWORD noBytes = ::GetRegionData((HRGN)region.GetHRGN(), 0, NULL);
	RGNDATA* rgnData = (RGNDATA*) new char[noBytes];
	::GetRegionData((HRGN)region.GetHRGN(), noBytes, rgnData);
	HRGN hrgn = ::ExtCreateRegion(NULL, noBytes, rgnData);
	delete[](char*) rgnData;

	RECT rect;
	DWORD dwStyle = ::GetWindowLong((HWND)victim->GetHandle(), GWL_STYLE);
	DWORD dwExStyle = ::GetWindowLong((HWND)victim->GetHandle(), GWL_EXSTYLE);
	::GetClientRect((HWND)victim->GetHandle(), &rect);
	::AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
	::OffsetRgn(hrgn, -rect.left, -rect.top);
	return true;
}