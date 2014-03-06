
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

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "error-display.hh"
#include "local-environment.hh"
#include "symbol-table.hh"
#include "basic-block.hh"
#include "procedure.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name)
{
	return_type = proc_return_type;
	name = proc_name;
	arg_string_list = *(new list<string>);
	local_arg_table = *(new Symbol_Table());
}

void Procedure::set_data_type(Data_Type r_type){
	return_type = r_type;
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}

void Procedure::set_local_list(Symbol_Table & new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}

void Procedure::set_arg_list(Symbol_Table & new_list)
{
	local_arg_table = new_list;
	local_arg_table.set_table_scope(local);
}

list<string> & Procedure::get_arg_string_list(){
	return arg_string_list;
}

void Procedure::addArg(string arg_name){
	
	// list<string> li;
	// cout<<arg_string_list.size()<<endl;
	// cout<<arg_string_list.size()<<endl;
	list<string>::iterator it;
	for(it = arg_string_list.begin(); it!=arg_string_list.end(); it++){
		if(*it == arg_name) return;
	}
	// cout<<arg_name<<endl;
	arg_string_list.push_back(arg_name);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return (local_symbol_table.variable_in_symbol_list_check(variable)||local_arg_table.variable_in_symbol_list_check(variable));
}

bool Procedure::variable_in_arg_list_check(string variable)
{
	return local_arg_table.variable_in_symbol_list_check(variable);
}


Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{	
	if(local_symbol_table.variable_in_symbol_list_check(variable_name)){
		return local_symbol_table.get_symbol_table_entry(variable_name);	
	}
	else{
		return local_arg_table.get_symbol_table_entry(variable_name);	
	}
	
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: "<<name<< "\n";

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++){
		(*i)->print_bb(file_buffer);
	}
	list<Basic_Block *>::reverse_iterator last;
	last = basic_block_list.rbegin();
	// cout<<"PRININTG"<<endl;
	(*last)->successor_found();
}
	
Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}

Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == current_bb.get_bb_number())
		{
			flag = true;
			continue;
		}
		if (flag)
			return (*i);
	}
	
	return NULL;
}

Basic_Block * Procedure::get_jump_bb(int bb_num)
{
	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == bb_num)
		{
			return (*i);
		}
	}
	return NULL;
}

Eval_Result & Procedure::evaluate(ostream & file_buffer)
{
	Local_Environment & eval_env = *new Local_Environment();
	local_arg_table.create(eval_env);
	local_symbol_table.create(eval_env);
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure << "<<name<<" >>\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		//TODO_DONE
		if(result->get_result_enum() == skip_result){
			if(result->get_value().int_val == -1) break;
			current_bb = get_jump_bb(result->get_value().int_val);
			continue;
		}
		current_bb = get_next_bb(*current_bb);		
	}

	list<Basic_Block *>::reverse_iterator last;
	last = basic_block_list.rbegin();
	(*last)->successor_found();

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << "<<name<<" >>\n";
	eval_env.print(file_buffer);

	return *result;
}

Local_Environment & Procedure::get_local_env(){
	Local_Environment & eval_env = *new Local_Environment();
	local_arg_table.create(eval_env);
	local_symbol_table.create(eval_env);
	return eval_env;
}

Eval_Result & Procedure::evaluate_in_env(ostream & file_buffer,Local_Environment & eval_env)
{	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure << "<<name<<" >>\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		//TODO_DONE
		if(result->get_result_enum() == skip_result){
			if(result->get_value().int_val == -1) break;
			current_bb = get_jump_bb(result->get_value().int_val);
			continue;
		}
		current_bb = get_next_bb(*current_bb);		
	}

	list<Basic_Block *>::reverse_iterator last;
	last = basic_block_list.rbegin();
	(*last)->successor_found();

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << "<<name<<" >>\n";
	eval_env.print(file_buffer);
	if(return_type == int_data_type){
		file_buffer << LOC_VAR_SPACE<<PROC_SPACE<<"return : "<<result->get_value().int_val<<"\n";
	}
	else{
		file_buffer << LOC_VAR_SPACE<<PROC_SPACE<<"return : "<<result->get_value().float_val<<"\n";
	}
	

	return *result;
}

