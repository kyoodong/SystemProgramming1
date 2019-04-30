/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_DIRECTIVE 20

#define ERROR 987654321
#define X 0b00001000
#define E 0b00110001
#define I 0b00010000
#define N 0b00100000
#define P 0b00000010
#define DIRECT 0b00110000

int registerCount = 10;
char registers[10] = {
    'A', 'X', 'L', 'B', 'S', 'T', 'F', '?', 'P', 'W'
};

struct directive_unit {
	char name[10];
	int operandCount;
};
typedef struct directive_unit directive;
directive *directive_table[MAX_DIRECTIVE];
int directive_index;

/* 
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 라인 별로 하나의 instruction을 저장한다.
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
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;


/* 
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit {
	char label[20];
	char operator[20];
	char operand[MAX_OPERAND][20];
	char comment[100];
	int operandCount;
	int address;
	char nixbpe;
    char objectCode[10];
};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치, 심볼이 속한 컨트롤섹션으로 구성된다.
 * 추후 프로젝트에서 사용된다.
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
int skip_past_blank (const char* str);
int split(char* srcStr, char* dstStr, char token);

// 새로 정의한 함수
int read_operator(const char* str, char* str_for_save);
int search_symbol(const char* name, const char* csect);
int insert_symbol(const char* name, const char* csect, int address);
int search_unassigned_literal();
int insert_literal(const char* name);
int search_literal(const char* name);
void literal2ObjectCode(const char* literal, char* str_for_save);
void setNIXBPE(token* t);
int calculateOperandInFormat3(token* t, int pc, const char* csect);
int calculateOperandInFormat2(token* t);
void printF(const FILE* file, const char* format, ...);

char currentCsect[10];
static int objectProgramLength;

/*
 * 리터럴을 관리하는 구조체이다.
 * 리터럴 테이블은 이름, 주소값으로 구성된다.
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
