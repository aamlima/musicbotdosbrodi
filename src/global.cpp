#include "global.h"
#include <QtCore\qsettings.h>
#include "langs.h"

namespace Global
{

	TS3Functions ts3Functions;

	MusicBot musicbot;

	Localization languages[2];
	int curLanguage = LANG_EN_US;

	std::string getConfigFilePath(){
		char* configPath = (char*)malloc(512);
		ts3Functions.getConfigPath(configPath, 512);
		std::string path = configPath;
		free(configPath);
		path.append("MusicBotdosBrodi.ini");
		return path;
	}

	void loadSettings(){

		QSettings cfg(QString::fromStdString(getConfigFilePath()), QSettings::IniFormat);
		
		curLanguage = cfg.value("lang", LANG_EN_US).toInt();
		
		musicbot.setVlcPath(cfg.value("vlcPath", "\"C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe\" --extraintf rc --rc-host 127.0.0.1:32323").toString().toStdString());
		std::string host = cfg.value("rcHost", "127.0.0.1").toString().toStdString();
		int port = cfg.value("rcPort", 32323).toInt();
		musicbot.setHostPort(host, port);
		
		musicbot.setVote(cfg.value("voteEnabled", false).toBool());
		musicbot.setPVoteNeeded(cfg.value("pVoteNeeded", 0.5f).toFloat());

		bool cmds[COMMANDS_QTD];
		cfg.beginReadArray("commandsEnabled");
		for (int j = 0; j < COMMANDS_QTD; j++){
			cfg.setArrayIndex(j);
			cmds[j] = cfg.value("cmd", true).toBool();
		}
		cfg.endArray();
		musicbot.setCommandsEnabled(cmds);

		if (musicbot.isConnected()){
			std::string channel = cfg.value("channelName", "Default Channel").toString().toStdString();
			uint64 schID = musicbot.getSchID();
			uint64* canais;
			if (ts3Functions.getChannelList(schID, &canais) != ERROR_ok){
				ts3Functions.logMessage("Error requesting channel list", LogLevel_ERROR, "Plugin", schID);
				return;
			}
			unsigned int i = 0;
			while (canais[i]){
				char* nomeCanal;
				if (ts3Functions.getChannelVariableAsString(schID, canais[i], 0, &nomeCanal) != ERROR_ok){
					ts3Functions.logMessage("Error requesting channel name", LogLevel_ERROR, "Plugin", schID);
					break;
				}
				std::string nome = nomeCanal;
				if (nome == channel){
					musicbot.setChannelID(canais[i]);
					ts3Functions.freeMemory(nomeCanal);
					break;
				}
				ts3Functions.freeMemory(nomeCanal);
				i++;
			}
			ts3Functions.freeMemory(canais);
		}

	}

	void generateLocaleStrings(){
		generateENUSStrings();
		generatePTBRStrings();
	}

}