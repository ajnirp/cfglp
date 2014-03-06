
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

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
	Comparison_Op_Enum comparison_op_enum;
	Data_Type data_type;
	Arith_Op_Enum arith_op_enum;
};


%token <integer_value> INTEGER_NUMBER
%token <integer_value> BASIC_BLOCK
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token RETURN
%token INTEGER FLOAT DOUBLE VOID
%token IF ELSE GOTO
%token ASSIGN_OP NE EQ LT LE GT GE

%type <symbol_table> declaration_statement_list
%type <symbol_table> parameter_list
%type <symbol_entry> declaration_statement
%type <symbol_entry> parameter
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_or_function_call_list
%type <ast_list> argumentList
%type <ast> assignment_statement
%type <ast> function_call
%type <ast> return_stmt
%type <ast> var_const
%type <ast> var_const_plain
%type <ast> variable
%type <ast> constant

%type <comparison_op_enum> comparison_op
%type <data_type> typecast
%type <arith_op_enum> plus_minus
%type <arith_op_enum> mul_div
%type <comparison_op_enum> equality_op
%type <ast> comparison_expr
%type <ast> h_comparison_expr
%type <ast> all_expr
%type <ast> goto_statement
%type <ast> if_statement
%type <ast> mul_div_expr
%type <ast> arithmetic_expr


%start program

%%

program:
	declaration_statement_list procedure_declaration_list {
		program_object.set_global_table(*$1);
	}
	procedure_definition_list
|
	procedure_declaration_list procedure_definition_list
|
	declaration_statement_list {
		program_object.set_global_table(*$1);
	} 
	procedure_definition_list
|
	procedure_definition_list
;

procedure_declaration:
	INTEGER  procedure_name ';' {
		current_procedure->set_data_type(int_data_type);
	}
|
	FLOAT procedure_name ';' {
		current_procedure->set_data_type(float_data_type);
	}
|
	DOUBLE procedure_name ';' {
		current_procedure->set_data_type(float_data_type);
	}
|
	VOID procedure_name ';' {
		current_procedure->set_data_type(void_data_type);
	}
;

procedure_declaration_list:
	procedure_declaration
|	
	procedure_declaration_list procedure_declaration
;

procedure_definition_list:
	procedure_definition_list procedure_definition
|
	procedure_definition
;

procedure_definition:
	procedure_name procedure_body
;

parameter_list
:	parameter {
		//if
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
		current_procedure->addArg($1->get_variable_name());
		//end
	}
|	parameter_list ',' parameter {
		//if
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($3->get_variable_name(), line);

		string var_name = $3->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($3);
		current_procedure->addArg($3->get_variable_name());
		//end
	}
;

parameter:
	INTEGER NAME {
		//if
		$$ = new Symbol_Table_Entry(*$2, int_data_type);
		delete $2;
		//end
	}
|
	FLOAT NAME {
		//if
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
		//end
	}
|
	DOUBLE NAME {
		//if
		$$ = new Symbol_Table_Entry(*$2, float_data_type);
		delete $2;
		//end
	}
;

procedure_name:
	NAME '(' {	
			//if
			if(program_object.get_procedure_map(*$1) == NULL){
			//cout<<"KHKJHKKJHHKJHK"<<endl;
				current_procedure = new Procedure(void_data_type, *$1);
				//current_procedure->set_arg_list(*$3);
				//current_procedure->set_name(*$1);
				//exit(0);
				program_object.set_procedure_map(*current_procedure);
			}
			else{
				current_procedure = program_object.get_procedure_map(*$1);
				//current_procedure->set_arg_list(*$3);
			}
			//end
		}
	parameter_list ')'
	{
		//if
		current_procedure->set_arg_list(*$4);
		//end
	}
|
	NAME '(' ')' {
		//if
		if(program_object.get_procedure_map(*$1) == NULL){
			current_procedure = new Procedure(void_data_type, *$1);
			//current_procedure->set_name(*$1);
			current_procedure->set_arg_list(*(new Symbol_Table()));
			program_object.set_procedure_map(*current_procedure);
		}
		else{
			current_procedure = program_object.get_procedure_map(*$1);
			//current_procedure->set_arg_list(*(new Symbol_Table()));
		}
		//end
	}
;

procedure_body:
	'{' declaration_statement_list
	{
		//if
		current_procedure->set_local_list(*$2);
		delete $2;
		//end
	
	}
	basic_block_list '}'
	{
		
		//if
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$4);

		delete $4;
		//end
	}
|
	'{' basic_block_list '}'
	{
		
		//if
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$2);

		delete $2;
		//end
	}
;

declaration_statement_list:
	declaration_statement
	{
		//if
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if(current_procedure != NULL && current_procedure->variable_in_arg_list_check(var_name))
		{
			int line = get_line_number();
			report_error("Variable is declared twice", line);
		}
			
		$$ = new Symbol_Table();
		$$->push_symbol($1);
		//end
	}
|
	declaration_statement_list declaration_statement
	{
		//if
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		if(current_procedure != NULL && current_procedure->variable_in_arg_list_check(var_name))
		{
			int line = get_line_number();
			report_error("Variable is declared twice", line);
		}

		$$->push_symbol($2);
		//end
	}
;

declaration_statement:
	INTEGER NAME ';'
	{
		//if
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
		//end
	
	}
|
	FLOAT NAME ';'
	{
		//if
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		//end
	
	}
|
	DOUBLE NAME ';'
	{
		//if
		
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		//end
	
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		//if
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}
		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
		//end
	}
|
	basic_block
	{
		//if
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
		//end
	}
	
;


basic_block:
	BASIC_BLOCK ':' executable_statement_list
	{
		//if
		if ($3 != NULL){
			$$ = new Basic_Block($1, *$3);
		}
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}
		bb_made.insert($1);
		//end
	}
;

executable_statement_list:
	assignment_or_function_call_list
	{
		//if
		$$ = $1;
		//end
	}
|
	assignment_or_function_call_list return_stmt ';'
	{
		//if
		//Ast * ret = new Return_Ast();

		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		//end
	}
|
	assignment_or_function_call_list goto_statement ';'
	{
	
		//if

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		//end
	}
|
	assignment_or_function_call_list if_statement
	{
		//if
		
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		//end
	}
;


if_statement:
	IF '(' h_comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
		
		//if
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		//end
	}
|
	IF '(' comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		//if
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		//end
	}
|	IF '(' var_const ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		//if
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		//end
	}
;
	

goto_statement
:	GOTO BASIC_BLOCK {
		
		//if
		$$ = new Goto_Ast($2);
		bb_requested.push_back($2);
		//end
	}
;

assignment_or_function_call_list:
	{
		//if
		$$ = NULL;
		//end
	}
|
	assignment_or_function_call_list assignment_statement
	{
		//if
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		//end
	}
|
	assignment_or_function_call_list function_call ';'
	{
		//if
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		//end
	}
;

function_call
:	NAME '(' argumentList ')' {
		//program_object.get_procedure_map(*$1).get_return_type()
		$$ = new function_call_Ast(*$1,$3);
	}
|	NAME '(' ')' {
		$$ = new function_call_Ast(*$1,new list<Ast *>);
	}
;

argumentList
:	var_const {
		//if
		$$ = new list<Ast *>;
		$$->push_back($1);
		//end
	}
| 	all_expr {
		//if
		$$ = new list<Ast *>;
		$$->push_back($1);
		//end
	}
|	argumentList ',' var_const {
		//if
		if ($1 == NULL)
			$$ = new list<Ast *>;
		else
			$$ = $1;

		$$->push_back($3);
		//end
	}
|	argumentList ',' all_expr {
		//if
		if ($1 == NULL)
			$$ = new list<Ast *>;
		else
			$$ = $1;

		$$->push_back($3);
		//end
	}
;


assignment_statement:
	variable ASSIGN_OP var_const ';'
	{
		//if
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|
	variable ASSIGN_OP h_comparison_expr ';'
	{
	
		//if
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|
	variable ASSIGN_OP comparison_expr ';'	{
	
		//if
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|
	variable ASSIGN_OP arithmetic_expr ';'	{
		
		//if
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		//end
	
	}
|
	variable ASSIGN_OP mul_div_expr ';'	{
		
		//if
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
		//end
	}
;

equality_op
:	NE	{
		//if
		$$ = NE_OP; 
		//end
	}
|	EQ	{
		//if
		$$ = EQ_OP; 
		//end
	}
;

comparison_op
:	GE	{
		//if
		$$ = GE_OP; 
		//end
	}
|	LE	{
		//if
		$$ = LE_OP; 
		//end
	}
|	LT	{
		//if
		$$ = LT_OP; 
		//end
	}
|	GT	{
		//if
		$$ = GT_OP; 
		//end
	}
;

plus_minus
:	'+' {
		//if
		$$ = PLUS_OP;
		//end
	}
| 	'-' {
		//if
		$$ = MINUS_OP;
		//end
	}
;

mul_div
: 	'/' {
		//if
		$$ = DIV_OP;
		//end
	}
|	'*' {
		//if
		$$ = MUL_OP;
		//end
	}

;

typecast
:	'(' INTEGER ')' {
		//if
		$$ = int_data_type;
		//end
	}
|	'(' FLOAT ')' {
		//if
		$$ = float_data_type;
		//end
	}
|	'(' DOUBLE ')' {
		//if
		$$ = float_data_type;
		//end
	}
;

h_comparison_expr
:	var_const equality_op var_const 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	var_const equality_op comparison_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	var_const equality_op arithmetic_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	var_const equality_op mul_div_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	comparison_expr equality_op var_const 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	comparison_expr equality_op comparison_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	comparison_expr equality_op arithmetic_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	comparison_expr equality_op mul_div_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	mul_div_expr equality_op var_const 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	mul_div_expr equality_op comparison_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	mul_div_expr equality_op arithmetic_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	mul_div_expr equality_op mul_div_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	arithmetic_expr equality_op comparison_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	arithmetic_expr equality_op var_const 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	arithmetic_expr equality_op arithmetic_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	arithmetic_expr equality_op mul_div_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	h_comparison_expr equality_op comparison_expr 	{
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	h_comparison_expr equality_op arithmetic_expr {
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	h_comparison_expr equality_op var_const {
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
|	h_comparison_expr equality_op mul_div_expr {
		//if
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		//end
	}
;


comparison_expr
: var_const comparison_op var_const {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| var_const comparison_op mul_div_expr {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| var_const comparison_op arithmetic_expr {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| arithmetic_expr comparison_op var_const{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| arithmetic_expr comparison_op mul_div_expr{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| arithmetic_expr comparison_op arithmetic_expr{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| mul_div_expr comparison_op var_const{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| mul_div_expr comparison_op mul_div_expr{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| mul_div_expr comparison_op arithmetic_expr{
	//level2
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| comparison_expr comparison_op var_const {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| comparison_expr comparison_op mul_div_expr {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
| comparison_expr comparison_op arithmetic_expr {
	//if
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
;

mul_div_expr
:	var_const mul_div var_const{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	mul_div_expr mul_div var_const {
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
;


arithmetic_expr
:	var_const plus_minus var_const{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	var_const plus_minus mul_div_expr{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	mul_div_expr plus_minus var_const{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	mul_div_expr plus_minus mul_div_expr{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	arithmetic_expr plus_minus var_const{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
|	arithmetic_expr plus_minus mul_div_expr{
	//if
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	//end
}
;

var_const:
	var_const_plain{
		//if
		$$ = $1;
		//end
	}
|	typecast var_const_plain {
		//if
		$$ = new Typecast_Ast($1, $2);
		//end
	}
|	'-' typecast var_const_plain {
		//if
		$$ = new Typecast_Ast($2, new UnaryMinus_Ast($3));
		//end
	}
| '-' var_const_plain {
	//if
	$$ = new UnaryMinus_Ast($2);
	//end
}
;


var_const_plain
:	variable {
	//if
	$$ = $1;
	//end
}
|	constant {
	//if
	$$ = $1;
	//end
}	
| function_call{
	//if
	$$ = $1;
	//end
}
| '(' all_expr ')' {
	//if
	$$ = $2;
	//end
}
| '(' var_const ')' {
	//if
	$$ = $2;
	//end
}
;

return_stmt
:	RETURN {
		//if
		$$ = new Return_Ast(NULL);
		//end
	}
|	RETURN var_const {
		//if
		$$ = new Return_Ast($2);
		//end
}
|	RETURN all_expr {
		//if
		$$ = new Return_Ast($2);
		//end
}
;

all_expr
:	mul_div_expr {
		//if
		$$ = $1;
		//end
	}
|	arithmetic_expr {
		//if
		$$ = $1;
		//end
	}
|	comparison_expr {
		//if
		$$ = $1;
		//end
	}
|	h_comparison_expr {
		//if
		$$ = $1;
		//end
	}
;

variable:
	NAME
	{
		//if
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
		//end
	}
;

constant:
	INTEGER_NUMBER
	{
		//if
		$$ = new Number_Ast<int>($1, int_data_type);
		//end	
	}
|	
	FLOAT_NUMBER
	{
		//if
		$$ = new Number_Ast<float>($1, float_data_type);	
		//end
	}
;
