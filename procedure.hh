
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

#ifndef PROCEDURE_HH
#define PROCEDURE_HH

#include<string>
#include<map>
#include<list>
#include "basic-block.hh"           

#define PROC_SPACE "   "
#define LOC_VAR_SPACE "      "

using namespace std;

class Procedure;

class Procedure
{
	Data_Type return_type;
	string name;
	Symbol_Table local_symbol_table;
	list<Basic_Block *> basic_block_list;
	Symbol_Table local_arg_table;
	list<string> arg_string_list;

public:
	Procedure(Data_Type proc_return_type, string proc_name);
	~Procedure();

	string get_proc_name();
	void set_basic_block_list(list<Basic_Block *> bb_list);
	void set_local_list(Symbol_Table & new_list);
	void set_arg_list(Symbol_Table & new_list);
	void addArg(string arg_name);
	void set_data_type(Data_Type r_type);
	list<string> & get_arg_string_list();
	Data_Type get_return_type();
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);

	void print_ast(ostream & file_buffer);

	Basic_Block * get_next_bb(Basic_Block & current_bb);
	Basic_Block * get_jump_bb(int bb_num);
	Basic_Block & get_start_basic_block();

	Eval_Result & evaluate(ostream & file_buffer);

	bool variable_in_symbol_list_check(string variable);
	bool variable_in_arg_list_check(string variable);
	bool match_arg_types(list<Ast * > * argList);
	void match_arg_list(Symbol_Table & arg_table, int line);
	Local_Environment & get_local_env();
	Eval_Result & evaluate_in_env(ostream & file_buffer,Local_Environment & eval_env);
};

#endif
