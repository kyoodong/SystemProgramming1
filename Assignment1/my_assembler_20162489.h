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
	int operandCount;
	char nixbpe;
};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ, �ɺ��� ���� ��Ʈ�Ѽ������� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
	char csect[10];
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int symbolIndex;

static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);

int read_operand(const char* str, char* str_for_save);
int skip_past_blank (char* str);
int split(char* srcStr, char* dstStr, char token);

// ���� ������ �Լ�
int read_operator(const char* str, char* str_for_save);
int search_symbol(const char* name, const char* csect);
int insert_symbol(const char* name, const char* csect, int address);
int search_unassigned_literal();
int insert_literal(const char* name);
int search_literal(const char* name);

char currentCsect[10];
static int objectProgramLength;

/*
 * ���ͷ��� �����ϴ� ����ü�̴�.
 * ���ͷ� ���̺��� �̸�, �ּҰ����� �����ȴ�.
 */
struct literal_unit {
	char name[10];
	int addr;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];
static int literalIndex;

///////
void make_symtab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);