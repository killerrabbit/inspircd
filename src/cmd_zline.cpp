/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  Inspire is copyright (C) 2002-2005 ChatSpike-Dev.
 *                       E-mail:
 *                <brain.net>
 *                <Craig.net>
 *
 * Written by Craig Edwards, Craig McLure, and others.
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

using namespace std;

#include "inspircd_config.h"
#include "inspircd.h"
#include "inspircd_io.h"
#include <time.h>
#include <string>
#ifdef GCC3
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include <map>
#include <sstream>
#include <vector>
#include <deque>
#include "users.h"
#include "ctables.h"
#include "globals.h"
#include "modules.h"
#include "dynamic.h"
#include "wildcard.h"
#include "message.h"
#include "commands.h"
#include "mode.h"
#include "xline.h"
#include "inspstring.h"
#include "dnsqueue.h"
#include "helperfuncs.h"
#include "hashcomp.h"
#include "socketengine.h"
#include "typedefs.h"
#include "command_parse.h"
#include "cmd_zline.h"

extern ServerConfig* Config;
extern InspIRCd* ServerInstance;
extern int MODCOUNT;
extern std::vector<Module*> modules;
extern std::vector<ircd_module*> factory;
extern time_t TIME;
extern user_hash clientlist;
extern chan_hash chanlist;
extern whowas_hash whowas;
extern std::vector<userrec*> all_opers;
extern std::vector<userrec*> local_users;
extern userrec* fd_ref_table[65536];

void cmd_zline::Handle (char **parameters, int pcnt, userrec *user)
{
	if (pcnt >= 3)
	{
		if (strchr(parameters[0],'@'))
		{
			WriteServ(user->fd,"NOTICE %s :*** You cannot include a username in a zline, a zline must ban only an IP mask",user->nick);
			return;
		}
		if (ip_matches_everyone(parameters[0],user))
			return;
		add_zline(duration(parameters[1]),user->nick,parameters[2],parameters[0]);
		FOREACH_MOD OnAddZLine(duration(parameters[1]), user, parameters[2], parameters[0]);
		if (!duration(parameters[1]))
		{
			WriteOpers("*** %s added permenant Z-line for %s.",user->nick,parameters[0]);
		}
		else
		{
			WriteOpers("*** %s added timed Z-line for %s, expires in %d seconds.",user->nick,parameters[0],duration(parameters[1]));
		}
		apply_lines(APPLY_ZLINES);
	}
	else
	{
		if (del_zline(parameters[0]))
		{
			FOREACH_MOD OnDelZLine(user, parameters[0]);
			WriteOpers("*** %s Removed Z-line on %s.",user->nick,parameters[0]);
		}
		else
		{
			WriteServ(user->fd,"NOTICE %s :*** Z-Line %s not found in list, try /stats Z.",user->nick,parameters[0]);
		}
	}
}


