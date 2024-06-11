
void WriteCfgString   (const char *Section,const char *Key, char *Value) ;
void WriteCfgInt      (const char *Section,const char *Key, int Value) ;
void ReadCfgString    (const char *Section,const char *Key, const char *DefaultValue,char *retValue) ;
int ReadCfgInt        (const char *Section,const char *Key, int DefaultValue) ;

 
void LoadConfig()  ;
void SaveConfig()  ; 
    
