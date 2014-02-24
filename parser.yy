
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
%token INTEGER FLOAT DOUBLE
%token IF ELSE GOTO
%token ASSIGN_OP NE EQ LT LE GT GE

%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
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
%type <ast> goto_statement
%type <ast> if_statement
%type <ast> mul_div_expr
%type <ast> arithmetic_expr


%start program

%%

program:
	declaration_statement_list procedure_name
	{
		
		program_object.set_global_table(*$1);
		return_statement_used_flag = false;				// No return statement in the current procedure till now
		
	}
	procedure_body
	{
		
		int i = 0;
		while(i < bb_requested.size() && bb_made.find(bb_requested[i]) != bb_made.end()){
			i++;
		}

		if(i < bb_requested.size()){
			//error
			stringstream ss;
			ss << bb_requested[i];
			string error = "bb ";
			error += ss.str();
			error += " doesn't exist";
			report_error(error, -1);
		}

		program_object.set_procedure_map(*current_procedure);

		if ($1)
			$1->global_list_in_proc_map_check(get_line_number());

		delete $1;
		
	}
|
	procedure_name
	{
		
		return_statement_used_flag = false;				// No return statement in the current procedure till now
		
	}
	procedure_body
	{	
		
		int i = 0;
		while(i < bb_requested.size() && bb_made.find(bb_requested[i]) != bb_made.end()){
			i++;
		}

		if(i < bb_requested.size()){
			//error
			stringstream ss;
			ss << bb_requested[i];
			string error = "bb ";
			error += ss.str();
			error += " doesn't exist";
			report_error(error, -1);
		}

		program_object.set_procedure_map(*current_procedure);
		
	}
;

procedure_name:
	NAME '(' ')'
	{
		
		current_procedure = new Procedure(void_data_type, *$1);
		
	}
;

procedure_body:
	'{' declaration_statement_list
	{
		
		current_procedure->set_local_list(*$2);
		delete $2;
		
	
	}
	basic_block_list '}'
	{
		
		
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$4);

		delete $4;
		
	}
|
	'{' basic_block_list '}'
	{
		
		
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}
		
		current_procedure->set_basic_block_list(*$2);

		delete $2;
		
	}
;

declaration_statement_list:
	declaration_statement
	{
		
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
		
	}
|
	declaration_statement_list declaration_statement
	{
		
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
		
	}
;

declaration_statement:
	INTEGER NAME ';'
	{
		
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
		
	
	}
|
	FLOAT NAME ';'
	{
		
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		
	
	}
|
	DOUBLE NAME ';'
	{
		
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		
	
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}
		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
		
	}
|
	basic_block
	{
		
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
		
	}
	
;


basic_block:
	BASIC_BLOCK ':' executable_statement_list
	{
		
		if ($3 != NULL){
			$$ = new Basic_Block($1, *$3);
		}
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}
		bb_made.insert($1);
		
	}
;

executable_statement_list:
	assignment_statement_list
	{
		
		$$ = $1;
		
	}
|
	assignment_statement_list RETURN ';'
	{
		
		Ast * ret = new Return_Ast();

		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back(ret);
		
	}
|
	assignment_statement_list goto_statement ';'
	{
	
		

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		
	}
|
	assignment_statement_list if_statement
	{
	
		
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		
	}
;

if_statement:
	IF '(' h_comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
			
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		
	}
|
	IF '(' comparison_expr ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		
	}
|	IF '(' var_const ')' GOTO BASIC_BLOCK ';' ELSE GOTO BASIC_BLOCK ';'{
	
		
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10);
		
	}
;
	

goto_statement
:	GOTO BASIC_BLOCK {
		

		$$ = new Goto_Ast($2);
		bb_requested.push_back($2);
		
	}
;

assignment_statement_list:
	{
		
		$$ = NULL;
		
	}
|
	assignment_statement_list assignment_statement
	{
		
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		
	}
;

assignment_statement:
	variable ASSIGN_OP var_const ';'
	{
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	variable ASSIGN_OP h_comparison_expr ';'	{
	
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	variable ASSIGN_OP comparison_expr ';'	{
	
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	variable ASSIGN_OP arithmetic_expr ';'	{
		
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	
	}
|
	variable ASSIGN_OP mul_div_expr ';'	{
		
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	
	}
;

equality_op
:	NE	{
		
		$$ = NE_OP; 
		
	}
|	EQ	{
		
		$$ = EQ_OP; 
		
	}
;

comparison_op
:	GE	{
		
		$$ = GE_OP; 
		
	}
|	LE	{
		
		$$ = LE_OP; 
		
	}
|	LT	{
		
		$$ = LT_OP; 
		
	}
|	GT	{
		
		$$ = GT_OP; 
		
	}
;

plus_minus
:	'+' {
		
		$$ = PLUS_OP;
		
	}
| 	'-' {
		
		$$ = MINUS_OP;
		
	}
;

mul_div
: 	'/' {
		
		$$ = DIV_OP;
		
	}
|	'*' {
		
		$$ = MUL_OP;
		
	}

;

typecast
:	'(' INTEGER ')' {
		
		$$ = int_data_type;
		
	}
|	'(' FLOAT ')' {
		
		$$ = float_data_type;
		
	}
|	'(' DOUBLE ')' {
		
		$$ = float_data_type;
		
	}
;

h_comparison_expr
:	var_const equality_op var_const 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	var_const equality_op comparison_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	var_const equality_op arithmetic_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	var_const equality_op mul_div_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	comparison_expr equality_op var_const 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	comparison_expr equality_op comparison_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	comparison_expr equality_op arithmetic_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	comparison_expr equality_op mul_div_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	mul_div_expr equality_op var_const 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	mul_div_expr equality_op comparison_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	mul_div_expr equality_op arithmetic_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	mul_div_expr equality_op mul_div_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	arithmetic_expr equality_op comparison_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	arithmetic_expr equality_op var_const 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	arithmetic_expr equality_op arithmetic_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	arithmetic_expr equality_op mul_div_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	h_comparison_expr equality_op comparison_expr 	{
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	h_comparison_expr equality_op arithmetic_expr {
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	h_comparison_expr equality_op var_const {
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|	h_comparison_expr equality_op mul_div_expr {
		
		$$ = new Comparison_Ast($1,$2,$3);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
;


comparison_expr
: var_const comparison_op var_const {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| var_const comparison_op mul_div_expr {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| var_const comparison_op arithmetic_expr {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| arithmetic_expr comparison_op var_const{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| arithmetic_expr comparison_op mul_div_expr{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| arithmetic_expr comparison_op arithmetic_expr{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| mul_div_expr comparison_op var_const{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| mul_div_expr comparison_op mul_div_expr{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| mul_div_expr comparison_op arithmetic_expr{
	//level2
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| comparison_expr comparison_op var_const {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| comparison_expr comparison_op mul_div_expr {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
| comparison_expr comparison_op arithmetic_expr {
	
	$$ = new Comparison_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
;

mul_div_expr
:	var_const mul_div var_const{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
|	mul_div_expr mul_div var_const {
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
;


arithmetic_expr
:	var_const plus_minus var_const{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
}
|	var_const plus_minus mul_div_expr{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
|	mul_div_expr plus_minus var_const{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
|	mul_div_expr plus_minus mul_div_expr{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
|	arithmetic_expr plus_minus var_const{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
|	arithmetic_expr plus_minus mul_div_expr{
	//level2
	// submission 3b
	$$ = new Arithmetic_Ast($1,$2,$3);
	int line = get_line_number();
	$$->check_ast(line);
	
}
;

var_const:
	var_const_plain{
		$$ = $1;
	}
|	typecast var_const_plain {
		$$ = new Typecast_Ast($1, $2);
	}
;


var_const_plain
:	variable {
	
	$$ = $1;
	
}
|	constant {
	
	$$ = $1;
	
}	
| '(' h_comparison_expr ')' {
	
	$$ = $2;
	
}
| '(' comparison_expr ')' {
	
	$$ = $2;
	
}
| '(' arithmetic_expr ')' {
	
	$$ = $2;
	
}
| '(' mul_div_expr ')' {
	
	$$ = $2;
	
}
| '-' var_const_plain {
	
	$$ = new UnaryMinus_Ast($2);
	
}
| '(' var_const ')' {
	
	$$ = $2;
	
}

;

variable:
	NAME
	{
		
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
		
	}
;

constant:
	INTEGER_NUMBER
	{
		$$ = new Number_Ast<int>($1, int_data_type);	
	}
|	
	FLOAT_NUMBER
	{
		$$ = new Number_Ast<float>($1, float_data_type);	
	}
;
