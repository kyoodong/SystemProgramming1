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

	make_symtab_output("symtab_20162489");
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

	if ((result = init_directive_file("directive.data")) < 0)
		return -1;
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
 * ���� : ����� ���þ� ��� ������ �о� ���þ� ��� ���̺�(directive_table)�� �����ϴ� �Լ��̴�.
 * �Ű� : ���þ� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 */
int init_directive_file(char *directive_file)
{
	FILE * file = fopen(directive_file, "r");
	if (file == NULL) {
		fclose(file);
		return -1;
	}

	while (!feof(file)) {
		directive* p_directive = calloc(sizeof(directive), 1);

		// ���Ŀ� ���缭 �о����
		if (fscanf(file, "%s %d", p_directive->name, &p_directive->operandCount) != 2) {
			// ���Ŀ� ���� ���� ��� ���ݱ��� �о��� �����͸� ��� �޸� ����
			for (int i = 0; i < directive_index; i++) {
				free(directive_table[i]);
			}
			directive_index = 0;
			fclose(file);
			return -2;
		}
		directive_table[directive_index++] = p_directive;
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
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str) 
{
	token* newToken = calloc(sizeof(token), 1);

	// ���̺� ���� ���� �Ǵ�
	if (str[0] != '\t') {
		sscanf(str, "%s", newToken->label);

		// �ɺ� �߰�
		symbol newSymbol;
		strcpy()
		strcpy(newSymbol.symbol, newToken->label);
		newSymbol.addr = locctr;
		sym_table[symbolIndex++] = newSymbol;

		// �ּ����̸� ����
		if (newToken->label[0] == '.') {
			free(newToken);
			return 0;
		}
	}

	// ��ɾ� ���� �˻�
	newToken->instIndex = search_opcode(str, newToken->operator);
	if (newToken->instIndex >= 0) {
		// ��ɾ �����Ƿ� ���þ �ƴ��� ǥ���ϱ� ���� directiveIndex�� -1�� �ʱ�ȭ
		newToken->directiveIndex = -1;

		// �ǿ����ڸ� �Է¹޴� ���������� üũ
		if (inst_table[newToken->instIndex]->operandCount > 0) {
			if (read_operand(str, newToken->operand[0]) == -1)
				return -1;

			// �ǿ����ڸ� �Է¹ް�, ','(��ǥ)�� �����Ͽ� �������� �ǿ����ڰ� ������ ���
			// �̵� ������ operand[index]�� ������ ����
			newToken->operandCount = 1;
			while (split(newToken->operand[newToken->operandCount - 1], newToken->operand[newToken->operandCount], ',') != -1)
				newToken->operandCount++;
		}

		// 1, 2����
		if (inst_table[newToken->instIndex]->format < 3)
			locctr += inst_table[newToken->instIndex]->format;

		// 4����
		else if (newToken->operator[0] == '+')
			locctr += 4;

		// 3����
		else
			locctr += 3;
	}

	// ��ɾ �ƴ�
	else {
		// ���þ� ���� �˻�
		int directive_index = search_directive(str);
		if (directive_index >= 0) {
			// ���þ �����Ƿ� ��ɾ �ƴ��� ǥ���ϱ� ���� instIndex�� -1�� �ʱ�ȭ
			newToken->instIndex = -1;
			strcpy(newToken->operator, directive_table[directive_index]->name);

			// �ǿ����ڸ� �Է¹޴� ���þ����� üũ
			if (directive_table[directive_index]->operandCount > 0) {
				if (read_operand(str, newToken->operand[0]) == -1)
					return -1;

				// �ǿ����ڸ� �Է¹ް�, ','(��ǥ)�� �����Ͽ� �������� �ǿ����ڰ� ������ ���
				// �̵� ������ operand[index]�� ������ ����
				newToken->operandCount = 1;
				while (split(newToken->operand[newToken->operandCount - 1], newToken->operand[newToken->operandCount], ',') != -1)
					newToken->operandCount++;
			}
		}
		// ��ɾ, ���þ �ƴ� ��Ȳ
		else {
			// TODO: �� �� ���� Ű���� ���� ó��
		}
	}
	token_table[token_line++] = newToken;
	return 0;
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
int skip_past_blank(char* str) {
	int count = 0;
	while (str[count] != '\t') {
		if (str[count] == '\n' || str[count] == '\0')
			return -1;

		count++;
	}
	return count + 1;
}

/*
* ���� : �� ���ڿ��� ������ ���ϴ� �Լ��̴�.
* �Ű� : ���ϰ� ���� ���ڿ�
* ��ȯ : ���� = 1, �ٸ� = 0
*/
int is_equal_string(const char* str1, const char* str2) {
	int length = strlen(str1);
	if (length != strlen(str2))
		return 0;

	for (int i = 0; i < length; i++) {
		if (str1[i] != str2[i])
			return 0;
	}
	return 1;
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : str = ��ū ������ ���е� ���ڿ�, op = ���� �ڵ带 ������ ���ڿ�(NULL ����)
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str, char* op) 
{
	if (op == NULL) {
		char new_op[20];
		op = new_op;
	}

	int skipIndex = skip_past_blank(str);
	if (skipIndex == -1)
		return -1;

	str += skipIndex;

	sscanf(str, "%s", op);
	int format = 0;
	if (str[0] == '+') {
		format = 4;
		op++;
	}

	for (int i = 0; i < inst_index; i++) {
		if (is_equal_string(inst_table[i]->name, op)) {
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
 * ���� : �Է� ���ڿ��� ����� ���þ������� �˻��ϴ� �Լ��̴�.
 * �Ű� : ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���þ� ���̺� �ε���, ���þ� �ƴ� = -1, ���� < -1
 * ���� :
 *
 * ----------------------------------------------------------------------------------
 */
int search_directive(char *str)
{
	// ���þ� ��ġ���� ������ �̵�
	int skipIndex = skip_past_blank(str);
	if (skipIndex == -1)
		return -1;
	str += skipIndex;

	// ���þ� ���� üũ
	char directive[20];
	sscanf(str, "%s", directive);
	for (int i = 0; i < directive_index; i++) {
		if (is_equal_string(directive_table[i]->name, directive)) {
			return i;
		}
	}
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
	return 0;
}


/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	// ǥ�� ���
	if (file_name == NULL) {
		for (int i = 0; i < token_line; i++) {
			printf("%s\t%s\t%s", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0]);
			for (int j = 1; j < token_table[i]->operandCount; j++)
				printf(",%s", token_table[i]->operand[j]);
			if (token_table[i]->instIndex >= 0)
				printf("\t%02X", inst_table[token_table[i]->instIndex]->opcode);
			printf("\n");
		}
		return;
	}

	// ���� ���
	FILE* file = fopen(file_name, "w");
	if (file == NULL) {
		fclose(file);
		return;
	}

	for (int i = 0; i < token_line; i++) {
		fprintf(file, "%s\t%s\t%s", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0]);
		for (int j = 1; j < token_table[i]->operandCount; j++)
			fprintf(file, ",%s", token_table[i]->operand[j]);
		if (token_table[i]->instIndex >= 0)
			fprintf(file, "\t%02X", inst_table[token_table[i]->instIndex]->opcode);
		fprintf(file, "\n");
	}
	fclose(file);
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
