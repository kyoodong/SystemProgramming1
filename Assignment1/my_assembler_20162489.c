/*
 * ȭ�ϸ� : my_assembler_00000000.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler_20162489.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n"); 
		return -1 ; 
	}

	if(assem_pass1() < 0 ){
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_symtab_output(/*"symtab_20162489"*/ NULL);
	if (assem_pass2() < 0) {
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_objectcode_output("output_20162489");
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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

		// ���Ŀ� ���缭 �о����
		if (fscanf(file, "%s %d %2hhx %d", p_inst->name, &p_inst->format, &p_inst->opcode, &p_inst->operandCount) != 4) {
			// ���Ŀ� ���� ���� ��� ���ݱ��� �о��� �����͸� ��� �޸� ����
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
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
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0, ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str) 
{
	token* newToken = calloc(sizeof(token), 1);

	// ���̺� ���� ���� �Ǵ�
	if (str[0] != '\t') {
		sscanf(str, "%s", newToken->label);

		// �ּ����̸� ����
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

	// �ǿ����ڸ� �Է¹ް�, ','(��ǥ)�� �����Ͽ� �������� �ǿ����ڰ� ������ ���
	// �̵� ������ operand[index]�� ������ ����
	newToken->operandCount = 1;
	while (split(newToken->operand[newToken->operandCount - 1], newToken->operand[newToken->operandCount], ',') != -1)
		newToken->operandCount++;

	token_table[token_line++] = newToken;
	return 0;
}

/*
* ���� : ���� �ּҰ��� �Ҵ���� ���� ���ͷ��� ã���ִ� �Լ��̴�.
* �Ű� : name = ���ͷ� �̸�
* ��ȯ : ( -1 ) = �ּҰ��� �Ҵ���� ���� ���ͷ��� ����
*		( n ) = ���ͷ� �ε���
*/
int search_unassigned_literal() {
	for (int i = 0; i < literalIndex; i++) {
		if (literal_table[i].addr == -1)
			return i;
	}
	return -1;
}

/*
* ���� : �ɺ� ���̺��� �ɺ��� �˻��ϴ� �Լ��̴�.
* �Ű� : name = �ɺ� �̸�
*		csect = �ɺ��� ���� control section
* ��ȯ : ( -1 ) = �ɺ� ���̺� ����
*		( n ) = �ɺ� ���̺� �� �ε���
*/
int search_symbol(const char* name, const char* csect) {
	for (int i = 0; i < symbolIndex; i++) {
		if (!strcmp(name, sym_table[i].symbol) && !strcmp(csect, sym_table[i].csect))
			return i;
	}
	return -1;
}

/*
* ���� : �ɺ� ���̺� �ɺ��� �߰����ִ� �Լ��̴�.
* �Ű� : name = �ɺ� �̸�
*		csect = �ɺ��� ���� control section
*		address = �ּҰ�
* ��ȯ : ( 1 ) = �ɺ� ���̺� �� �߰���
*		( 0 ) = �ɺ� ���̺� �߰� �� ���� �߻�
*/
int insert_symbol(const char* name, const char* csect, int address) {
	// �̹� �ִ� �ɺ�
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
* ���� : ���ڿ��� Ư�� ����(��ū)�� �������� �ڸ��� �Լ��̴�.
* �Ű� : srcStr = �ڸ� ���ڿ�. �Լ� ���� �� ��ū�� �������� ������ ��ġ�� ���ڿ��� ����ȴ�.
*		dstStr = �Լ� ���� �� ��ū�� �������� ������ ��ġ�� ���ڿ��� ����ȴ�.
*		token = ���ڿ��� �ڸ��� ������ �Ǵ� ����
* ��ȯ : ( -1 ) = ���ڿ� ���� ��ū ���ڰ� ������
*		( 0 ) = ���������� �߶󳻾��� ��
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
* ���� : �ǿ����ڸ� �о���̴� �Լ��̴�.
* �Ű� : str = ��ū(\n)���� ���е� ���ڿ�, str_for_save = ��ɾ ����� ���ڿ�
* ��ȯ : ( -1 ) = ���� �߻�
*		( 0 ) = ��������
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
* ���� : ���ڿ� ���� �� ���� �ִ� \t ���ڸ� ��� �ǳʶٱ� ���� �Լ��̴�.
* �Ű� : \t �ǳʶٱ⸦ ���ϴ� ���ڿ�
* ��ȯ : �������� = \t�� �ƴ� ���ڰ� ó������ ������ �ε���
*		\t�� ���� ��� = ( -1 )
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
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : str = ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
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

	// ã�� ����
	return -1;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
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
		// ��ɾ�
		if ((instIndex = search_opcode(t.operator)) != -1) {
			// ���ͷ�
			if (t.operand[0][0] == '=') {
				insert_literal(t.operand[0]);
			}

			// 1, 2����
			if (inst_table[instIndex]->format < 3)
				locctr += inst_table[instIndex]->format;

			// 4����
			else if (t.operator[0] == '+')
				locctr += 4;

			// 3����
			else
				locctr += 3;
		}
		// ���þ� ���� �˻�
		else if (!strcmp(t.operator, "RESW")) {
			int size = atoi(t.operand[0]);
			locctr += 3 * size;
		}
		else if (!strcmp(t.operator, "RESB")) {
			int size = atoi(t.operand[0]);
			locctr += size;
		}
		else if (!strcmp(t.operator, "BYTE")) {
			// 16����
			if (t.operand[0] == 'X') {

			}

			// TODO: locctr �� �ö󰡴°� �����ؾ���
			locctr++;
		}
		else if (!strcmp(t.operator, "WORD")) {
			// TODO: locctr �� �ö󰡴°� �����ؾ���
			locctr += 3;
		}
		else if (!strcmp(t.operator, "START")) {
			// Csect ����
			strcpy(currentCsect, t.label);

			int startAddress = atoi(t.operand[0]);
			locctr = startAddress;
		}
		else if (!strcmp(t.operator, "END")) {
			// ���α׷� ���� �ּ� ����� ���̽�
		}
		else if (!strcmp(t.operator, "BASE")) {
			// TODO: BASE ó��
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
			// �ǿ����ڸ� �Է¹ް�, ','(��ǥ)�� �����Ͽ� �������� �ǿ����ڰ� ������ ���
			// �̵� ������ operand[index]�� ������ ����
			t.operandCount = 1;
			while (split(t.operand[t.operandCount - 1], t.operand[t.operandCount], ',') != -1)
				t.operandCount++;
		}
		else if (!strcmp(t.operator, "EXTREF")) {
			// �ǿ����ڸ� �Է¹ް�, ','(��ǥ)�� �����Ͽ� �������� �ǿ����ڰ� ������ ���
			// �̵� ������ operand[index]�� ������ ����
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
		// ��ɾ, ���þ �ƴ� ��Ȳ
		else {
			// TODO: �� �� ���� Ű���� ���� ó��
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	// ǥ�� �����
	if (file_name == NULL) {
		for (int i = 0; i < symbolIndex; i++) {
			printf("%s %X\n", sym_table[i].symbol, sym_table[i].addr);
		}
		return;
	}

	// ���� �����
	FILE* file = fopen(file_name, "w");
	if (file == NULL) {
		printf("%s ���� ���� ����\n", file_name);
		return;
	}

	fclose(file);
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */

}
