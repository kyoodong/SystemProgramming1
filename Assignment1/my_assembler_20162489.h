/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_DIRECTIVE 20

struct directive_unit {
	char name[10];
	int operandCount;
};
typedef struct directive_unit directive;
directive *directive_table[MAX_DIRECTIVE];
int directive_index;

/* 
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit {
	char name[10];
	unsigned char opcode;
	int format;
	int operandCount;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;


/* 
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit {
	char label[20];
	char operator[20];
	char operand[MAX_OPERAND][20];
	char comment[100];
	int instIndex;
	int directiveIndex;
	int operandCount;
	//char nixbpe; // ���� ������Ʈ���� ���ȴ�.
};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str, char* op);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

// ���� ������ �Լ�
int init_directive_file(char *directive_file);
int search_directive(char *str);
int read_operand(const char* str, char* str_for_save);
int skip_past_blank (char* str);
int is_equal_string(const char* str1, const char* str2);
int split(char* srcStr, char* dstStr, char token);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
void make_symtab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);