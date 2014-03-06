
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
#include<iomanip>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"procedure.hh"
#include"program.hh"

Program program_object;

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

bool Ast::successor_found(){
	return false;	
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
			file_buffer << loc_var_val->get_value().int_val << "\n";
		else if (loc_var_val->get_result_enum() == float_result)
			file_buffer<< loc_var_val->get_value().float_val << "\n";
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
				file_buffer << glob_var_val->get_value().int_val << "\n";
		}
		else if (glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0.00\n";
			else
				file_buffer << glob_var_val->get_value().float_val << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if(!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name)){
		report_error("Variable should be defined before its use",-2);
	}
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}
	else if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
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
	// print_ast(file_buffer);

	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result_value_type sVal;
		sVal.int_val = constant;
		// cout<<"CONSTANT "<<constant<<endl;
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

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(Ast * r_ast)
{
	ret_ast = r_ast;
}

Return_Ast::~Return_Ast()
{
	delete(ret_ast);
}

void Return_Ast::print_ast(ostream & file_buffer)
{
	if(ret_ast == NULL)
	{
		file_buffer << "\n" <<AST_SPACE <<  "RETURN <NOTHING>\n";
	}
	else{
		file_buffer << "\n" <<AST_SPACE <<  "RETURN ";
		ret_ast->print_ast(file_buffer);
		file_buffer<<"\n\n";
	}

}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if(ret_ast == NULL){
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_result_enum(skip_result);
		result_value_type sVal;
		sVal.int_val == 2678;
		result.set_value(sVal);
		print_ast(file_buffer);
		return result;
	}
	else{
		Eval_Result & result = ret_ast->evaluate(eval_env ,file_buffer);
		print_ast(file_buffer);
		if(result.get_result_enum() == int_result){
			Eval_Result & res = *new Eval_Result_Value_Int();
			res.set_value(result.get_value());
			res.set_result_enum(skip_result);
			return res;
		}
		else{
			Eval_Result & res = *new Eval_Result_Value_Float();
			res.set_value(result.get_value());
			res.set_result_enum(skip_result);
			return res;
		}
	}
	
}

bool Return_Ast::successor_found(){
	return true;	
}

/////////////////////////////////////////////////////////////////

template class Number_Ast<int>;
// submission 3b
template class Number_Ast<float>;

/////////////////////////////////////////////////////////////////

Comparison_Ast::Comparison_Ast(Ast * temp_lhs, Comparison_Op_Enum temp_op, Ast * temp_rhs)
{
	lhs = temp_lhs;
	op = temp_op;
	rhs = temp_rhs;
	node_data_type = int_data_type;
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
		// node_data_type = lhs->get_data_type();
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
	// cout<<"in here"<<endl;	
	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	if(!lhs_result.is_variable_defined()){
		report_error("Variable should be defined to be on lhs of condition", -2);
	}

	if(!rhs_result.is_variable_defined()){
		report_error("Variable should be defined to be on rhs of condition", -2);
	}

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

/////////////////////////////////////////////////////////////////

// submission 3b
Arithmetic_Ast::Arithmetic_Ast(Ast * temp_lhs, Arith_Op_Enum temp_op, Ast * temp_rhs)
{
	lhs = temp_lhs;
	op = temp_op;
	rhs = temp_rhs;
}

Arithmetic_Ast::~Arithmetic_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Arithmetic_Ast::get_data_type()
{
	return node_data_type;
}

// todo
bool Arithmetic_Ast::check_ast(int line)
{
	// cout<<"hy ethe "<<lhs->get_data_type()<<"  "<<rhs->get_data_type()<<endl;
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		//Default data type of comparison is int, true is 1 and false is 0
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic statement data type not compatible", line);
}

void Arithmetic_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Arith: ";
	switch(op)
	{
		case PLUS_OP:
			file_buffer << "PLUS";
			break;
		case MINUS_OP:
			file_buffer << "MINUS";
			break;
		case MUL_OP:
			file_buffer << "MULT";
			break;
		case DIV_OP:
			file_buffer << "DIV";
			break;
	}

	file_buffer <<"\n"<<AST_NODE_SPACE<<"   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer <<AST_NODE_SPACE<<"   RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Arithmetic_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result & lhs_result = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & rhs_result = rhs->evaluate(eval_env, file_buffer);
	if(!lhs_result.is_variable_defined()){
		report_error("Variable should be defined to be on lhs of arithmetic expression", -2);
	}

	if(!rhs_result.is_variable_defined()){
		report_error("Variable should be defined to be on rhs of arithmetic expression", -2);
	}
	Eval_Result & result = *new Eval_Result_Value_Int();

	result_value_type sVal;
	if(lhs_result.get_result_enum() == int_result){
		result = *new Eval_Result_Value_Int();
		switch(op){	
			case PLUS_OP:
				sVal.int_val = lhs_result.get_value().int_val + rhs_result.get_value().int_val;
				break;
			case MINUS_OP:
				sVal.int_val = lhs_result.get_value().int_val - rhs_result.get_value().int_val;
				break;
			case MUL_OP:
				sVal.int_val = lhs_result.get_value().int_val * rhs_result.get_value().int_val;
				break;
			case DIV_OP:
				sVal.int_val = lhs_result.get_value().int_val / rhs_result.get_value().int_val;
				break;
		}
		result.set_value(sVal);
	}
	else if(lhs_result.get_result_enum() == float_result){
		result = *new Eval_Result_Value_Float();
		switch(op){	
			case PLUS_OP:
				sVal.float_val = lhs_result.get_value().float_val + rhs_result.get_value().float_val;
				break;
			case MINUS_OP:
				sVal.float_val = lhs_result.get_value().float_val - rhs_result.get_value().float_val;
				break;
			case MUL_OP:
				sVal.float_val = lhs_result.get_value().float_val * rhs_result.get_value().float_val;
				break;
			case DIV_OP:
				sVal.float_val = lhs_result.get_value().float_val / rhs_result.get_value().float_val;
				break;
		}
		result.set_value(sVal);
	}
	return result;
}

/////////////////////////////////////////////////////////////////

// submission 3b
Typecast_Ast::Typecast_Ast(Data_Type dt, Ast * a)
{
	dtype = dt;
	ast = a;
}

Typecast_Ast::~Typecast_Ast()
{
	delete ast;
}

Data_Type Typecast_Ast::get_data_type()
{
	return dtype;
}

void Typecast_Ast::print_ast(ostream & file_buffer)
{
	ast->print_ast(file_buffer);
}

Eval_Result & Typecast_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & r = ast->evaluate(eval_env, file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	result_value_type sVal;
	switch (dtype)
	{
		case int_data_type:
			result.set_variable_status(r.is_variable_defined());
			if(r.get_result_enum() == float_result){
				sVal.int_val = (int)r.get_value().float_val;
				result.set_value(sVal);
			}
			else{
				result.set_value(r.get_value());
			}
			break;
		case float_data_type:
			result = *new Eval_Result_Value_Float();
			result.set_variable_status(r.is_variable_defined());
			if(r.get_result_enum() == int_result){
				sVal.float_val = (float)r.get_value().int_val;
				result.set_value(sVal);
			}
			else{
				result.set_value(r.get_value());
			}
			break;
	}
	return result;
}

/////////////////////////////////////////////////////////////////

// submission 3b
UnaryMinus_Ast::UnaryMinus_Ast(Ast * a)
{
	ast = a;
	node_data_type = a->get_data_type();
}

UnaryMinus_Ast::~UnaryMinus_Ast()
{
	delete ast;
}

Data_Type UnaryMinus_Ast::get_data_type()
{
	return node_data_type;
}

void UnaryMinus_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Arith: UMINUS";

	file_buffer <<"\n"<<AST_NODE_SPACE<<"   LHS (";
	ast->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & UnaryMinus_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	Eval_Result & r = ast->evaluate(eval_env,file_buffer);
	result_value_type sVal;
	if(node_data_type == float_data_type){
		result = *new Eval_Result_Value_Float();
		sVal.float_val = -1*r.get_value().float_val;
		result.set_value(sVal);
	}
	else{
		sVal.int_val = -1*r.get_value().int_val;
		result.set_value(sVal);
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
		result.set_result_enum(goto_result);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////

function_call_Ast::function_call_Ast (string fName, list<Ast *> * AList){
	fn_name = fName;
	argList = *AList;
}
function_call_Ast::~function_call_Ast(){
	list<Ast *>::iterator i;
	for (i = argList.begin(); i != argList.end(); i++)
		delete (*i);
}

Data_Type function_call_Ast::get_data_type(){
	
	Procedure * curP =  program_object.get_procedure_map(fn_name);
	// cout<<"idhar aaya "<<curP->get_return_type()<<endl;
	return curP->get_return_type();
}

void function_call_Ast::print_ast(ostream & file_buffer){
	file_buffer<<"\n"<<AST_SPACE<<"FN CALL: fn(";
	list<Ast*>::iterator it;
	for(it=argList.begin(); it!=argList.end(); it++){
		file_buffer<<"\n"<<AST_NODE_SPACE;
		(*it)->print_ast(file_buffer);
	}
	file_buffer<<")";
}

Eval_Result & function_call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
	Procedure * curP =  program_object.get_procedure_map(fn_name);
	if(curP == NULL){
		cout<<"Process does not exist!!"<<endl;
		exit(0);
	}
	list<string> args = curP->get_arg_string_list();
	if(args.size() != argList.size()){
		cout<<"Wrong number of arguments"<<endl;
		exit(0);
	}
	 // = *new Eval_Result_Value_Int();
	Local_Environment & proc_local_env = curP->get_local_env();
	list<Ast*>::iterator it1;
	list<string>::iterator it2;
// cout<<"hey there "<<argList.size()<<endl;
	result_value_type sVal;
	for(it1=argList.begin(),it2=args.begin(); it1!=argList.end(); it1++,it2++){
		//change the buffer later
		Eval_Result & tempResult = (*it1)->evaluate(eval_env, file_buffer);
		// cout<<"look at me "<<(tempResult.get_value()).float_val<<endl
		if(tempResult.get_result_enum() == proc_local_env.get_variable_value(*it2)->get_result_enum()){
			if(tempResult.get_result_enum() == int_result){
				Eval_Result_Value & arg_val = *new Eval_Result_Value_Int();
				sVal.int_val = tempResult.get_value().int_val;
				arg_val.set_value(sVal);
				proc_local_env.put_variable_value(arg_val,*it2);
			}
			else{
				Eval_Result_Value & arg_val = *new Eval_Result_Value_Float();
				sVal.float_val = tempResult.get_value().float_val;
				arg_val.set_value(sVal);
				proc_local_env.put_variable_value(arg_val,*it2);
			}
			
		}
		else{
			cout<<"incompatible arguments"<<endl;
			exit(0);
		}
	}
	Eval_Result & ret_res = curP->evaluate_in_env(file_buffer,proc_local_env);
	Data_Type ret_data_type = curP->get_return_type();
	if(ret_data_type == int_data_type){
		ret_res.set_result_enum(int_result);
	}
	else if(ret_data_type == float_data_type){
		ret_res.set_result_enum(float_result);
	}
	else if(ret_data_type == void_data_type){
		ret_res.set_result_enum(void_result);
	}
	return ret_res;
}
