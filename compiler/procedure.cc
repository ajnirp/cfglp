
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

#include<string>
#include<fstream>
#include<iostream>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name, int line)
{
	return_type = proc_return_type;
	name = proc_name;
	arg_string_list = *(new list<string>);
	local_arg_table = *(new Symbol_Table());
	defined = false;
	lineno = line;
	compiled = false;
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

void Procedure::set_data_type(Data_Type r_type){
	return_type = r_type;
}

void Procedure::set_defined(){
	defined = true;
}

bool Procedure::is_defined(){
	return defined;
}

void Procedure::set_basic_block_list(list<Basic_Block *> & bb_list)
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
		CHECK_INVARIANT(false , "Formal parameter list of the procedure and its prototype should match");
	}

	list<Symbol_Table_Entry *>::iterator it1;
	list<Symbol_Table_Entry *>::iterator it2;

	for(it1 = t1.begin(), it2 = t2.begin(); it1 != t1.end(); it1++,it2++){
		if((*it1)->get_data_type() != (*it2)->get_data_type()){
			CHECK_INVARIANT(false, "Return type of one of the parameters of the procedure and its prototype doesn't match");
		}
		if((*it1)->get_variable_name() != (*it2)->get_variable_name()){
			CHECK_INVARIANT(false, "Variable name of one of the parameters of the procedre and its prototypes doesn't match");
		}
	}
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
	// cout<<arg_name<<" ADDED "<<endl;
	arg_string_list.push_back(arg_name);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	// return local_symbol_table.get_symbol_table_entry(variable_name);
	if(local_symbol_table.variable_in_symbol_list_check(variable_name)){
		// cout<<"YAHA AATA "<<variable_name<<endl;
		return local_symbol_table.get_symbol_table_entry(variable_name);	
	}
	else{

		return local_arg_table.get_symbol_table_entry(variable_name);	
	}
}

Symbol_Table_Entry & Procedure::get_arg_symbol_table_entry(string variable_name)
{
	return local_arg_table.get_symbol_table_entry(variable_name);
}

void Procedure::print(ostream & file_buffer)
{
	// CHECK_INVARIANT((return_type == void_data_type), "Only void return type of funtion is allowed");
	if(return_type == void_data_type){
		file_buffer << PROC_SPACE << "Procedure: "<<name<<", Return Type: VOID\n";
	}
	else if(return_type == int_data_type){
		file_buffer << PROC_SPACE << "Procedure: "<<name<<", Return Type: INT\n";
	}
	else if(return_type == float_data_type){
		file_buffer << PROC_SPACE << "Procedure: "<<name<<", Return Type: FLOAT\n";
	}

	if ((command_options.is_show_symtab_selected()) || (command_options.is_show_program_selected()))
	{
		
		file_buffer << "   Fromal Parameter List\n";
		if(compiled){
			local_arg_table.print_sp(file_buffer);
		}
		else local_arg_table.print(file_buffer);
		file_buffer << "   Local Declarartions\n";
		local_symbol_table.print(file_buffer);
	}

	if ((command_options.is_show_program_selected()) || (command_options.is_show_ast_selected()))
	{
		list<Basic_Block *>::iterator i;
		for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
			(*i)->print_bb(file_buffer);
	}
}

Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
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

bool Procedure::variable_in_arg_list_check(string variable)
{
	return local_arg_table.variable_in_symbol_list_check(variable);
}

Eval_Result & Procedure::evaluate(ostream & file_buffer)
{
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure " << name << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(result->get_result_enum() == skip_result_val){
			break;
		}
		if(result->get_result_enum() == goto_result){
			current_bb = get_jump_bb(result->get_value().int_val);
			continue;
		}
		current_bb = get_next_bb(*current_bb);		
	}

	if(result->get_result_enum() != skip_result_val){
		CHECK_INVARIANT(false , "Atleast one of true, false, direct successors should be set");
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating):\n";
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

	file_buffer << PROC_SPACE << "Evaluating Procedure " << name << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(result->get_result_enum() == skip_result_val){
			break;
		}
		if(result->get_result_enum() == goto_result){
			current_bb = get_jump_bb(result->get_value().int_val);
			continue;
		}
		current_bb = get_next_bb(*current_bb);		
	}

	if(result->get_result_enum() != skip_result_val){
		CHECK_INVARIANT(false , "Atleast one of true, false, direct successors should be set");
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating):\n";
	eval_env.print(file_buffer);

	return *result;
}



void Procedure::compile()
{
	// assign offsets to local symbol table
	// merge the local_symbol table and local_arg table
	// local_symbol_table.append_table(local_arg_table);

	// calculate size
	local_arg_table.reverse_table();
	local_arg_table.set_start_offset_of_first_symbol(0);
	local_arg_table.set_size(local_arg_table.getTotalSize()+8);
	local_arg_table.assign_offsets();
	local_arg_table.reverse_table();

	compiled = true;
	local_symbol_table.set_start_offset_of_first_symbol(4);
	local_symbol_table.set_size(0);
	local_symbol_table.assign_offsets();

	// compile the program by visiting each basic block
	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->compile();
}

void Procedure::print_icode(ostream & file_buffer)
{
	file_buffer << "  Procedure: " << name << "\n";
	file_buffer << "  Intermediate Code Statements\n";

	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_icode(file_buffer);
}

void Procedure::print_assembly(ostream & file_buffer)
{
	print_prologue(file_buffer);

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_assembly(file_buffer);
	// file_buffer<<"\tj epilogue_"<<name<<"\n";
	print_epilogue(file_buffer);
}

void Procedure::print_prologue(ostream & file_buffer)
{
	stringstream prologue;

	prologue << "\n\
	.text \t\t\t# The .text assembler directive indicates\n\
	.globl " << name << "\t\t# The following is the code (as oppose to data)\n";

	prologue << name << ":\t\t\t\t# .globl makes main know to the \n\t\t\t\t# outside of the program.\n\
# Prologue begins \n\
	sw $ra, 0($sp)\t\t# Save the return address\n\
	sw $fp, -4($sp)\t\t# Save the frame pointer\n\
	sub $fp, $sp, 8\t\t# Update the frame pointer\n";
	int size = local_symbol_table.get_size();
	// cout<<"THE SIE IS "<<size<<endl;
	size = -size;
	if (size > 0)
		prologue << "\n\tsub $sp, $sp, " << (size + 8) << "\t\t# Make space for the locals\n";
	else
		prologue << "\n\tsub $sp, $sp, 8\t\t# Make space for the locals\n";

	prologue << "# Prologue ends\n\n";

	file_buffer << prologue.str();
}

void Procedure::print_epilogue(ostream & file_buffer)
{
	stringstream epilogue;

	int size = local_symbol_table.get_size();
	size = -size;
	if (size > 0){
		epilogue << "\n# Epilogue Begins\n";
		epilogue<<"epilogue_"<<name<<":\n";
		epilogue << "\tadd $sp, $sp, " << (size + 8) << "\n";
	}
	else{
		epilogue << "\n# Epilogue Begins\n";
		epilogue<<"epilogue_"<<name<<":\n";
		epilogue<<"\tadd $sp, $sp, 8\n";
	}

	epilogue << "\tlw $fp, -4($sp)  \n\tlw $ra, 0($sp)\n\tjr        $31\t\t# Jump back to the called procedure\n# Epilogue Ends\n\n";

	file_buffer << epilogue.str();
}
