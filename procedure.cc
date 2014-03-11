
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

void Procedure::match_arg_list(Symbol_Table & arg_table, int line){
	list<Symbol_Table_Entry *> & t1 = local_arg_table.get_var_table();
	list<Symbol_Table_Entry *> & t2 = arg_table.get_var_table();

	if(t1.size() != t2.size()){
		report_error("Formal parameter list of the procedure and its prototype should match", line);
	}

	list<Symbol_Table_Entry *>::iterator it1;
	list<Symbol_Table_Entry *>::iterator it2;

	for(it1 = t1.begin(), it2 = t2.begin(); it1 != t1.end(); it1++,it2++){
		if((*it1)->get_data_type() != (*it2)->get_data_type()){
			report_error("Return type of one of the parameters of the procedure and its prototype doesn't match",line);
		}
		if((*it1)->get_variable_name() != (*it2)->get_variable_name()){
			report_error("Variable name of one of the parameters of the procedre and its prototypes doesn't match",line);
		}
	}
}

list<string> & Procedure::get_arg_string_list(){
	return arg_string_list;
}

bool Procedure::match_arg_types(list<Ast * > * argList){
	if(argList->size() == 0) return true;
	list<Ast*>:: iterator it;
	list<string>:: iterator it_1 = arg_string_list.begin();
	int i = 0;
	for(it = argList->begin(); it != argList->end(); it++){
		if((*it)->get_data_type() != local_arg_table.get_symbol_table_entry(*it_1).get_data_type()) return false;
		it_1++;
	}
	return true;
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
		if(result->get_result_enum() == skip_result || result->get_result_enum() == skip_result_val) break;
		if(result->get_result_enum() == goto_result){
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
	

	if(result->get_result_enum() == skip_result_val){
		
	}
	else{
		if(result->get_result_enum() == skip_result){
			if(return_type == int_data_type){
				Eval_Result_Value_Int ret_val;
				ret_val.set_value(result->get_value());
				eval_env.put_variable_value(ret_val,"return");
			}
			else if(return_type == float_data_type){
				Eval_Result_Value_Float ret_val;
				ret_val.set_value(result->get_value());
				eval_env.put_variable_value(ret_val,"return");
			}
		}
		else if(return_type == int_data_type){
			Eval_Result_Value_Int ret_val;
			ret_val.set_value(result->get_value());
			eval_env.put_variable_value(ret_val,"return");
		}
		else if(return_type == float_data_type){
			Eval_Result_Value_Float ret_val;
			ret_val.set_value(result->get_value());
			eval_env.put_variable_value(ret_val,"return");
		}
	}
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
		if(result->get_result_enum() == skip_result || result->get_result_enum() == skip_result_val) {
			
			break;
		}
		if(result->get_result_enum() == goto_result){
			current_bb = get_jump_bb(result->get_value().int_val);
			continue;
		}
		current_bb = get_next_bb(*current_bb);		
	}

	list<Basic_Block *>::reverse_iterator last;
	last = basic_block_list.rbegin();
	(*last)->successor_found();

	file_buffer << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << "<<name<<" >>\n";
	/*check for return result and function prototype datatype match*/
	if(result->get_result_enum() == skip_result_val){
		
	}
	else{
		if(result->get_result_enum() == skip_result){

			if(return_type == int_data_type){
				Eval_Result_Value_Int ret_val;
				ret_val.set_value(result->get_value());
				eval_env.put_variable_value(ret_val,"return");
			}
			else if(return_type == float_data_type){
				Eval_Result_Value_Float ret_val;
				ret_val.set_value(result->get_value());
				eval_env.put_variable_value(ret_val,"return");
			}
		}
		else if(return_type == int_data_type){
			Eval_Result_Value_Int ret_val;
			ret_val.set_value(result->get_value());
			eval_env.put_variable_value(ret_val,"return");
		}
		else if(return_type == float_data_type){
			Eval_Result_Value_Float ret_val;
			ret_val.set_value(result->get_value());
			eval_env.put_variable_value(ret_val,"return");
		}
	}
	eval_env.print(file_buffer);
	return *result;
}

