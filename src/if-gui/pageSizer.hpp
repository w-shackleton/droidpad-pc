/*
 * This file is part of DroidPad.
 * DroidPad lets you use an Android mobile to control a joystick or mouse
 * on a Windows or Linux computer.
 *
 * DroidPad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DroidPad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DroidPad, in the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.
 */

// Based off the wxWizard wxWizardSizer source

#ifndef _PAGESIZER_H
#define _PAGESIZER_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>

class PageSizer : public wxSizer
{
	public:
		PageSizer();

		virtual wxSizerItem *Insert(size_t index, wxSizerItem *item);

		virtual void RecalcSizes();
		virtual wxSize CalcMin();

		// get the max size of all wizard pages
		wxSize GetMaxChildSize();

		// return the border which can be either set using wxWizard::SetBorder() or
		// have default value
		// int GetBorder() const;

		// hide the pages which we temporarily "show" when they're added to this
		// sizer (see Insert())
		void HidePages();

		void SetCurrent(int position);

	private:
		wxSize SiblingSize(wxSizerItem *child);

		wxSize childSize;
		int childPosition;
};

#endif
