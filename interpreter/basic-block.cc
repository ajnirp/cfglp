
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

using namespace std;

#include "local-environment.hh"
#include "error-display.hh"
#include "user-options.hh"

#include "symbol-table.hh"
#include "ast.hh"
#include "basic-block.hh"

Basic_Block::Basic_Block(int basic_block_number, list<Ast *> & ast_list)
{
	id_number = basic_block_number;
	statement_list = ast_list;
}

Basic_Block::~Basic_Block()
{
	list<Ast *>::iterator i;
	for (i = statement_list.begin(); i != statement_list.end(); i++)
		delete (*i);
}

int Basic_Block::get_bb_number()
{
	return id_number;
}
void Basic_Block::print_bb(ostream & file_buffer)
{
	file_buffer << "\n" << BB_SPACE << "Basic_Block " << id_number;

	list<Ast *>::iterator i;
	// cout<<"size is "<<statement_list.size()<<endl;
	// int k = 1;
	for(i = statement_list.begin(); i != statement_list.end(); i++){
		// cout<<k<<endl;
		(*i)->print_ast(file_buffer);
		// k++;
	}
}

Eval_Result & Basic_Block::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result * result = NULL;

	file_buffer << "\n" << BB_SPACE << "Basic Block: " << id_number<<"\n";

	list <Ast *>::iterator i;

	for (i = statement_list.begin(); i != statement_list.end(); i++)
	{
		if((*i) == NULL)
			report_error ("Ast pointer seems to be NULL", NOLINE);

		result = &((*i)->evaluate(eval_env, file_buffer));
		//TODO_DONE

		if(result->get_result_enum() == skip_result || result->get_result_enum() == goto_result || result->get_result_enum() == skip_result_val){
			break;
		}
	}

	if (result == NULL) {
		if (statement_list.size() > 0) {
			report_internal_error("Atleast one of true, false, direct successors should be set");
		}
		else {
			result = new Eval_Result_Value_Int();
			// result->set_result_enum(skip_result_val);
			result_value_type sVal;
			sVal.int_val == 2678;
			result->set_value(sVal);
		}
	}
	return *result;
}

void Basic_Block::successor_found(){
	if (statement_list.size() == 0) {
		return;
	}

	list<Ast *>::reverse_iterator i = statement_list.rbegin();
	if(i != statement_list.rend()){
		if(!((*i)->successor_found())){
			report_internal_error("Atleast one of true, false, direct successors should be set");
		}
	}
	else{
		report_internal_error("Atleast one of true, false, direct successors should be set");
	}
	
}
