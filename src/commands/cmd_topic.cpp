/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2007-2008 Robin Burchell <robin+git@viroteck.net>
 *   Copyright (C) 2008 Craig Edwards <craigedwards@brainbox.cc>
 *   Copyright (C) 2008 Thomas Stagner <aquanight@inspircd.org>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "inspircd.h"
#include "commands/cmd_topic.h"


extern "C" DllExport Command* init_command(InspIRCd* Instance)
{
	return new CommandTopic(Instance);
}

CmdResult CommandTopic::Handle (const std::vector<std::string>& parameters, User *user)
{
	Channel* c;

	c = ServerInstance->FindChan(parameters[0]);
	if (!c)
	{
		user->WriteNumeric(401, "%s %s :No such nick/channel",user->nick.c_str(), parameters[0].c_str());
		return CMD_FAILURE;
	}

	if (parameters.size() == 1)
	{
		if (c)
		{
			if ((c->IsModeSet('s')) && (!c->HasUser(user)))
			{
				user->WriteNumeric(401, "%s %s :No such nick/channel",user->nick.c_str(), c->name.c_str());
				return CMD_FAILURE;
			}

			if (c->topic.length())
			{
				user->WriteNumeric(332, "%s %s :%s", user->nick.c_str(), c->name.c_str(), c->topic.c_str());
				user->WriteNumeric(333, "%s %s %s %lu", user->nick.c_str(), c->name.c_str(), c->setby.c_str(), (unsigned long)c->topicset);
			}
			else
			{
				user->WriteNumeric(RPL_NOTOPICSET, "%s %s :No topic is set.", user->nick.c_str(), c->name.c_str());
			}
		}
		return CMD_SUCCESS;
	}
	else if (parameters.size()>1)
	{
		std::string t = parameters[1]; // needed, in case a module wants to change it
		c->SetTopic(user, t);
	}

	return CMD_SUCCESS;
}

