
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

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name) && eval_env.is_variable_defined(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	if (interpreter_global_table.does_variable_exist(variable_name) && interpreter_global_table.is_variable_defined(variable_name))
	{
		Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
		return *result;
	}

	report_error("Variable not defined", -2);
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
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
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" AST_SPACE <<  "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	print_ast(file_buffer);
	return result;
}

template class Number_Ast<int>;

//TODO_DONE

////////////////////////////////////////////////////////////////

Comparison_Ast::Comparison_Ast(Ast * temp_lhs, Comparison_Op_Enum temp_op, Ast * temp_rhs)
{
	lhs = temp_lhs;
	op = temp_op;
	rhs = temp_rhs;
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
	//changes for float or any other type of comparison to be made here
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		//Default data type of comparison is int, true is 1 and false is 0
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Comparison statement data type not compatible", line);
}

void Comparison_Ast::print_ast(ostream & file_buffer)
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
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer <<AST_NODE_SPACE<<"   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Comparison_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();

	switch(op){	
		case NE_OP:
			result.set_value((int)(lhs_result.get_value() != rhs_result.get_value()));
			break;
		case GE_OP:
			result.set_value((int)(lhs_result.get_value() >= rhs_result.get_value()));
			break;
		case LE_OP:
			result.set_value((int)(lhs_result.get_value() <= rhs_result.get_value()));
			break;
		case EQ_OP:
			result.set_value((int)(lhs_result.get_value() == rhs_result.get_value()));
			break;
		case LT_OP:
			result.set_value((int)(lhs_result.get_value() < rhs_result.get_value()));
			break;
		case GT_OP:
			result.set_value((int)(lhs_result.get_value() > rhs_result.get_value()));
			break;
	}
	
	return result;
}


/////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int temp_bb_number)
{
	bb_number = temp_bb_number;
}

Goto_Ast::~Goto_Ast()
{}

Data_Type Goto_Ast::get_data_type()
{
	return void_data_type;
}

void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Goto statement:";	
	file_buffer << "\n" << AST_NODE_SPACE<< "Successor: "<<bb_number;	

}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print_ast(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	result.set_value(bb_number);
	result.set_result_enum(skip_result);
	file_buffer <<"\n"<<AST_SPACE<< "GOTO (BB "<<bb_number<<")\n";
	return result;
}

/////////////////////////////////////////////////////////////////

If_Ast::If_Ast(Ast* temp_condition ,int temp_true_bb_number, int temp_false_bb_number)
{
	condition = temp_condition;
	true_bb_number = temp_true_bb_number;
	false_bb_number = temp_false_bb_number;
}

If_Ast::~If_Ast()
{
	delete condition;
}

Data_Type If_Ast::get_data_type()
{
	return void_data_type;
}

void If_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" AST_SPACE <<  "If_Else statement:";	
	condition->print_ast(file_buffer);
	file_buffer <<"\n"<<AST_NODE_SPACE<<"True Successor: "<<true_bb_number;
	file_buffer <<"\n"<<AST_NODE_SPACE<<"False Successor: "<<false_bb_number;
}

Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
		print_ast(file_buffer);

		Eval_Result & condition_result = condition->evaluate(eval_env, file_buffer);

		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_result_enum(skip_result);
		if(condition_result.get_value()){
			file_buffer <<"\n"<<AST_SPACE<<"Condition True : Goto (BB "<<true_bb_number<<")\n";
			result.set_value(true_bb_number);
		}
		else{
			file_buffer <<"\n"<< AST_SPACE<<"Condition False : Goto (BB "<<false_bb_number<<")\n";
			result.set_value(false_bb_number);
		}
		return result;
}

