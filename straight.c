
/*
 * modern compiler impl in C -- Data Structs for Tree Langs
 * straight-line prog lang
 * whitespace only serves as separator
 * Stm -- Stm; Stm		(CompoundStm)
 * Stm -- id := Exp		(AssignStm)
 * Stm -- print(ExpList)	(PrintStm)
 * Exp -- id			(IdExp)
 * Exp -- num			(NumExp)
 * Exp -- Exp Binop Exp		(BinopExp)
 * Exp -- (Stm, Exp)		(EseqExp)
 * ExpList -- Exp, ExpList	(PairExpList)
 * ExpList -- Exp		(LastExpList)
 * Binop -- +			(Plus)
 * Binop -- -			(Minus)
 * Binop -- *			(Times)
 * Binop -- /			(Div)
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct stm Stm;
typedef struct exp Exp;
typedef struct explist ExpList;

struct stm {
	enum { COMPOUND_STM, ASSIGN_STM, PRINT_STM } type;
	union {
		struct {
			Stm* left;
			Stm* right;
		} compound;
		struct {
			char* id;
			Exp* right;
		} assign;
		struct {
			ExpList* exps;
		} print;
	} u;
};

typedef enum { ADD, SUB, MUL, DIV } BinOp;

struct exp {
	enum { ID_EXP, NUM_EXP, BINOP_EXP, ESEQ_EXP } type;
	union {
		char* id;
		int num;
		struct {
			Exp* left;
			BinOp op;
			Exp* right;
		} binop;
		struct {
			Stm* stm;
			Exp* exp;
		} eseq;
	} u;
};

struct explist {
	enum { PAIR_EXP_LIST, LAST_EXP_LIST } type;
	union {
		struct {
			Exp* head;
			ExpList* tail;
		} pair;
		Exp* last;
	} u;
};

typedef struct table Table;
struct table {char *id; int val; Table *tail};

Table *update_table(Table *t, char *key, int val);
int table_lookup(Table *t, char *key);

Stm* mk_compound_stm(Stm* left, Stm* right);
Stm* mk_assign_stm(char* id, Exp* right);
Stm* mk_print_stm(ExpList* exps);

Exp* mk_id_exp(char* id);
Exp* mk_num_exp(int num);
Exp* mk_binop_exp(Exp* left, BinOp op, Exp* right);
Exp* mk_eseq_exp(Stm* stm, Exp* exp);

ExpList* mk_pair_explist(Exp* head, ExpList* tail);
ExpList* mk_last_explist(Exp* last);


static int maxargs_exp(Exp* e);
static int maxargs(Stm* s);

static int max(int a, int b)
{
	return a > b ? a : b;
}

static int maxargs_exp(Exp* e)
{
	switch (e->type) {
	case ESEQ_EXP:
		return max(maxargs(e->u.eseq.stm), maxargs_exp(e->u.eseq.exp));
	case BINOP_EXP:
		return max(maxargs_exp(e->u.binop.left),
			   maxargs_exp(e->u.binop.right));

	default:
		return 0;
	}
}

static int maxargs(Stm* s)
{
	int c = 0, m = 0;
	ExpList* e;
	switch (s->type) {
	case COMPOUND_STM:
		return max(maxargs(s->u.compound.left),
			   maxargs(s->u.compound.right));
	case ASSIGN_STM:
		return maxargs_exp(s->u.assign.right);
	case PRINT_STM:
		for (e = s->u.print.exps; e->type == PAIR_EXP_LIST;
		     e = e->u.pair.tail) {
			c++;
			m = max(maxargs_exp(e->u.pair.head), m);
		}
		c++;
		m = max(maxargs_exp(e->u.pair.head), m);
	}
	return max(m, c);
}

Table *interp_exp(Exp *e)
{
	
}

Table *interp_stm(Stm *s)
{


}

int main(void)
{
	Stm* prog = mk_compound_stm(
		mk_assign_stm("a",
			      mk_binop_exp(mk_num_exp(5), ADD, mk_num_exp(3))),
		mk_compound_stm(
			mk_assign_stm(
				"b",
				mk_eseq_exp(mk_print_stm(mk_pair_explist(
						    mk_id_exp("a"),
						    mk_last_explist(mk_binop_exp(
							    mk_id_exp("a"), SUB,
							    mk_num_exp(1))))),
					    mk_binop_exp(mk_num_exp(10), MUL,
							 mk_id_exp("b")))),
			mk_print_stm(mk_last_explist(mk_id_exp("b")))));
	printf("%d\n", maxargs(prog));
	return 0;
}

Stm* mk_compound_stm(Stm* left, Stm* right)
{
	Stm* s = malloc(sizeof(*s));
	s->type = COMPOUND_STM;
	s->u.compound.left = left;
	s->u.compound.right = right;
	return s;
}

Stm* mk_assign_stm(char* id, Exp* right)
{
	Stm* s = malloc(sizeof(*s));
	s->type = ASSIGN_STM;
	s->u.assign.id = id;
	s->u.assign.right = right;
	return s;
}

Stm* mk_print_stm(ExpList* exps)
{
	Stm* s = malloc(sizeof(*s));
	s->type = PRINT_STM;
	s->u.print.exps = exps;
	return s;
}

Exp* mk_id_exp(char* id)
{
	Exp* e = malloc(sizeof(*e));
	e->type = ID_EXP;
	e->u.id = id;
	return e;
}

Exp* mk_num_exp(int num)
{
	Exp* e = malloc(sizeof(*e));
	e->type = NUM_EXP;
	e->u.num = num;
	return e;
}

Exp* mk_binop_exp(Exp* left, BinOp op, Exp* right)
{
	Exp* e = malloc(sizeof(*e));
	e->type = BINOP_EXP;
	e->u.binop.left = left;
	e->u.binop.op = op;
	e->u.binop.right = right;
	return e;
}

Exp* mk_eseq_exp(Stm* stm, Exp* exp)
{
	Exp* e = malloc(sizeof(*e));
	e->type = ESEQ_EXP;
	e->u.eseq.stm = stm;
	e->u.eseq.exp = exp;
	return e;
}

ExpList* mk_pair_explist(Exp* head, ExpList* tail)
{
	ExpList* e = malloc(sizeof(*e));
	e->type = PAIR_EXP_LIST;
	e->u.pair.head = head;
	e->u.pair.tail = tail;
	return e;
}

ExpList* mk_last_explist(Exp* last)
{
	ExpList* e = malloc(sizeof(*e));
	e->type = LAST_EXP_LIST;
	e->u.last = last;
	return e;
}

/* retarded table impl */
Table *update_table(Table *t, char *key, int val)
{
	Table *t2;
	for (t2 = t; t2; t2 = t2->tail) {
		if (!strcmp(t->id, key)) {
			t->val = val;
			return t;
		}
	}
	t2 = malloc(sizeof(*t2));
	t2->id = key;
	t2->val = val;
	t2->tail = t;
	return t2;
}

int table_lookup(Table *t, char *key)
{
	for (; t; t = t->tail) {
		if (!strcmp(t->id, key)) {
			return t->val;
		}
	}
	return 0;
}