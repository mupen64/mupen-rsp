#include <windows.h>
#include "Config.h"

#include <cstdio>

#include "win.h"

#define CfgFileName "mupen64_rsp_hle.cfg"

////////////////////// Service functions and structures ////////////////////////


char *CfgFilePath()
{
   static char *cfgpath = NULL;
   if (cfgpath == NULL)
     {
       	cfgpath = (char*)malloc(strlen(AppPath)+1+strlen(CfgFileName));
	    strcpy(cfgpath, AppPath);
    	strcat(cfgpath, CfgFileName);
     }
   return cfgpath;
}


void WriteCfgString   (const char *Section,const char *Key,char *Value) 
{
    WritePrivateProfileString( Section, Key, Value, CfgFilePath());
}


void WriteCfgInt      (const char *Section,const char *Key,int Value) 
{
    static char TempStr[100];
    sprintf(TempStr,"%d",Value);
    WriteCfgString( Section, Key, TempStr );
}


void ReadCfgString    (const char *Section,const char *Key,const char *DefaultValue,char *retValue) 
{
    GetPrivateProfileString( Section, Key, DefaultValue, retValue, 100, CfgFilePath());
}


int ReadCfgInt        (const char *Section,const char *Key,int DefaultValue) 
{
    return GetPrivateProfileInt( Section, Key, DefaultValue, CfgFilePath());
}

//////////////////////////// Load and Save Config //////////////////////////////

void LoadConfig()
{
    AudioHle = ReadCfgInt( "Settings", "AudioHle", FALSE);
    GraphicsHle = ReadCfgInt( "Settings", "GraphicsHle", TRUE);
    SpecificHle = ReadCfgInt( "Settings", "SpecificHle", FALSE);
    ReadCfgString ( "Settings", "Audio Plugin", "", audioname );
}

/////////////////////////////////////////////////////////////////////////////////


void SaveConfig()
{
    WriteCfgInt( "Settings", "AudioHle", AudioHle);
    WriteCfgInt( "Settings", "GraphicsHle", GraphicsHle);
    WriteCfgInt("Settings","SpecificHle",SpecificHle);
    WriteCfgString("Settings","Audio Plugin",audioname);
}


