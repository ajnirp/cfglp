
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#ifndef AST_HH
#define AST_HH

#include<string>
#include<iostream>
#include<iomanip>
#include<typeinfo>
#include<list>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_SUB_NODE_SPACE "               "

using namespace std;

typedef enum
{
	NE_OP,GE_OP,LE_OP,EQ_OP,LT_OP,GT_OP
} Comparison_Op_Enum;

typedef enum
{
	PLUS_OP, MINUS_OP, DIV_OP, MUL_OP
} Arith_Op_Enum;

class Ast;

class Ast
{
protected:
	typedef enum
	{
		zero_arity = 0,
		unary_arity = 1,
		binary_arity = 2
	}Ast_Arity;

	Data_Type node_data_type;
	Ast_Arity ast_num_child;

	int lineno;

public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast();
	virtual Symbol_Table_Entry & get_symbol_entry();

	virtual void print(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;

	virtual Code_For_Ast & compile() = 0;
	virtual Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra) = 0;
	virtual Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Assignment_Ast();

	// Data_Type get_data_type(); // submission 5a
	bool check_ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Name_Ast:public Ast
{
	Symbol_Table_Entry * variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line);
	~Name_Ast();

	Data_Type get_data_type();
	Symbol_Table_Entry & get_symbol_entry();

	void print(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
	Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type, int line);
	~Number_Ast();

	Data_Type get_data_type();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Return_Ast:public Ast
{

public:
	Return_Ast(int line);
	~Return_Ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

// submission 5a
class Comparison_Ast:public Ast
{
	Ast * lhs;
	Comparison_Op_Enum op;
	Ast * rhs;

public:
	Comparison_Ast(Ast * temp_lhs, Comparison_Op_Enum temp_op, Ast * temp_rhs, int line);
	~Comparison_Ast();

	Data_Type get_data_type();
	bool check_ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
	Code_For_Ast & create_set_stmt(Register_Descriptor* reg1, Register_Descriptor* reg2);
};

class Goto_Ast:public Ast
{
	int bb_number;

public:
	Goto_Ast(int temp_bb_number, int line);
	~Goto_Ast();

	Data_Type get_data_type();
	bool successor_found();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class If_Ast:public Ast
{
	Ast* condition;
	int true_bb_number;
	int false_bb_number;

public:
	If_Ast(Ast* temp_condition ,int temp_true_bb_number, int temp_false_bb_number, int line);
	~If_Ast();

	Data_Type get_data_type();
	bool successor_found();
	void print(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Arithmetic_Ast:public Ast
{
	Ast * lhs;
	Arith_Op_Enum op;
	Ast * rhs;

public:
	Arithmetic_Ast(Ast * temp_lhs, Arith_Op_Enum temp_op, Ast * temp_rhs,int line);
	~Arithmetic_Ast();

	Data_Type get_data_type();
	bool check_ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Typecast_Ast:public Ast
{
	Data_Type dtype;
	Ast * ast;

public:
	Typecast_Ast(Data_Type dt, Ast * a);
	~Typecast_Ast();

	Data_Type get_data_type();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class UnaryMinus_Ast:public Ast
{
	Ast * ast;

public:
	UnaryMinus_Ast(Ast * a);
	~UnaryMinus_Ast();

	Data_Type get_data_type();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

#endif