
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

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "

using namespace std;


//TODO_DONE
typedef enum
{
	NE_OP,GE_OP,LE_OP,EQ_OP,LT_OP,GT_OP
} Comparison_Op_Enum;

//level2
typedef enum
{
	PLUS_OP, MINUS_OP, DIV_OP, MUL_OP
} Arith_Op_Enum;

class Ast;

class Ast
{
protected:
	Data_Type node_data_type;
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual bool successor_found();
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Return_Ast:public Ast
{
	Ast * ret_ast;
public:
	Return_Ast(Ast * r_ast);
	~Return_Ast();

	void print_ast(ostream & file_buffer);
	bool successor_found();
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

//TODO_DONE

class Comparison_Ast:public Ast
{
	Ast * lhs;
	Comparison_Op_Enum op;
	Ast * rhs;

public:
	Comparison_Ast(Ast * temp_lhs, Comparison_Op_Enum temp_op, Ast * temp_rhs);
	~Comparison_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Goto_Ast:public Ast
{
	int bb_number;

public:
	Goto_Ast(int temp_bb_number);
	~Goto_Ast();

	Data_Type get_data_type();
	bool successor_found();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class If_Ast:public Ast
{
	Ast* condition;
	int true_bb_number;
	int false_bb_number;

public:
	If_Ast(Ast* temp_condition ,int temp_true_bb_number, int temp_false_bb_number);
	~If_Ast();

	Data_Type get_data_type();
	bool successor_found();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

// submission 3b
class Arithmetic_Ast:public Ast
{
	Ast * lhs;
	Arith_Op_Enum op;
	Ast * rhs;

public:
	Arithmetic_Ast(Ast * temp_lhs, Arith_Op_Enum temp_op, Ast * temp_rhs);
	~Arithmetic_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

// submission 3b
class Typecast_Ast:public Ast
{
	Data_Type dtype;
	Ast * ast;

public:
	Typecast_Ast(Data_Type dt, Ast * a);
	~Typecast_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class UnaryMinus_Ast:public Ast
{
	Ast * ast;

public:
	UnaryMinus_Ast(Ast * a);
	~UnaryMinus_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class function_call_Ast:public Ast
{
	string fn_name;
	list<Ast *> argList;

public:
	function_call_Ast(string fName, list<Ast *> * AList);
	~function_call_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

#endif