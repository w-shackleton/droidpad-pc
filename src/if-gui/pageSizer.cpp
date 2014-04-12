#include "pageSizer.hpp"

#include "log.hpp"

PageSizer::PageSizer() :
	childSize(wxDefaultSize)
{
}

wxSizerItem *PageSizer::Insert(size_t index, wxSizerItem *item)
{
	if ( item->IsWindow() )
	{
		// we must pretend that the window is shown as otherwise it wouldn't be
		// taken into account for the layout -- but avoid really showing it, so
		// just set the internal flag instead of calling wxWindow::Show()
		item->GetWindow()->wxWindowBase::Show();
	}

	return wxSizer::Insert(index, item);
}

void PageSizer::HidePages()
{
	for (wxSizerItemList::compatibility_iterator node = GetChildren().GetFirst();
			node;
			node = node->GetNext()) {
		wxSizerItem * const item = node->GetData();
		if ( item->IsWindow() )
			item->GetWindow()->wxWindowBase::Show(false);
	}
}

void PageSizer::RecalcSizes()
{
	for (wxSizerItemList::compatibility_iterator node = GetChildren().GetFirst();
			node;
			node = node->GetNext()) {
		wxWindow *item = node->GetData()->GetWindow();
		item->SetSize(wxRect(10000, 10000, 20, 20)); // It works...
	}

	wxSizerItem *item = GetItem(childPosition);
	if(!item) return;
	wxWindow *current = item->GetWindow();
	if(!current) {
		LOGW("No current page found for given position!");
	}
	int border = item->GetBorder();
	int flags = item->GetFlag();
	wxPoint pos = m_position;
	wxSize size = m_size;
	if(flags & wxLEFT) {
		pos.x += border;
		size.x -= border;
	}
	if(flags & wxRIGHT) {
		size.x -= border;
	}
	if(flags & wxTOP) {
		pos.y += border;
		size.y -= border;
	}
	if(flags & wxBOTTOM) {
		size.x -= border;
	}
	current->SetSize(wxRect(pos, size));
}

void PageSizer::SetCurrent(int position) {
	HidePages(); // ???
	childPosition = position;

	RecalcSizes();

	wxSizerItem *item = GetItem(childPosition);
	if(!item) return;
	wxWindow *current = item->GetWindow();
	if(!current) return;
	current->Update();
}

wxSize PageSizer::CalcMin()
{
	wxSizerItem *item = GetItem(childPosition);
	if(!item) return wxSize(0, 0);
	wxWindow *current = item->GetWindow();
	if(!current) {
		LOGW("No current page found for given position!");
	}
	return current->GetMinSize();
}

wxSize PageSizer::GetMaxChildSize()
{
	wxSize maxOfMin;

	for ( wxSizerItemList::compatibility_iterator childNode = m_children.GetFirst();
			childNode;
			childNode = childNode->GetNext() )
	{
		wxSizerItem *child = childNode->GetData();
		maxOfMin.IncTo(child->CalcMin());
		// maxOfMin.IncTo(SiblingSize(child));
	}

	childSize = maxOfMin;

	return maxOfMin;
}

/*
int PageSizer::GetBorder() const
{
	return m_owner->m_border;
}
*/

wxSize PageSizer::SiblingSize(wxSizerItem *child)
{
	wxSize maxSibling;

	/*
	if ( child->IsWindow() )
	{
		wxWindow *page = wxDynamicCast(child->GetWindow(), wxWindow);
		if ( page )
		{
			for ( wxWindow *sibling = page->GetNext();
					sibling;
					sibling = sibling->GetNext() )
			{
				if ( sibling->GetSizer() )
				{
					maxSibling.IncTo(sibling->GetSizer()->CalcMin());
				}
			}
		}
	}
	*/

	return maxSibling;
}

