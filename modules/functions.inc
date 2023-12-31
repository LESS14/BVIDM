stock GetPlayerNameEx(playerid) {
    static Name[MAX_PLAYER_NAME+1]; // MAX_PLAYER_NAME + '\0'
    GetPlayerName(playerid, Name, sizeof(Name));
    return Name;
}

stock ZerarDadosSQL(playerid) {
    PlayerData[playerid][pID] = 0;
    PlayerData[playerid][pSenha] = 0;
    PlayerData[playerid][pDinheiro] = 0;
    PlayerData[playerid][pScore] = 0;
    PlayerData[playerid][pSkin] = 0;
    PlayerData[playerid][pLogado] = false;
    PlayerData[playerid][pTentativasLogin] = 0;
    PlayerData[playerid][pTentativasRegistro] = 0;
    PlayerData[playerid][pPosX] = 0;
    PlayerData[playerid][pPosA] = 0;
    PlayerData[playerid][pPosY] = 0;
    PlayerData[playerid][pPosA] = 0;
    return 1;
}