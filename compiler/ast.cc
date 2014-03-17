
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

#include<iostream>
#include<fstream>
#include<typeinfo>

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

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

/*
Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type; // submission 5a
}
*/

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:";

	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}

// submission 5a
//TODO - add support for arbitrary lhs
Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	lra.optimize_lra(mc_2m, lhs, rhs);
	Code_For_Ast load_stmt = rhs->compile_and_optimize_ast(lra);

	Register_Descriptor * result_register = load_stmt.get_reg();

	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);

	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	return *assign_stmt;
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		bool is_int_result = loc_var_val->get_result_enum() == int_result;
		bool is_float_result = loc_var_val->get_result_enum() == float_result;
		CHECK_INVARIANT(is_int_result || is_float_result, "Result type can only be int and float");

		if (is_int_result)
			file_buffer << loc_var_val->get_value().int_val << "\n";
		else if (is_float_result)
			file_buffer << loc_var_val->get_value().float_val << "\n";
	}

	else
	{
		bool is_int_result = glob_var_val->get_result_enum() == int_result;
		bool is_float_result = glob_var_val->get_result_enum() == float_result;
		CHECK_INVARIANT(is_int_result || is_float_result, "Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else {
			if (is_int_result)
				file_buffer << loc_var_val->get_value().int_val << "\n";
			else if (is_float_result)
				file_buffer << loc_var_val->get_value().float_val << "\n";
		}
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	// if (variable_symbol_entry->get_data_type() == int_data_type)
	// 	i = new Eval_Result_Value_Int();
	// else if (variable_symbol_entry->get_data_type() == float_data_type)
	// 	i = new Eval_Result_Value_Float();
	// else
	// 	CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (result.get_result_enum() == int_result) {
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}
	else if (result.get_result_enum() == float_result) {
		i = new Eval_Result_Value_Float();
		i->set_value(result.get_value());
	}
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(load, opd, register_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt = new Move_IC_Stmt(store, register_opd, opd);

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		load_stmt = new Move_IC_Stmt(load, opd, register_opd);
			
		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;

	ast_num_child = zero_arity;

	lineno = line;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result_value_type sVal;
		sVal.int_val = constant;
		result.set_value(sVal);
		return result;
	}
	else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result_value_type sVal;
		sVal.float_val = constant;
		result.set_value(sVal);
		return result;
	}
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);

	return num_code;
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	return result;
}

Code_For_Ast & Return_Ast::compile()
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

template class Number_Ast<int>;
template class Number_Ast<float>;

///////////////////////////////////////////////////////////////////////////////

Comparison_Ast::Comparison_Ast(Ast * temp_lhs, Comparison_Op_Enum temp_op, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	op = temp_op;
	rhs = temp_rhs;
	node_data_type = int_data_type;
	lineno = line;
}

Comparison_Ast::~Comparison_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Comparison_Ast::get_data_type()
{
	return node_data_type;
}

bool Comparison_Ast::check_ast(int line)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	//changes for float or any other type of comparison to be made here
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		//Default data type of comparison is int, true is 1 and false is 0
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Comparison statement data type not compatible");
}

void Comparison_Ast::print(ostream & file_buffer)
{
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: ";
	switch(op){
		case NE_OP:
			file_buffer << "NE";
			break;
		case GE_OP:
			file_buffer << "GE";
			break;
		case LE_OP:
			file_buffer << "LE";
			break;
		case EQ_OP:
			file_buffer << "EQ";
			break;
		case LT_OP:
			file_buffer << "LT";
			break;
		case GT_OP:
			file_buffer << "GT";
			break;
	}

	file_buffer <<"\n"<<AST_NODE_SPACE<<"   LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer <<AST_NODE_SPACE<<"   RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Comparison_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();

	CHECK_INPUT_AND_ABORT(lhs_result.is_variable_defined(), "Variable should be defined to be on lhs of Comparison_Ast", lineno);
	CHECK_INPUT_AND_ABORT(rhs_result.is_variable_defined(), "Variable should be defined to be on rhs of Comparison_Ast", lineno);

	result_value_type sVal;

	if(lhs_result.get_result_enum() == int_result){
		switch(op){
			case NE_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val != rhs_result.get_value().int_val);
				break;
			case GE_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val >= rhs_result.get_value().int_val);
				break;
			case LE_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val <= rhs_result.get_value().int_val);
				break;
			case EQ_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val == rhs_result.get_value().int_val);
				break;
			case LT_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val < rhs_result.get_value().int_val);
				break;
			case GT_OP:
				sVal.int_val = (int)(lhs_result.get_value().int_val > rhs_result.get_value().int_val);
				break;
		}
		result.set_value(sVal);
	}
	else if(lhs_result.get_result_enum() == float_result){
		switch(op){	
			case NE_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val != rhs_result.get_value().float_val);
				break;
			case GE_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val >= rhs_result.get_value().float_val);
				break;
			case LE_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val <= rhs_result.get_value().float_val);
				break;
			case EQ_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val == rhs_result.get_value().float_val);
				break;
			case LT_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val < rhs_result.get_value().float_val);
				break;
			case GT_OP:
				sVal.int_val = (int)(lhs_result.get_value().float_val > rhs_result.get_value().float_val);
				break;
		}
		result.set_value(sVal);
	}
	
	
	return result;
}

//submission 5a
Code_For_Ast & Comparison_Ast::compile()
{
	/*
		An comparison x comp y where y is a variable is 
		compiled as two load statements followed by
		a set statement (sge, sle, sgt, slt):
		(load) R <- x
		(load) S <- y
		(set) T <- R , S
		If y is a constant, the statement is compiled as:
		(load) R <- x
		(imm_Load) S <- y
		(set) T <- R , S
		where imm_Load denotes the load immediate operation.
	*/
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_lhs_stmt = lhs->compile();
	Code_For_Ast & load_rhs_stmt = rhs->compile();

	Register_Descriptor * load_lhs_register = load_lhs_stmt.get_reg();
	Register_Descriptor * load_rhs_register = load_rhs_stmt.get_reg();

	Code_For_Ast set_stmt = lhs->create_set_stmt(load_lhs_register, load_rhs_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_lhs_stmt.get_icode_list().empty() == false)
		ic_list = load_lhs_stmt.get_icode_list();
	if (load_rhs_stmt.get_icode_list().empty() == false)
		ic_list = load_rhs_stmt.get_icode_list();

	if (set_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), set_stmt.get_icode_list());

	// register which is being set after comparing the other two
	Register_Descriptor * set_register = set_stmt.get_reg();

	Code_For_Ast * comparison_stmt;
	if (ic_list.empty() == false) {
		comparison_stmt = new Code_For_Ast(ic_list, set_register);
	}

	return *comparison_stmt;
}

Code_For_Ast & Comparison_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	//TODO
}

Code_For_Ast & Comparison_Ast::create_set_stmt(Register_Descriptor* reg1, Register_Descriptor* reg2)
{
	CHECK_INVARIANT((reg1 != NULL), "First register cannot be null");
	CHECK_INVARIANT((reg2 != NULL), "Second register cannot be null");

	//TODO
}

/////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int temp_bb_number, int line)
{
	bb_number = temp_bb_number;
	lineno = line;
}

Goto_Ast::~Goto_Ast()
{}

Data_Type Goto_Ast::get_data_type()
{
	return void_data_type;
}

void Goto_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Goto statement:";	
	file_buffer << "\n" << AST_NODE_SPACE<< "Successor: "<<bb_number;	

}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	result_value_type sVal;
	sVal.int_val = bb_number;
	result.set_value(sVal);
	result.set_result_enum(goto_result);
	file_buffer <<"\n"<<AST_SPACE<< "GOTO (BB "<<bb_number<<")\n";
	return result;
}

bool Goto_Ast::successor_found(){
	return true;	
}

/////////////////////////////////////////////////////////////////

If_Ast::If_Ast(Ast* temp_condition ,int temp_true_bb_number, int temp_false_bb_number, int line)
{
	condition = temp_condition;
	true_bb_number = temp_true_bb_number;
	false_bb_number = temp_false_bb_number;
	lineno = line;
}

If_Ast::~If_Ast()
{
	delete condition;
}

Data_Type If_Ast::get_data_type()
{
	return void_data_type;
}

void If_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" AST_SPACE <<  "If_Else statement:";	
	condition->print(file_buffer);
	file_buffer <<"\n"<<AST_NODE_SPACE<<"True Successor: "<<true_bb_number;
	file_buffer <<"\n"<<AST_NODE_SPACE<<"False Successor: "<<false_bb_number;
}

Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
		file_buffer << "\n" AST_SPACE <<  "If_Else statement:";	
		condition->print(file_buffer);

		Eval_Result & condition_result = condition->evaluate(eval_env, file_buffer);

		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_result_enum(goto_result);
		file_buffer <<"\n"<<AST_NODE_SPACE<<"True Successor: "<<true_bb_number;
		file_buffer <<"\n"<<AST_NODE_SPACE<<"False Successor: "<<false_bb_number;
		if(condition_result.get_value().int_val){
			file_buffer <<"\n"<<AST_SPACE<<"Condition True : Goto (BB "<<true_bb_number<<")\n";
			result_value_type sVal;
			sVal.int_val = true_bb_number;
			result.set_value(sVal);
		}
		else{
			file_buffer <<"\n"<< AST_SPACE<<"Condition False : Goto (BB "<<false_bb_number<<")\n";
			result_value_type sVal;
			sVal.int_val = false_bb_number;
			result.set_value(sVal);
		}
		return result;
}

bool If_Ast::successor_found(){
	return true;	
}