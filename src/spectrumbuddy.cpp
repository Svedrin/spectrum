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

#include "spectrumbuddy.h"
#include "main.h"
#include "user.h"
#include "log.h"
#include "sql.h"
#include "usermanager.h"

#include "transport.h"

SpectrumBuddy::SpectrumBuddy(long id, PurpleBuddy *buddy) : AbstractSpectrumBuddy(id), m_buddy(buddy) {
}

SpectrumBuddy::~SpectrumBuddy() {
}


std::string SpectrumBuddy::getAlias() {
	std::string alias;
	if (purple_buddy_get_server_alias(m_buddy))
		alias = (std::string) purple_buddy_get_server_alias(m_buddy);
	else
		alias = (std::string) purple_buddy_get_alias(m_buddy);
	return alias;
}

std::string SpectrumBuddy::getName() {
	std::string name(purple_buddy_get_name(m_buddy));
	if (name.empty()) {
		Log("SpectrumBuddy::getName", "Name is EMPTY!");
	}
	return name;
}

bool SpectrumBuddy::getStatus(PurpleStatusPrimitive &status, std::string &statusMessage) {
	PurplePresence *pres = purple_buddy_get_presence(m_buddy);
	if (pres == NULL)
		return false;
	PurpleStatus *stat = purple_presence_get_active_status(pres);
	if (stat == NULL)
		return false;
	status = purple_status_type_get_primitive(purple_status_get_type(stat));
	const char *message = purple_status_get_attr_string(stat, "message");

	if (message != NULL) {
		char *stripped = purple_markup_strip_html(message);
		statusMessage = std::string(stripped);
		g_free(stripped);
	}
	else
		statusMessage = "";
	return true;
}

bool SpectrumBuddy::getXStatus(std::string &mood, std::string &comment) {
#if defined(PURPLE_MOOD_NAME) && defined(PURPLE_MOOD_COMMENT)
	PurplePresence *pres = purple_buddy_get_presence(m_buddy);
	if (pres == NULL)
		return false;

	if (purple_presence_is_status_primitive_active(pres, PURPLE_STATUS_MOOD)) {
		PurpleStatus *stat = purple_presence_get_status(pres, "mood");
		const char *m = purple_status_get_attr_string(stat, PURPLE_MOOD_NAME);
		const char *c = purple_status_get_attr_string(stat, PURPLE_MOOD_COMMENT);
		mood = m ? m : "";
		comment = c ? c : "";
		return true;
	}
#endif
	return false;
}

std::string SpectrumBuddy::getIconHash() {
	char *avatarHash = NULL;
	PurpleBuddyIcon *icon = purple_buddy_icons_find(purple_buddy_get_account(m_buddy), purple_buddy_get_name(m_buddy));
	if (icon) {
		avatarHash = purple_buddy_icon_get_full_path(icon);
		Log(getName(), "avatarHash");
	}

	if (avatarHash) {
		Log(getName(), "Got avatar hash");
		// Check if it's patched libpurple which saves icons to directories
		char *hash = strrchr(avatarHash,'/');
		std::string h;
		if (hash) {
			char *dot;
			hash++;
			dot = strchr(hash, '.');
			if (dot)
				*dot = '\0';

			std::string ret(hash);
			g_free(avatarHash);
			return ret;
		}
		else {
			std::string ret(avatarHash);
			g_free(avatarHash);
			return ret;
		}
	}

	return "";
}

std::string SpectrumBuddy::getGroup() {
	return purple_group_get_name(purple_buddy_get_group(m_buddy)) ? std::string(purple_group_get_name(purple_buddy_get_group(m_buddy))) : std::string("Buddies");
}

std::string SpectrumBuddy::getSafeName() {
	std::string name = getName();
	Transport::instance()->protocol()->prepareUsername(name, purple_buddy_get_account(m_buddy));
	if (getFlags() & SPECTRUM_BUDDY_JID_ESCAPING) {
		name = JID::escapeNode(name);
	}
	else {
		if (name.find_last_of("@") != std::string::npos) {
			name.replace(name.find_last_of("@"), 1, "%");
		}
	}
	if (name.empty()) {
		Log("SpectrumBuddy::getSafeName", "Name is EMPTY! Previous was " << getName() << ".");
	}
	return name;
}

