/**
 * XMPP - libpurple transport
 *
 * Copyright (C) 2009, Jan Kaluza <hanzz@soc.pidgin.im>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef SPECTRUM_BUDDY_H
#define SPECTRUM_BUDDY_H

#include <string>
#include "purple.h"
#include "account.h"
#include "glib.h"
#include "gloox/tag.h"
#include <algorithm>
#include "abstractspectrumbuddy.h"

using namespace gloox;

// Wrapper for PurpleBuddy
class SpectrumBuddy : public AbstractSpectrumBuddy {
	public:
		SpectrumBuddy(long id, PurpleBuddy *buddy);
		virtual ~SpectrumBuddy();

		std::string getAlias();
		std::string getName();
		bool getStatus(PurpleStatusPrimitive &status, std::string &statusMessage);
		bool getXStatus(std::string &mood, std::string &comment);
		std::string getIconHash();
		std::string getGroup();
		std::string getSafeName();
		PurpleBuddy *getBuddy() { return m_buddy; }

	private:
		PurpleBuddy *m_buddy;
};

#endif
