#include <a_mysql>
#include <../../modules/database.inc>


#define HOST "localhost"
#define USER "root"
#define DTBS "servidor"
#define PASS ""
#define Dialog_Login 0
#define Dialog_Registro 1
new MySQL:ConexaoSQL;


hook OnGameModeInit() {
    ConexaoSQL = mysql_connect(HOST, USER, PASS, DTBS);
    if(mysql_errno(ConexaoSQL) != 0) {
        print("[MySQL] Falha ao tentar estabelecer conexão com o banco de dados.");
    } else {
        print("[MySQL] Sucesso ao conectar com o banco de dados.");
    }
    return 1;
}


hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    new Query[125];

    switch(dialogid) {
        case Dialog_Registro:  {
            if(!response)
                return Kick(playerid);

            if(strlen(inputtext) < 4 || strlen(inputtext) > 24) {
                SendClientMessage(playerid, 0xFF0000AA, "[SERVER] Escolha uma senha entre 4 a 24 caracteres.");
                TogglePlayerSpectating(playerid, 1);

                ShowPlayerDialog(playerid, Dialog_Registro, DIALOG_STYLE_INPUT, "Registro", "Digite uma senha para se registrar em nosso servidor", "Registrar", "Sair"); // Mostra o dialog para ele tentar de novo.
                PlayerData[playerid][pTentativasRegistro]++;

            } else {
                TogglePlayerSpectating(playerid, 0);
                mysql_format(ConexaoSQL, Query, sizeof(Query), "INSERT INTO `jogadores`(`pNome`,`pSenha`) VALUES ('%e', '%e')", GetPlayerNameEx(playerid), inputtext);
                mysql_tquery(ConexaoSQL, Query, "InserirDadosSQL", "i", playerid);
            }
            if(PlayerData[playerid][pTentativasRegistro] == 3) {
                    SendClientMessage(playerid, 0xFF0000AA, "[SERVER] Limite de tentativas de registro excedida.");
                    PlayerData[playerid][pTentativasRegistro] = 0;
                    SetTimerEx("KickPlayerinTime", 200, false, "i", playerid);

            }
        }
        case Dialog_Login: {
            if(!response)
                return Kick(playerid);

            if(!strcmp(PlayerData[playerid][pSenha], inputtext, true, 24)) {
                TogglePlayerSpectating(playerid, 0);
                mysql_format(ConexaoSQL, Query, sizeof(Query), "SELECT * FROM jogadores WHERE pNome='%e'", GetPlayerNameEx(playerid)); // Seleciona tudo aonde o nome dele está
                mysql_tquery(ConexaoSQL, Query, "CarregarContaSQL", "i", playerid);

                SendClientMessage(playerid, 0x80FF00AA, "[Server] Logado com sucesso.");
            } else {
                TogglePlayerSpectating(playerid, 1);
                SendClientMessage(playerid, 0xFF0000AA, "[SERVER] Senha errada, tente novamente.");
                PlayerData[playerid][pTentativasLogin]++;
                ShowPlayerDialog(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "Digite sua senha para entrar em nosso servidor.", "Confirmar", "Sair"); // Mostra a dialog pra ele tentar dnvo
            }
            if(PlayerData[playerid][pTentativasLogin] == 3) {
                SendClientMessage(playerid, 0xFF0000AA, "[SERVER] Limite de tentativas de login excedida.");
                PlayerData[playerid][pTentativasLogin] = 0;
                SetTimerEx("KickPlayerinTime", 200, false, "i", playerid);
            }
        }
    }
    return 1;
}

hook OnPlayerConnect(playerid) {
    new Query[90];
    TogglePlayerSpectating(playerid, 1);

    mysql_format(ConexaoSQL, Query, sizeof(Query), "SELECT `pSenha`, `pID` FROM `Jogadores` WHERE `pNome`='%e'", GetPlayerNameEx(playerid));
    mysql_tquery(ConexaoSQL, Query, "VerificarContaSQL", "i", playerid);
    return 1;
}

public VerificarContaSQL(playerid)
{
    if(cache_num_rows() > 0) {
        cache_get_value_name(0, "pSenha", PlayerData[playerid][pSenha], 24); // Já puxa a senha pra comparar se é igual a que ele vai digitar no dialog.
        ShowPlayerDialog(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "Digite sua senha para entrar em nosso servidor.", "Confirmar", "Sair");
    } else {
        ShowPlayerDialog(playerid, Dialog_Registro, DIALOG_STYLE_INPUT, "Registro", "Digite uma senha para se registrar em nosso servidor", "Registrar", "Sair");
    }
    return 1;
} 

public InserirDadosSQL(playerid) {
    new Query[90];

    PlayerData[playerid][pID] = cache_insert_id();
    printf("[MYSQL] Jogador %s registrado como ID %d", GetPlayerNameEx(playerid), PlayerData[playerid][pID]);

    mysql_format(ConexaoSQL, Query, sizeof(Query), "SELECT * FROM jogadores WHERE pID='%i'", PlayerData[playerid][pID]);
    mysql_query(ConexaoSQL,Query);

    CarregarContaSQL(playerid);
    return 1;
}

public CarregarContaSQL(playerid) {
    PlayerData[playerid][pLogado] = true;

    cache_get_value_int(0, "pID", PlayerData[playerid][pID]);
    cache_get_value_int(0, "pDinheiro", PlayerData[playerid][pDinheiro]);
    cache_get_value_int(0, "pScore", PlayerData[playerid][pScore]); 
    cache_get_value_int(0, "pSkin", PlayerData[playerid][pSkin]);
    cache_get_value_float(0, "pPosX", PlayerData[playerid][pPosX]);
    cache_get_value_float(0, "pPosY", PlayerData[playerid][pPosY]);
    cache_get_value_float(0, "pPosZ", PlayerData[playerid][pPosZ]);
    cache_get_value_float(0, "pPosA", PlayerData[playerid][pPosA]);

    SetPlayerScore(playerid, PlayerData[playerid][pScore]);
    GivePlayerMoney(playerid, PlayerData[playerid][pDinheiro]);

    SetSpawnInfo(playerid, 0, PlayerData[playerid][pSkin], PlayerData[playerid][pPosX], PlayerData[playerid][pPosY], PlayerData[playerid][pPosZ], PlayerData[playerid][pPosA], 0, 0, 0, 0 ,0, 0);
    SpawnPlayer(playerid);

    SetPlayerSkin(playerid, PlayerData[playerid][pSkin]);

    return 1;
}

public SalvarContaSQL(playerid) {
    if(PlayerData[playerid][pLogado] == false)
        return 0;

    new Query[250];

    PlayerData[playerid][pDinheiro] = GetPlayerMoney(playerid); 
    PlayerData[playerid][pScore] = GetPlayerScore(playerid);
    PlayerData[playerid][pSkin] = GetPlayerSkin(playerid);
    GetPlayerPos(playerid, PlayerData[playerid][pPosX], PlayerData[playerid][pPosY], PlayerData[playerid][pPosZ]);
    GetPlayerFacingAngle(playerid, PlayerData[playerid][pPosA]);

    mysql_format(ConexaoSQL, Query, sizeof(Query), "UPDATE `jogadores` SET \
    `pDinheiro`='%i', \
    `pScore`='%i', \
    `pSkin`='%i', \
    `pPosX`='%f', \
    `pPosY`='%f', \
    `pPosZ`='%f', \
    `pPosA`='%f' WHERE `pID`='%i'", PlayerData[playerid][pDinheiro],
                                    PlayerData[playerid][pScore],
                                    PlayerData[playerid][pSkin],
                                    PlayerData[playerid][pPosX],
                                    PlayerData[playerid][pPosY],
                                    PlayerData[playerid][pPosZ],
                                    PlayerData[playerid][pPosA],
                                    PlayerData[playerid][pID]);
    mysql_query(ConexaoSQL, Query);

    printf("[MYSQL] Dados do Jogador %s ID %d salvo com sucesso", GetPlayerNameEx(playerid), PlayerData[playerid][pID]);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(PlayerData[playerid][pLogado] == true && reason >= 0) {
        SalvarContaSQL(playerid);
        ZerarDadosSQL(playerid);
    }
    return 1;
} 