/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler_20162489.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n"); 
		return -1 ; 
	}

	if(assem_pass1() < 0 ){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_symtab_output(/*"symtab_20162489"*/ NULL);
	if (assem_pass2() < 0) {
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_objectcode_output("output_20162489");
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result ; 

	if((result = init_inst_file("inst.data")) < 0 )
		return -1 ;
	if((result = init_input_file("input.txt")) < 0 )
		return -1 ; 
	return result ; 
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE * file = fopen(inst_file, "r");
	if (file == NULL) {
		fclose(file);
		return -1;
	}

	while (!feof(file)) {
		inst* p_inst = calloc(sizeof(inst), 1);

		// 형식에 맞춰서 읽어들임
		if (fscanf(file, "%s %d %2hhx %d", p_inst->name, &p_inst->format, &p_inst->opcode, &p_inst->operandCount) != 4) {
			// 형식에 맞지 않은 경우 지금까지 읽었던 데이터를 모두 메모리 해제
			for (int i = 0; i < inst_index; i++) {
				free(inst_table[i]);
			}
			inst_index = 0;
			fclose(file);
			return -2;
		}
		inst_table[inst_index++] = p_inst;
	}
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE * file = fopen(input_file, "r");
	if (file == NULL) {
		fclose(file);
		return -1;
	}
	line_num = 0;

	while (!feof(file)) {
		char str[100] = { 0 };
		if (fscanf(file, "%[^\n]", str) == -1)
			continue;
		fgetc(file);

		char* d_str = calloc(sizeof(char) * strlen(str), 1);
		strcpy(d_str, str);
		input_data[line_num++] = d_str;
	}
	
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0, 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str) 
{
	token* newToken = calloc(sizeof(token), 1);

	// 레이블 존재 유무 판단
	if (str[0] != '\t') {
		sscanf(str, "%s", newToken->label);

		// 주석문이면 무시
		if (newToken->label[0] == '.') {
			free(newToken);
			return 0;
		}
	}

	read_operator(str, newToken->operator);

	if (read_operand(str, newToken->operand[0]) == -1) {
		token_table[token_line++] = newToken;
		return 0;
	}

	// 피연산자를 입력받고, ','(쉼표)로 구분하여 여러개의 피연산자가 지정된 경우
	// 이들 각각을 operand[index]에 나누어 저장
	newToken->operandCount = 1;
	while (split(newToken->operand[newToken->operandCount - 1], newToken->operand[newToken->operandCount], ',') != -1)
		newToken->operandCount++;

	token_table[token_line++] = newToken;
	return 0;
}

/*
* 설명 : 아직 주소값이 할당되지 않은 리터럴을 찾아주는 함수이다.
* 매개 : name = 리터럴 이름
* 반환 : ( -1 ) = 주소값이 할당되지 않은 리터럴이 없음
*		( n ) = 리터럴 인덱스
*/
int search_unassigned_literal() {
	for (int i = 0; i < literalIndex; i++) {
		if (literal_table[i].addr == -1)
			return i;
	}
	return -1;
}

/*
* 설명 : 심볼 테이블에서 심볼을 검색하는 함수이다.
* 매개 : name = 심볼 이름
*		csect = 심볼이 속한 control section
* 반환 : ( -1 ) = 심볼 테이블에 없음
*		( n ) = 심볼 테이블 내 인덱스
*/
int search_symbol(const char* name, const char* csect) {
	for (int i = 0; i < symbolIndex; i++) {
		if (!strcmp(name, sym_table[i].symbol) && !strcmp(csect, sym_table[i].csect))
			return i;
	}
	return -1;
}

/*
* 설명 : 심볼 테이블에 심볼을 추가해주는 함수이다.
* 매개 : name = 심볼 이름
*		csect = 심볼이 속한 control section
*		address = 주소값
* 반환 : ( 1 ) = 심볼 테이블에 잘 추가됨
*		( 0 ) = 심볼 테이블에 추가 중 에러 발생
*/
int insert_symbol(const char* name, const char* csect, int address) {
	// 이미 있는 심볼
	if (search_symbol(name, csect) != -1)
		return 0;

	symbol newSymbol;
	strcpy(newSymbol.csect, csect);
	strcpy(newSymbol.symbol, name);
	newSymbol.addr = address;
	sym_table[symbolIndex++] = newSymbol;
	return 1;
}

int insert_literal(const char* name) {
	if (search_literal(name) != -1)
		return -1;

	literal lit;
	strcpy(lit.name, name);
	lit.addr = -1;
	literal_table[literalIndex++] = lit;
}

int search_literal(const char* name) {
	for (int i = 0; i < literalIndex; i++) {
		if (!strcmp(name, literal_table[i].name))
			return i;
	}

	return -1;
}

/*
* 설명 : 문자열을 특정 문자(토큰)를 기준으로 자르는 함수이다.
* 매개 : srcStr = 자를 문자열. 함수 실행 후 토큰을 기준으로 좌측에 위치한 문자열이 저장된다.
*		dstStr = 함수 실행 후 토큰을 기준으로 우측에 위치한 문자열이 저장된다.
*		token = 문자열을 자르는 기준이 되는 문자
* 반환 : ( -1 ) = 문자열 내에 토큰 문자가 없을때
*		( 0 ) = 성공적으로 잘라내었을 때
*/
int split(char* srcStr, char* dstStr, char token) {
	int length = strlen(srcStr);
	int i;
	for (i = 0; srcStr[i] != token && srcStr[i] != '\0'; i++);

	if (srcStr[i] == '\0')
		return -1;

	srcStr[i] = '\0';
	int j;
	for (j = 0; i + 1 + j < length && srcStr[i + 1 + j] != '\0'; j++) {
		dstStr[j] = srcStr[i + 1 + j];
	}
	dstStr[j] = '\0';
	return 0;
}

/*
* 설명 : 피연산자를 읽어들이는 함수이다.
* 매개 : str = 토큰(\n)으로 구분된 문자열, str_for_save = 명령어가 저장될 문자열
* 반환 : ( -1 ) = 오류 발생
*		( 0 ) = 정상종료
*/
int read_operand(const char* str, char* str_for_save) {
	int skip_index = skip_past_blank(str);
	if (skip_index == -1)
		return -1;

	str += skip_index;

	skip_index = skip_past_blank(str);
	if (skip_index == -1)
		return -1;

	str += skip_index;
	sscanf(str, "%s", str_for_save);
	return 0;
}

/*
* 설명 : 문자열 왼쪽 맨 끝에 있는 \t 문자를 모두 건너뛰기 위한 함수이다.
* 매개 : \t 건너뛰기를 원하는 문자열
* 반환 : 정상종료 = \t가 아닌 문자가 처음으로 나오는 인덱스
*		\t가 없는 경우 = ( -1 )
*/
int skip_past_blank(const char* str) {
	int count = 0;
	while (str[count] != '\t') {
		if (str[count] == '\n' || str[count] == '\0')
			return -1;

		count++;
	}
	return count + 1;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매개 : str = 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str) 
{
	char op[20];
	sscanf(str, "%s", op);
	int format = 0;
	int index = 0;
	if (str[0] == '+') {
		format = 4;
		index++;
	}

	for (int i = 0; i < inst_index; i++) {
		if (!strcmp(inst_table[i]->name, op + index)) {
			if (format == 4) {
				if (inst_table[i]->format == 3) {
					return i;
				}
			}
			else {
				return i;
			}
		}
	}

	// 찾지 못함
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* input_data의 문자열을 한줄씩 입력 받아서
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
	for (int i = 0; i < line_num; i++) {
		if (token_parsing(input_data[i]) < 0)
			return -1;
	}

	for (int i = 0; i < token_line; i++) {
		token t = *token_table[i];

		if (strlen(t.label) > 0 && strcmp(t.operator, "CSECT")) {
			if (!insert_symbol(t.label, currentCsect, locctr))
				return -1;
		}

		int instIndex = -1;
		// 명령어
		if ((instIndex = search_opcode(t.operator)) != -1) {
			// 리터럴
			if (t.operand[0][0] == '=') {
				insert_literal(t.operand[0]);
			}

			// 1, 2형식
			if (inst_table[instIndex]->format < 3)
				locctr += inst_table[instIndex]->format;

			// 4형식
			else if (t.operator[0] == '+')
				locctr += 4;

			// 3형식
			else
				locctr += 3;
		}
		// 지시어 여부 검사
		else if (!strcmp(t.operator, "RESW")) {
			int size = atoi(t.operand[0]);
			locctr += 3 * size;
		}
		else if (!strcmp(t.operator, "RESB")) {
			int size = atoi(t.operand[0]);
			locctr += size;
		}
		else if (!strcmp(t.operator, "BYTE")) {
			// 16진수
			if (t.operand[0] == 'X') {

			}

			// TODO: locctr 값 올라가는거 수정해야함
			locctr++;
		}
		else if (!strcmp(t.operator, "WORD")) {
			// TODO: locctr 값 올라가는거 수정해야함
			locctr += 3;
		}
		else if (!strcmp(t.operator, "START")) {
			// Csect 설정
			strcpy(currentCsect, t.label);

			int startAddress = atoi(t.operand[0]);
			locctr = startAddress;
		}
		else if (!strcmp(t.operator, "END")) {
			// 프로그램 시작 주소 명시한 케이스
		}
		else if (!strcmp(t.operator, "BASE")) {
			// TODO: BASE 처리
			int startAddress = atoi(t.operand[0]);
			locctr = startAddress;
		}
		else if (!strcmp(t.operator, "EQU")) {

		}
		else if (!strcmp(t.operator, "ORG")) {

		}
		else if (!strcmp(t.operator, "LTORG")) {
			int index;
			while ((index = search_unassigned_literal()) != -1) {
				literal_table[index].addr = locctr;

				if (literal_table[index].name[1] == 'C') {
					int size = 0;

					for (int t = 3; literal_table[index].name[t] != '\''; t++, size++);
					locctr += size;
				}
				else if (literal_table[index].name[1] == 'X') {
					int size = 0;

					for (int t = 3; literal_table[index].name[t] != '\''; t++, size++);
					locctr += size / 2;
				}
			}
		}
		else if (!strcmp(t.operator, "EXTDEF")) {
			// 피연산자를 입력받고, ','(쉼표)로 구분하여 여러개의 피연산자가 지정된 경우
			// 이들 각각을 operand[index]에 나누어 저장
			t.operandCount = 1;
			while (split(t.operand[t.operandCount - 1], t.operand[t.operandCount], ',') != -1)
				t.operandCount++;
		}
		else if (!strcmp(t.operator, "EXTREF")) {
			// 피연산자를 입력받고, ','(쉼표)로 구분하여 여러개의 피연산자가 지정된 경우
			// 이들 각각을 operand[index]에 나누어 저장
			t.operandCount = 1;
			while (split(t.operand[t.operandCount - 1], t.operand[t.operandCount], ',') != -1)
				t.operandCount++;
		}
		else if (!strcmp(t.operator, "NOBASE")) {

		}
		else if (!strcmp(t.operator, "USE")) {

		}
		else if (!strcmp(t.operator, "CSECT")) {
			strcpy(currentCsect, t.label);
			objectProgramLength += locctr;
			locctr = 0;

			if (!insert_symbol(t.label, currentCsect, locctr))
				return -1;
		}
		// 명령어도, 지시어도 아닌 상황
		else {
			// TODO: 알 수 없는 키워드 에러 처리
		}
	}
	return 0;
}

int read_operator(const char* str, char* str_for_save) {
	int skip_index = skip_past_blank(str);
	if (skip_index == -1)
		return -1;

	str += skip_index;
	sscanf(str, "%s", str_for_save);
	return 0;
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	// 표준 입출력
	if (file_name == NULL) {
		for (int i = 0; i < symbolIndex; i++) {
			printf("%s %X\n", sym_table[i].symbol, sym_table[i].addr);
		}
		return;
	}

	// 파일 입출력
	FILE* file = fopen(file_name, "w");
	if (file == NULL) {
		printf("%s 파일 생성 에러\n", file_name);
		return;
	}

	fclose(file);
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */

}
