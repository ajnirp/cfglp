
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
%type <symbol_entry> declaration_statement
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
	declaration_statement_list procedure_declaration_list procedure_definition_list
|
	procedure_declaration_list procedure_definition_list
|
	declaration_statement_list procedure_definition_list
|
	procedure_definition_list
;

procedure_declaration:
	INTEGER procedure_name ';'
|
	FLOAT procedure_name ';'
|
	DOUBLE procedure_name ';'
|
	VOID procedure_name ';'
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

parameter_list:
	parameter_list ',' parameter
|
	parameter
;

parameter:
	INTEGER NAME
|
	FLOAT NAME
|
	DOUBLE NAME
;

procedure_name:
	NAME '(' parameter_list ')'
	{
		#if 0
		current_procedure = new Procedure(void_data_type, *$1);
		#endif
		
	}
|
	NAME '(' ')'
;

procedure_body:
	'{' declaration_statement_list
	{
		#if 0
		current_procedure->set_local_list(*$2);
		delete $2;
		#endif
	
	}
	basic_block_list '}'
	{
		
		#if 0
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$4);

		delete $4;
		#endif
	}
|
	'{' basic_block_list '}'
	{
		
		#if 0
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$2);

		delete $2;
		#endif
	}
;

declaration_statement_list:
	declaration_statement
	{
		
		#if 0
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
		#endif
	}
|
	declaration_statement_list declaration_statement
	{
		
		#if 0
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

		$$->push_symbol($2);
		#endif
	}
;

declaration_statement:
	INTEGER NAME ';'
	{
		#if 0
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
		#endif
	
	}
|
	FLOAT NAME ';'
	{
		#if 0
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		#endif
	
	}
|
	DOUBLE NAME ';'
	{
		#if 0
		
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		#endif
	
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		#if 0
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}
		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
		#endif
	}
|
	basic_block
	{
		#if 0
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
		#endif
	}
	
;


basic_block:
	BASIC_BLOCK ':' executable_statement_list
	{
		#if 0
		if ($3 != NULL){
			$$ = new Basic_Block($1, *$3);
		}
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}
		bb_made.insert($1);
		#endif
	}
;

executable_statement_list:
	assignment_or_function_call_list
	{
		#if 0
		$$ = $1;
		#endif
	}
|
	assignment_or_function_call_list return_stmt ';'
	{
		#if 0
		Ast * ret = new Return_Ast();

		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back(ret);
		#endif
	}
|
	assignment_or_function_call_list goto_statement ';'
	{
	
		#if 0

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		#endif
	}
|
	assignment_or_function_call_list if_statement
	{
		#if 0
		
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		#endif
	}
;


if_statement:
	IF '(' h_comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
		
		#if 0
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		#endif
	}
|
	IF '(' comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		#if 0
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		#endif
	}
|	IF '(' var_const ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		#if 0
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		#endif
	}
;
	

goto_statement
:	GOTO BASIC_BLOCK {
		
		#if 0
		$$ = new Goto_Ast($2);
		bb_requested.push_back($2);
		#endif
	}
;

assignment_or_function_call_list:
	{
		#if 0
		$$ = NULL;
		#endif
	}
|
	assignment_or_function_call_list assignment_statement
	{
		#if 0
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		#endif
	}
|
	assignment_or_function_call_list function_call ';'
	{
		#if 0
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		#endif
	}
;

function_call
:	variable '(' argumentList ')' {
	
	}
|	variable '(' ')' {
	
	}
;

argumentList
:	var_const {
		#if 0
		$$ = new list<Ast *>;
		#endif
	}
| 	all_expr {
		#if 0
		$$ = new list<Ast *>;
		#endif
	}
|	argumentList ',' var_const {
		#if 0
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($3);
		#endif
	}
|	argumentList ',' all_expr {
		#if 0
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($3);
		#endif
	}
;


assignment_statement:
	variable ASSIGN_OP var_const ';'
	{
		#if 0
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|
	variable ASSIGN_OP h_comparison_expr ';'
	{
	
		#if 0
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|
	variable ASSIGN_OP comparison_expr ';'	{
	
		#if 0
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|
	variable ASSIGN_OP arithmetic_expr ';'	{
		
		#if 0
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		#endif
	
	}
|
	variable ASSIGN_OP mul_div_expr ';'	{
		
		#if 0
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
		#endif
	}
;

equality_op
:	NE	{
		#if 0
		$$ = NE_OP; 
		#endif
	}
|	EQ	{
		#if 0
		$$ = EQ_OP; 
		#endif
	}
;

comparison_op
:	GE	{
		#if 0
		$$ = GE_OP; 
		#endif
	}
|	LE	{
		#if 0
		$$ = LE_OP; 
		#endif
	}
|	LT	{
		#if 0
		$$ = LT_OP; 
		#endif
	}
|	GT	{
		#if 0
		$$ = GT_OP; 
		#endif
	}
;

plus_minus
:	'+' {
		#if 0
		$$ = PLUS_OP;
		#endif
	}
| 	'-' {
		#if 0
		$$ = MINUS_OP;
		#endif
	}
;

mul_div
: 	'/' {
		#if 0
		$$ = DIV_OP;
		#endif
	}
|	'*' {
		#if 0
		$$ = MUL_OP;
		#endif
	}

;

typecast
:	'(' INTEGER ')' {
		#if 0
		$$ = int_data_type;
		#endif
	}
|	'(' FLOAT ')' {
		#if 0
		$$ = float_data_type;
		#endif
	}
|	'(' DOUBLE ')' {
		#if 0
		$$ = float_data_type;
		#endif
	}
;

h_comparison_expr
:	var_const equality_op var_const 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	var_const equality_op comparison_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	var_const equality_op arithmetic_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	var_const equality_op mul_div_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	comparison_expr equality_op var_const 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	comparison_expr equality_op comparison_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	comparison_expr equality_op arithmetic_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	comparison_expr equality_op mul_div_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	mul_div_expr equality_op var_const 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	mul_div_expr equality_op comparison_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	mul_div_expr equality_op arithmetic_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	mul_div_expr equality_op mul_div_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	arithmetic_expr equality_op comparison_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	arithmetic_expr equality_op var_const 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	arithmetic_expr equality_op arithmetic_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	arithmetic_expr equality_op mul_div_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	h_comparison_expr equality_op comparison_expr 	{
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	h_comparison_expr equality_op arithmetic_expr {
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	h_comparison_expr equality_op var_const {
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
|	h_comparison_expr equality_op mul_div_expr {
		#if 0
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		#endif
	}
;


comparison_expr
: var_const comparison_op var_const {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| var_const comparison_op mul_div_expr {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| var_const comparison_op arithmetic_expr {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| arithmetic_expr comparison_op var_const{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| arithmetic_expr comparison_op mul_div_expr{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| arithmetic_expr comparison_op arithmetic_expr{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| mul_div_expr comparison_op var_const{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| mul_div_expr comparison_op mul_div_expr{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| mul_div_expr comparison_op arithmetic_expr{
	//level2
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| comparison_expr comparison_op var_const {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| comparison_expr comparison_op mul_div_expr {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
| comparison_expr comparison_op arithmetic_expr {
	#if 0
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
;

mul_div_expr
:	var_const mul_div var_const{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	mul_div_expr mul_div var_const {
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
;


arithmetic_expr
:	var_const plus_minus var_const{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	var_const plus_minus mul_div_expr{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	mul_div_expr plus_minus var_const{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	mul_div_expr plus_minus mul_div_expr{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	arithmetic_expr plus_minus var_const{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
|	arithmetic_expr plus_minus mul_div_expr{
	#if 0
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	#endif
}
;

var_const:
	var_const_plain{
		#if 0
		$$ = $1;
		#endif
	}
|	typecast var_const_plain {
		#if 0
		$$ = new Typecast_Ast($1, $2);
		#endif
	}
|	'-' typecast var_const_plain {
		#if 0
		$$ = new Typecast_Ast($2, new UnaryMinus_Ast($3));
		#endif
	}
| '-' var_const_plain {
	#if 0
	$$ = new UnaryMinus_Ast($2);
	#endif
}
;


var_const_plain
:	variable {
	#if 0
	$$ = $1;
	#endif
}
|	constant {
	#if 0
	$$ = $1;
	#endif
}	
| function_call{
	
}
| '(' h_comparison_expr ')' {
	#if 0
	$$ = $2;
	#endif
}
| '(' comparison_expr ')' {
	#if 0
	$$ = $2;
	#endif
}
| '(' arithmetic_expr ')' {
	#if 0
	$$ = $2;
	#endif
}
| '(' mul_div_expr ')' {
	#if 0
	$$ = $2;
	#endif
}
| '(' var_const ')' {
	#if 0
	$$ = $2;
	#endif
}
;

return_stmt
:	RETURN {
		#if 0
		$$ = new Return_Ast();
		#endif
	}
|	RETURN var_const {
		#if 0
		$$ = new Return_Ast();
		#endif
}
|	RETURN all_expr {
		#if 0
		$$ = new Return_Ast();
		#endif
}
;

all_expr
:	mul_div_expr {
		#if 0
		$$ = $1;
		#endif
	}
|	arithmetic_expr {
		#if 0
		$$ = $1;
		#endif
	}
|	comparison_expr {
		#if 0
		$$ = $1;
		#endif
	}
|	h_comparison_expr {
		#if 0
		$$ = $1;
		#endif
	}
;

variable:
	NAME
	{
		#if 0
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
		#endif
	}
;

constant:
	INTEGER_NUMBER
	{
		#if 0
		$$ = new Number_Ast<int>($1, int_data_type);
		#endif	
	}
|	
	FLOAT_NUMBER
	{
		#if 0
		$$ = new Number_Ast<float>($1, float_data_type);	
		#endif
	}
;
