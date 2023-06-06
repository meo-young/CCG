#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<time.h>
#define STRING_SIZE 256

/*******************************************************************************************************************/
int turn = 0;
int state = 0;
int p1_draw = 0;
int p2_draw = 0;
int game_cost = 1;
int possibe_cost = 1;
int total_turn = 0;
int p1_draw_damage = 1;
int p2_draw_damage = 1;
// 카드 정보
struct card {
	int type; // 1차 때 int num 이었던 것 2차 때 type으로 변경
	int tribe; // 2차 추가
	char name[100];
	int cost;
	int atk;
	int hp;
	int ability; // 2차 추가
	char line[100];
	char cha[5];
	int att_state;
};

//유저 정보
struct user {
	char id[13];
	char pwd[17];
	char nick[9];
};

//플레이어 정보
struct player {
	char name[9];
	int hp;
	struct card deck[30];
	struct card hand[10];
	struct card field[6];
	int fieldon[6];
	int handnum;
	int fieldnum;
};

/*******************************************************************************************************************/
// 함수 선언
void setting(struct player* p);
void menu(struct user* userlist, int* userindex, struct player* players);
void plusmember(struct user* userlist, int* userindex);
char* login(struct user* userlist, int* userindex);
void rule();
void GotoXY(int x, int y);
void field(struct player player1, struct player player2);
void empty_card(int x, int y, int j);
void full_card(int x, int y, struct card card, int j);
void draw_player(int x, int y, int i, struct player player);
void player_hand(struct player player);
void hand_out(int card_num, struct player* p);
void attack(int my_card, int target, struct player* p);
void player_attack(int my_card, struct player* p);
void quit(struct player* p);
void game_end(struct player *p);
void game_menu(struct player* p);
void who_first();
void trim(char* str, char* cpy, int cpy_len);
void deck_setting(struct player* p, int t);

/*******************************************************************************************************************/
//메인 함수
int main()
{
	struct card cardlist[44];
	struct user userlist[100];
	struct player players[2];
	FILE* cardFile = fopen("card.txt", "r");
	int index = 0;
	int card_index, my_card, target;
	if (cardFile != NULL) {
		char buffer[STRING_SIZE];
		while (!feof(cardFile)) {
			fgets(buffer, sizeof(buffer), cardFile);

			char* ptr = strtok(buffer, " ");
			int ab = 0;
			// 2차 추가 - 카드 생성 여기부터
			while (ptr != NULL) {
				if (ab == 0) {
					cardlist[index].type = atoi(ptr);
				}
				else if (ab == 1) {
					cardlist[index].tribe = atoi(ptr);
				}
				else if (ab == 2) {
					while (1) {
						char* pt = strstr(ptr, "_");
						if (pt == NULL) {
							break;
						}
						strncpy(pt, " ", 1);
					}
					strcpy(cardlist[index].name, ptr);
				}
				else if (ab == 3) {
					cardlist[index].cost = atoi(ptr);
				}
				else if (ab == 4) {
					cardlist[index].atk = atoi(ptr);
				}
				else if (ab == 5) {
					cardlist[index].hp = atoi(ptr);
				}
				else if (ab == 6) {
					cardlist[index].ability = atoi(ptr);
				}
				else if (ab == 7) {
					while (1) {
						char* pt = strstr(ptr, "_");
						if (pt == NULL) {
							break;
						}
						strncpy(pt, " ", 1);
					}
					strcpy(cardlist[index].line, ptr);
				}
				else {
					printf("카드 정보를 불러올 수 없습니다.\n프로그램을 종료합니다.");
					return;
				}
				ptr = strtok(NULL, " ");
				ab++;
			}
			// 2차 추가 - 카드 생성 여기까지 (while 문)
			index++;
		}
		fclose(cardFile);
	}
	else {
		printf("카드 정보를 불러올 수 없습니다.\n프로그램을 종료합니다.");
		return;
	}

	FILE* userFiletest = fopen("user.txt", "r");
	if (userFiletest == NULL) {
		printf("회원 정보 데이터 파일을 새로 생성했습니다.");
		FILE* userFiletest = fopen("user.txt", "w");
		fclose(userFiletest);
	}
	else {
		fclose(userFiletest);
	}
	FILE* userFile = fopen("user.txt", "r");
	int userindex = 0;
	if (userFile != NULL) {
		char buffer[STRING_SIZE];
		while (!feof(userFile)) {
			fgets(buffer, sizeof(buffer), userFile);

			char* ptr = strtok(buffer, " ");
			int ab = 0;
			while (ptr != NULL) {
				if (ab == 0) {
					strcpy(userlist[userindex].id, ptr);
				}
				else if (ab == 1) {
					strcpy(userlist[userindex].pwd, ptr);
				}
				else if (ab == 2) {
					strcpy(userlist[userindex].nick, ptr);
				}
				ptr = strtok(NULL, " \n");
				ab++;
			}
			if (ab > 1) {
				userindex++;
			}
		}
		fclose(userFile);
	}

	// 2차 추가 - 종족별 덱 생성 여기부터
	struct card warrior_deck[30];
	struct card magician_deck[30];
	struct card assassin_deck[30];
	int widx = 0;
	int midx = 0;
	int aidx = 0;
	for (int i = 0; i < 44; i++) {
		if (cardlist[i].tribe == 0) {
			if (!strcmp(cardlist[i].name,"coin")) {
				for (int j = 0; j < 2; j++) {
					warrior_deck[widx++] = cardlist[i];
					magician_deck[midx++] = cardlist[i];
					assassin_deck[aidx++] = cardlist[i];
				}
			}
		}
		else if (cardlist[i].tribe == 1) {
			for (int j = 0; j < 2; j++) {
				warrior_deck[widx++] = cardlist[i];
			}

		}
		else if (cardlist[i].tribe == 2) {
			for (int j = 0; j < 2; j++) {
				magician_deck[midx++] = cardlist[i];
			}
		}
		else if (cardlist[i].tribe == 3) {
			for (int j = 0; j < 2; j++) {
				assassin_deck[aidx++] = cardlist[i];
			}
		}
	}
	// 2차 추가 - 종족별 덱 생성 여기까지

	while (1) {
		if (state == 0) {
			menu(userlist, &userindex, players);
		}
		else if (state == 1) {
			game_menu(players);
		}
		else if (state == 2) {
			rule(); // 게임룰 출력
			getchar(); // 아무키나 입력시 다음 화면으로
			who_first(); // 선공, 후공 결정

			//난수 생성할 때마다 다른 결과가 나오도록 시드값 조정
			srand(time(NULL));

			// 0~29까지 중복되지 않도록 난수 생성 !
			int random[30];
			for (int i = 0; i < 30; i++) {
				random[i] = rand() % 30;
				for (int j = 0; j < i; j++) {
					if (random[i] == random[j]) {
						random[i] = rand() % 30;
						j = -1;
					}
				}
			}

			for (int i = 0; i < 30; i++) //random 순서로 덱 불러오기
			{
				players[0].deck[random[i]] = cardlist[(int)(i / 2) + 1];
			}

			for (int i = 0; i < 30; i++) {
				random[i] = rand() % 30;
				for (int j = 0; j < i; j++) {
					if (random[i] == random[j]) {
						random[i] = rand() % 30;
						j = -1;
					}
				}
			}

			for (int i = 0; i < 30; i++) //random 순서로 덱 불러오기
			{
				players[1].deck[random[i]] = cardlist[(int)(i / 2) + 1];
			}

			//처음 소환한 하수인은 공격 못 하도록 상태 설정
			// 2차 추가 -  속공 추가로 인한 변경 여기부터
			for (int i = 0; i < 30; i++) {
				if (players[0].deck[i].ability == 1) {
					players[0].deck[i].att_state = 1;
				}
				else {
					players[0].deck[i].att_state = -1;
				}
				if (players[1].deck[i].ability == 1) {
					players[1].deck[i].att_state = 1;
				}
				else {
					players[1].deck[i].att_state = -1;
				}
			}
			// 2차 추가 -  속공 추가로 인한 변경 여기까지

			//후공인 플레이어에게 동전
			players[!turn].hand[3] = cardlist[0];

			//게임 초반 셋팅
			setting(players);
			system("cls");

			char command[STRING_SIZE];
			char com;
			int t1, t2;
			char t2_string[STRING_SIZE];
			char trim_t2[STRING_SIZE];
			int k = 0;
			int space_count = 0;
			int ann;
			while (1) {
				k = 0;
				field(players[0], players[1]);
				player_hand(players[turn]);
				GotoXY(110, 43);
				printf("Mana Cost (%d/%d)", game_cost, possibe_cost);
				GotoXY(3, 59);
				printf("\n\"%s\"님의 턴! 어떤 명령을 내리시겠습니까 : ", players[turn].name);
				scanf(" %c", &com);
				if (com == 'a' || com == 'A' || space_count!=0) {
					ann = getchar();
					if (ann == ' ' || ann == '\t') {
						scanf(" %d", &t1);
						ann = getchar();
						if (ann == ' ' || ann == '\t') {
							scanf(" %s", t2_string);
							while ((ann = getchar()) != '\n') {
								if (ann == ' ' || ann == '\t') {

								}
								else {
									k = 1;
								}
							}
						}
						else {
							k = 1;
						}
					}
					else {
						k = 1;
					}
					if (k != 1) {
						trim(t2_string, trim_t2, STRING_SIZE);

						system("cls");

						if (strcmp(trim_t2, "player") == 0 || strcmp(trim_t2, "Player") == 0) {
							player_attack(t1, players);
						}
						else {
							if (strcmp(trim_t2, "1") == 0) {
								t2 = 1;
							}
							else if (strcmp(trim_t2, "2") == 0) {
								t2 = 2;
							}
							else if (strcmp(trim_t2, "3") == 0) {
								t2 = 3;
							}
							else if (strcmp(trim_t2, "4") == 0) {
								t2 = 4;
							}
							else if (strcmp(trim_t2, "5") == 0) {
								t2 = 5;
							}
							else if (strcmp(trim_t2, "6") == 0) {
								t2 = 6;
							}
							attack(t1, t2, players);
						}
						k = 0;
					}
				}
				else if (com == 'h' || com == 'H') {
					ann = getchar();
					if (ann == ' ' || ann == '\t') {
						scanf(" %d", &t1);
						while ((ann = getchar()) != '\n') {
							if (ann == ' ' || ann == '\t') {

							}
							else {
								k = 1;
							}
						}
					}
					else {
						k = 1;
					}
					
					if(k!=1) {
						system("cls");
						hand_out(t1, players);
					}
				}
				else if (com == 'q' || com == 'Q') {
					while ((ann = getchar()) != '\n') {
						if (ann == ' ' || ann == '\t') {
						}
						else {
							k = 1;
						}
					}
					if (k != 1) {
						system("cls");
						quit(players);
					}
				}
				//a, q, h 이외의 첫 문자를 입력했을 경우
				else {
					k = 1;
				}

				if(k==1) {
					system("cls");
					printf("제대로 입력한게 맞는지 확인해줘 !\n");
					printf("\t올바른 입력 예시 : h/H 1, a/A 1 2, q/Q\n");
				}

				if (players[0].hp <= 0) {
					system("cls");
					printf("\"%s\" 님의 hp가 바닥났네요 ! \n\n!", players[0].name);
					printf("\"%s\" 님의 승리 !", players[1].name);
					break;
				}
				else if (players[1].hp <= 0) {
					system("cls");
					printf("\"%s\" 님의 hp가 바닥났네요 ! \n\n!", players[1].name);
					printf("\"%s\" 님의 승리 !", players[0].name);
					break;
				}
			}
			while ((ann = getchar()) != '\n');
			state = 1;
		}
	}
}

/*******************************************************************************************************************/
//함수 정의

void who_first() {
	system("cls");
	char choice[STRING_SIZE];
	char tmp1[STRING_SIZE];

	while (1) {
		printf("\n 원하시는 공격방식을 선택해주세요 \n");
		printf(" ┌─────────────┐\n");
		printf(" │ 1. 선공     │\n");
		printf(" └─────────────┘\n");
		printf(" ┌─────────────┐\n");
		printf(" │ 2. 후공     │\n");
		printf(" └─────────────┘\n");

		scanf("%[^\n]s", choice);
		getchar();
		trim(choice, tmp1, STRING_SIZE);

		if (strcmp(tmp1, "선공") == 0)
		{
			turn = 0;
			break;
		}
		else if (strcmp(tmp1, "후공") == 0)
		{
			turn = 1;
			break;
		}
		else
		{
			printf(" 올바른 한글열을 입력해주세요!\n");
			printf("예시) \"선공\",\"후공\"\n\n");
		}
	}
	system("cls");
}

void game_menu(struct player* p) {
	system("cls");
	char choice[STRING_SIZE];
	char tmp2[STRING_SIZE];

	while (1) {
		printf("\n 환영합니다! %s님 ! \n\n", p[0].name);

		printf(" ┌─────────────┐\n");
		printf(" │ 1. 게임시작 │\n");
		printf(" └─────────────┘\n");
		printf(" ┌─────────────┐\n");
		printf(" │ 2. 로그아웃 │\n");
		printf(" └─────────────┘\n");
		printf(" ┌─────────────┐\n");
		printf(" │ 3. 종료     │\n");
		printf(" └─────────────┘\n");
		printf("\n 원하시는 메뉴를 선택해주세요 : ");

		scanf("%[^\n]s", choice);
		getchar();
		trim(choice, tmp2, STRING_SIZE);
		system("cls");

		if (strcmp(tmp2, "게임시작") == 0 || strcmp(tmp2, "1") == 0)
		{
			state = 2;
			break;
		}
		else if (strcmp(tmp2, "로그아웃") == 0 || strcmp(tmp2, "2") == 0)
		{
			state = 0;
			break;
		}
		else if (strcmp(tmp2, "종료") == 0)
		{
			printf("다음에 또 봐요~!");
			exit(0);
		}
		else
		{
			printf("올바른 명령어를 입력해주세요!\n");
			printf("\"1\",\"게임시작\",\"2\",\"로그아웃\",\"종료\"\n");
		}
	}

}

void setting(struct player *p)
{
	p[0].hp = 30;
	p[1].hp = 30;
	p[0].fieldnum = 0;
	p[1].fieldnum = 0;

	if (turn == 0) {
		p[0].handnum = 3;
		p1_draw = 3;
		p[1].handnum = 4;
		p2_draw = 3;
	}
	else if (turn == 1) {
		p[0].handnum = 4;
		p1_draw = 3;
		p[1].handnum = 3;
		p2_draw = 3;
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			p[i].fieldon[j] = 0;
		}
	}
	for (int i = 0; i < p1_draw;i++) {
		p[0].hand[i] = p[0].deck[i];
	}
	for (int i = 0; i < p2_draw;i++) {
		p[1].hand[i] = p[1].deck[i];
	}
}

void menu(struct user* userlist, int* userindex, struct player* players) 
{
	char choice[STRING_SIZE];
	char playername[9];
	char tmp3[STRING_SIZE];
	while (1) {
		printf("\n");
		printf(" 어서오세요! 건국 온라인입니다! 무엇을 도와드릴까요 ??\n");
		printf("┌─────────────┐\n");
		printf("│ 1. 회원가입 │\n");
		printf("└─────────────┘\n");
		printf("┌─────────────┐\n");
		printf("│ 2. 로그인   │\n");
		printf("└─────────────┘\n");
		printf("┌─────────────┐\n");
		printf("│ 3. 종료     │\n");
		printf("└─────────────┘\n");
		printf("원하시는 메뉴를 선택해주세요 : ");
		scanf("%[^\n]s", choice);
		getchar();
		trim(choice, tmp3, STRING_SIZE);
		system("cls");
		if (strcmp(tmp3, "회원가입") == 0 || strcmp(tmp3, "1") == 0) 
		{
			plusmember(userlist, userindex);
		}
		else if (strcmp(tmp3, "로그인") == 0 || strcmp(tmp3, "2") == 0)
		{
			strcpy(players[0].name, login(userlist, userindex));
			strcpy(players[1].name, "enemy");
			state = 1;
			break;
		}
		else if (strcmp(tmp3, "종료") == 0)
		{
			printf("다음에 또 봐요~!");
			exit(0);
		}
		else 
		{
			printf("올바른 명령어를 입력해주세요!\n");
			printf("\"1\",\"회원가입\",\"2\",\"로그인\",\"종료\"\n");
		}
	}
	printf("메인메뉴\n");
}

void plusmember(struct user* userlist, int* userindex) 
{
	char id[STRING_SIZE];
	char pwd[STRING_SIZE];
	int error = 0;
	printf("\n");
	printf("┌───────────────────────────┐\n");
	printf("│                           │\n");
	printf("│  id: [        ] (4 ~ 12)  │\n");
	printf("│                           │\n");
	printf("│  pw: [        ] (8 ~ 16 ) │\n");
	printf("│                           │\n");
	printf("└───────────────────────────┘\n");
	// 1차 때 누락되어 2차 때 다시 추가 - 오류 메시지 여기부터
	do{
		printf("사용하실 아이디를 입력해주세요 : ");
		scanf("%[^\n]s", id);
		getchar();
		error = 0;
		if (strlen(id) < 4)
		{
			printf("id의 길이가 정해진 길이 미만입니다.\n");
			error++;
		}
		else if (strlen(id) > 12)
		{
			printf("id의 길이가 정해진 길이를 초과합니다.\n");
			error++;
		}
		for (int i = 0; i < strlen(id); i++)
		{
			if (!((id[i] >= 48 && id[i] <= 57) || (id[i] >= 65 && id[i] <= 90) || (id[i] >= 97 && id[i] <= 122)))
			{
				printf("id에 알파벳과 숫자 이외의 문자가 포함되어 있습니다.\n");
				error++;
				break;
			}
		}
		for (int i = 0; i < *userindex; i++)
		{
			if (strcmp(userlist[i].id, id) == 0)
			{
				printf("중복된 id입니다.\n");
				error++;
				break;
			}
		}
	} while (error > 0);
	do {
		printf("사용하실 비밀번호를 입력해주세요 : ");
		scanf("%[^\n]s", pwd);
		getchar();
		error = 0;
		if (strlen(pwd) < 8)
		{
			printf("pw의 길이가 정해진 길이 미만입니다.\n");
			error++;
		}
		else if (strlen(pwd) > 16)
		{
			printf("pw의 길이가 정해진 길이를 초과합니다.\n");
			error++;
		}
		for (int i = 0; i < strlen(pwd); i++)
		{
			if (!((pwd[i] >= 48 && pwd[i] <= 57) || (pwd[i] >= 65 && pwd[i] <= 90) || (pwd[i] >= 97 && pwd[i] <= 122)))
			{
				printf("pw에 알파벳과 숫자 이외의 문자가 포함되어 있습니다.\n");
				error++;
				break;
			}
		}
	} while (error > 0);
	// 1차 때 누락되어 2차 때 다시 추가 - 오류 메시지 여기까지

	char nick[STRING_SIZE];
	do {
		printf("\n");
		printf("┌───────────────────────────────┐\n");
		printf("│                               │\n");
		printf("│닉네임: [        ](2 ~ 8)      │\n");
		printf("│                               │\n");
		printf("└───────────────────────────────┘\n");
		printf("사용하실 닉네임을 입력해주세요 : ");
		scanf("%[^\n]s", nick);
		getchar();
		error = 0;
		if (strlen(nick) < 2) 
		{
			printf("닉네임의 길이가 정해진 길이 미만입니다.\n");
			error++;
		}
		else if (strlen(nick) > 8) 
		{
			printf("닉네임의 길이가 정해진 길이를 초과합니다.\n");
			error++;
		}
		for (int i = 0; i < strlen(nick); i++) 
		{
			if (!((nick[i] >= 48 && nick[i] <= 57) || (nick[i] >= 65 && nick[i] <= 90) || (nick[i] >= 97 && nick[i] <= 122))) 
			{
				printf("닉네임에 알파벳과 숫자 이외의 문자가 포함되어 있습니다.\n");
				error++;
				break;
			}
		}
		for (int i = 0; i < *userindex; i++) 
		{
			if (strcmp(userlist[i].nick, nick) == 0) 
			{
				printf("중복된 닉네임입니다.\n");
				error++;
				break;
			}
		}
	} while (error > 0);

	FILE* userFile = fopen("user.txt", "a");
	if (*userindex != 0) 
	{
		fputs("\n", userFile);
	}
	fputs(id, userFile);
	fputs(" ", userFile);
	fputs(pwd, userFile);
	fputs(" ", userFile);
	fputs(nick, userFile);
	fclose(userFile);

	strcpy(userlist[*userindex].id, id);
	strcpy(userlist[*userindex].pwd, pwd);
	strcpy(userlist[*userindex].nick, nick);
	(*userindex)++;
}

char* login(struct user* userlist, int* userindex) 
{
	char id[STRING_SIZE];
	char pwd[STRING_SIZE];
	printf("\n");
	printf("┌───────────────────────────┐\n");
	printf("│                           │\n");
	printf("│  id: [        ] (4 ~ 12자)│\n");
	printf("│                           │\n");
	printf("│  pw: [        ] (8 ~ 16자)│\n");
	printf("│                           │\n");
	printf("└───────────────────────────┘\n");
	int check = 0;
	char playername[9];
	while (check == 0) 
	{
		printf("아이디를 입력해주세요 : ");
		scanf("%[^\n]s", id);
		getchar();
		printf("비밀번호를 입력해주세요 : ");
		scanf("%[^\n]s", pwd);
		getchar();
		for (int i = 0; i < *userindex; i++) 
		{
			if (strcmp(userlist[i].id, id) == 0 && strcmp(userlist[i].pwd, pwd) == 0) 
			{
				strcpy(playername, userlist[i].nick);
				printf("환영합니다 \"%s\"님\n", userlist[i].nick);
				check++;
				break;
			}
		}
		if (check == 0) 
		{
			printf("잘못된 정보입니다.\n");
		}
	}
	return playername;
}

void rule() 
{
	printf(" 플레이어 카드\n");
	printf("┌───────────┐\n");
	printf("│  (닉네임) │\n");
	for (int i = 0; i < 5; i++) 
	{
		printf("│           │\n");
	}
	printf("│      hp: ?│\n");
	printf("└───────────┘\n");
	printf(" 플레이어 카드에는 사용자의 닉네임과 체력 상태가 표시됩니다.\n");

	printf("\n\n 빈 카드\n");
	printf("┌───────────┐\n");
	for (int i = 0; i < 3; i++) 
	{
		printf("\n");
		printf("│           │\n");
	}
	printf("\n");
	printf("└───────────┘\n");
	printf(" 플레이어가 아무 카드도 전장에 내지 않았을 때 표시되는 카드입니다.\n");

	printf("\n\n 하수인 카드\n");
	printf("┌───────────┐\n");
	printf("│Pawn, Magic│\n");
	printf("│    Name   │\n");
	printf("│           │\n");
	printf("│           │\n");
	printf("│           │\n");
	printf("│(att)  (hp)│\n");
	printf("│   ? cost  │\n");
	printf("└───────────┘\n");
	printf(" 플레이어가 카드를 전장에 냈을 때 표시되는 카드입니다.\n");
	printf("\n 카드에는 카드의 성격(Pawn, Magic)과, 카드이름(Name), 체력 및 공격력(att, hp), 소요되는 마나(cost)가 표시됩니다.\n");


	printf("\n---------------------게임 진행 방식---------------------\n");
	printf("\n 선공인 플레이어는 카드 3장, 후공인 플레이어는 \"동전\"을 포함한 카드 4장으로 게임을 시작합니다.\n");
	printf("\n자신의 턴이 돌아올 때마다 각 플레이어는 카드 1장을 덱에서 꺼냅니다.\n");
	printf("\n만약 사용자가 카드 10장을 쥐고있는 상태에서 턴이 돌아올 경우, 덱에서 꺼낸 1장의 카드는 소멸됩니다.\n");
	printf("\n자신의 턴이 돌아왔을 때 덱에 남은 카드가 없을 경우, 플레이어의 체력을 1깎습니다.\n");
	printf("이후 턴이 돌아올 때마다 깎이는 체력은 1씩 증가합니다.");
	printf("\n 플레이어는 a/A, h/H, q/Q 3가지의 명령어로 게임을 진행할 수 있습니다.\n\n");
	printf(" a/A : Attack의 약어로써 전장에 있는 하수인이 어떤 유닛을 공격할지 선택할 수 있습니다.\n");
	printf(" 전장에 하수인이 존재하지 않을 경우 사용할 수 없습니다.\n");
	printf(" ex) a/A \"공격하는 대상의 번호 및 이름\", \"공격받는 대상의 번호 및 이름\"\n\n");
	printf(" h/H : Hand out의 약어로써 플레이어 손에 있는 카드를 전장에 낼 수 있습니다.\n");
	printf(" 손에 쥐고있는 카드가 없을 경우 사용할 수 없습니다.\n");
	printf(" ex) h/H \"낼 카드의 번호 및 이름\"\n\n");
	printf(" q/Q : Quit의 약어로써 플레이어의 턴을 마치고 상대에게 턴을 넘깁니다.\n");
	printf(" ex) q/Q \n\n");
}

void GotoXY(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void field(struct player player1, struct player player2)
{
	printf("\n");
	GotoXY(0, 5);

	for (int i = 0; i < 42; i++)
	{
		if (i == 0) 
		{
			printf("┌───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");

		}
		else if (i == 41) 
		{
			printf("└───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
		}
		else 
		{
			printf("│\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t            │\n");
		}
	}

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			draw_player(60, j + 6, j, player1);
			draw_player(60, j + 37, j, player2);
			if (player1.fieldon[i] == 0)
			{
				empty_card(i * 23 + 3, j + 16, j);
			}
			else if (player1.fieldon[i] == 1)
			{
				GotoXY(i * 23 + 9, 15);
				printf("%d", i+1);
				full_card(i * 23 + 3, j + 16, player1.field[i], j);
			}
			if (player2.fieldon[i] == 0)
			{
				empty_card(i * 23 + 3, j + 27, j);
			}
			else if (player2.fieldon[i] == 1)
			{
				full_card(i * 23 + 3, j + 27, player2.field[i], j);
				GotoXY(i * 23 + 9, 36);
				printf("%d", i+1);
			}
		}
	}
}

void empty_card(int x, int y, int j)
{
	GotoXY(x, y);
	if (j == 0) 
	{
		printf("┌  - - - - ┐");
	}
	else if (j == 8) 
	{
		printf("└  - - - - ┘");
	}
	else if (j % 2 == 0) 
	{
		printf("│          │");
	}
}

void full_card(int x, int y, struct card card, int j)
{
	GotoXY(x, y);
	if (j == 0) 
	{
		printf("┌───────────┐");
	}
	else if (j == 1) 
	{
		if (strcmp(card.name, "coin")==0) {
			printf("│   Magic   │");
		}
		else {
			printf("│    Pawn   │");
		}
	}
	else if (j == 2) 
	{
		printf("│%8s   │", card.name);
	}
	else if (j == 6) 
	{
		printf("│ %2d     %2d │", card.atk, card.hp);
	}
	else if (j == 7) 
	{
		printf("│  %2d cost  │", card.cost);
	}
	else if (j == 8) 
	{
		printf("└───────────┘");
	}
	else 
	{
		printf("│           │");
	}
}

void draw_player(int x, int y, int i, struct player player)
{
	GotoXY(x, y);
	if (i == 0) {
		printf("┌───────────┐");
	}
	else if (i == 8) {
		printf("└───────────┘");
	}
	else if (i == 1) {
		printf("│ %8s  │", player.name);
	}
	else if (i == 7) {
		printf("│      hp:%2d│", player.hp);
	}
	else {
		printf("│           │");
	}
}

void player_hand(struct player player)
{
	for (int i = 0; i < player.handnum; i++) {
		GotoXY(i * 15 + 9, 48);
		printf("%d", i+1);
	}
	for (int i = 0; i < player.handnum; i++) 
	{
		for (int j = 0; j < 9; j++) 
		{
			full_card(i * 15 + 3, j + 49, player.hand[i], j);
		}
	}
}

void hand_out(int card_num, struct player* p)
{
	if (p[turn].handnum == 0) {
		printf(" 손에 카드가 없는걸 ?..\n");
	}
	else if (card_num > p[turn].handnum)
	{
		printf(" 넌 지금 카드 %d장을 들고 있다구\n", p[turn].handnum);
	}
	else if (p[turn].hand[card_num - 1].cost > game_cost) {
		printf(" 그 카드는 내기엔 무리야.. \n");
	}
	else if (p[turn].fieldnum < 6)
	{	
		if (strcmp(p[turn].hand[card_num - 1].name, "coin") == 0) {
			game_cost++;
		}
		else {
			for (int i = 0; i < 6; i++)
			{
				if (p[turn].fieldon[i] == 0)
				{
					p[turn].fieldon[i] = 1;
					p[turn].field[i] = p[turn].hand[card_num - 1];
					break;
				}
			}
			p[turn].fieldnum++;
			game_cost -= (p[turn].hand[card_num - 1].cost);
		}
		printf("\"%s\"님이 \"%s\"카드를 냈습니다 !\n",p[turn].name,p[turn].hand[card_num-1].name);

		printf("%s : %s", p[turn].hand[card_num - 1].name, p[turn].hand[card_num - 1].line);
		for (int i = card_num - 1; i < sizeof(p[turn].hand) / sizeof(struct card); i++)
		{
			p[turn].hand[i] = p[turn].hand[i + 1];
		}
		p[turn].handnum--;
		
	}
	else if (p[turn].fieldnum >= 6)
	{
		printf(" 전장에 빈 자리가 없는걸 ?");
	}
}

void attack(int my_card, int target, struct player* p)
{
	int n = 0; // 2차 추가 도발
	if (my_card < 1 || my_card>6 || target > 6 || target < 1) {
		printf(" 거기엔 아무것도 없다구 !\n");
	}
	else if (p[turn].fieldon[my_card-1]==1 &&p[!turn].fieldon[target-1]==1)
	{
		// 2차 추가 도발 여기부터
		for (int i = 0; i < 5; i++) {
			if (p[!turn].fieldon[i] == 1) {
				if (p[!turn].field[i].ability == 2) {
					n = 1;
					break;
				}
			}
		}
		if (p[!turn].field[target - 1].ability == 2) {
			n = 2;
		}
		// 2차 추가 도발 여기까지

		if (p[turn].field[my_card - 1].att_state == -1) {
			printf(" 그 하수인은 아직 마음의 준비가 안 됐어 \n");
		}
		else if (p[turn].field[my_card - 1].att_state == 0) {
			printf(" 그 하수인은 이미 공격을 했다구 !\n");
		}
		else if (n == 0 || n == 2) { // 2차 변경 도발
			// 2차 추가 - 관통, 선제공격 여기부터
			if (p[turn].field[my_card - 1].ability == 3) {
				if (p[turn].field[my_card - 1].atk > p[!turn].field[target - 1].hp) {
					p[!turn].hp -= p[turn].field[my_card - 1].atk - p[!turn].field[target - 1].hp;
				}
			}
			else if (p[turn].field[my_card - 1].ability == 4) {
				if (p[turn].field[my_card - 1].atk >= p[!turn].field[target - 1].hp) {
					p[turn].field[my_card - 1].hp += p[!turn].field[target - 1].atk;
				}
			}
			// 2차 추가 - 관통, 선제공격 여기까지
			printf("\n \"%s\"가 \"%s\"를 공격하였습니다 !\n", p[turn].field[my_card - 1].name, p[!turn].field[target - 1].name);
			p[turn].field[my_card - 1].hp -= p[!turn].field[target - 1].atk;
			p[!turn].field[target - 1].hp -= p[turn].field[my_card - 1].atk;
			p[turn].field[my_card - 1].att_state = 0;
			if (p[turn].field[my_card - 1].hp <= 0)
			{
				p[turn].fieldon[my_card - 1] = 0;
				p[turn].fieldnum--;
			}
			if (p[!turn].field[target - 1].hp <= 0)
			{
				p[!turn].fieldon[target - 1] = 0;
				p[!turn].fieldnum--;
			}
		}
		// 2차 추가 도발 여기부터
		else {
			printf("도발 하수인을 먼저 처리해줘 \n");
		}
		// 2차 추가 도발 여기까지
	}
	else {
		printf(" 거기엔 아무것도 없다구 !\n");
	}
}

void player_attack(int my_card, struct player* p)
{
	if (my_card < 1 || my_card >6) {
		printf(" 거기엔 아무것도 없다구 !\n");
	}
	else if (p[turn].fieldon[my_card - 1] == 1) {
		if (p[turn].field[my_card - 1].att_state == -1) {
			printf(" 그 하수인은 아직 마음의 준비가 안 됐어 \n");
		}
		else if (p[turn].field[my_card - 1].att_state == 0) {
			printf(" 그 하수인은 이미 공격을 했다구 !\n");
		}
		else {
			printf("\n \"%s\"가 \"%s\"를 공격하였습니다 !\n", p[turn].field[my_card - 1].name, p[!turn].name);
			p[!turn].hp -= p[turn].field[my_card - 1].atk;
			p[turn].field[my_card - 1].att_state = 0;
		}
	}
	else {
		printf(" 거기엔 아무것도 없다구 !\n");
	}
}

void quit(struct player* p)
{	
	game_cost = possibe_cost;
	if (turn == 0) {
		printf("\"%s\" 님의 턴 종료 !\n", p[turn].name);
		for (int i = 0; i < 6; i++) {
			p[0].field[i].att_state = 1;
		}
		turn = 1;
		if (p2_draw == 30) {
			printf("%s 님의 hp가 %d만큼 깎였습니다.\n", p[1].name, p2_draw_damage);
			p[1].hp -= p2_draw_damage++;
		}
		else {
			if (p[1].handnum == 10) {
				printf("%s 가 소멸되었습니다.\n", p[1].deck[p2_draw].name);
				p2_draw++;
			}
			else {
				p[1].hand[p[1].handnum] = p[1].deck[p2_draw];
				p2_draw++;
				p[1].handnum++;
			}
		}
	}
	else if (turn == 1) {
		printf("\"%s\" 님의 턴 종료 !\n", p[turn].name);
		for (int i = 0; i < 6; i++) {
			p[1].field[i].att_state = 1;
		}
		turn = 0;
		if (p1_draw==30) {
			printf("%s 님의 hp가 %d만큼 깎였습니다.\n", p[0].name, p1_draw_damage);
			p[0].hp -= p1_draw_damage++;
		}
		else {
			if (p[0].handnum == 10) {
				printf("%s 가 소멸되었습니다.\n", p[0].deck[p1_draw].name);
				p1_draw++;
			}
			else {
				p[0].hand[p[0].handnum] = p[0].deck[p1_draw];
				p1_draw++;
				p[0].handnum++;
			}
		}
	}
	total_turn++;
	if (possibe_cost == 10) {
		return;
	}
	else {
		if (total_turn % 2 == 0) {
			game_cost = (total_turn / 2) + 1;
			possibe_cost = game_cost;
		}
	}
}

void trim(char* str, char* cpy, int cpy_len)
{
	int str_len, start_point = 0;
	int i, j;

	if (!str || !cpy)
	{
		return;
	}

	str_len = strlen(str) - 1;
	while (*(str + str_len) == ' ' || *(str + str_len) == '\t')
	{
		str_len--;
	}
	while (*(str + start_point) == ' ' || *(str + start_point) == '\t')
	{
		start_point++;
	}

	for (i = start_point, j = 0; i <= str_len && j < cpy_len; i++, j++) {
		cpy[j] = str[i];
	}
	cpy[j] = '\0';
}
