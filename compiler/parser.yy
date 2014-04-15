
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
	pair<Data_Type, string> * decl;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Comparison_Op_Enum comparison_op_enum;
	Data_Type data_type;
	Procedure * procedure;
	Arith_Op_Enum arith_op_enum;
};

%token <integer_value> INTEGER_NUMBER BBNUM
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token RETURN
%token INTEGER FLOAT DOUBLE VOID
%token IF ELSE GOTO
%token ASSIGN_OP NE EQ LT LE GT GE

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> parameter_list
%type <symbol_entry> parameter
%type <symbol_table> variable_declaration_list
%type <symbol_entry> variable_declaration
%type <ast_list> assignment_or_function_call_list
%type <decl> declaration
%type <ast_list> argumentList
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast> assignment_statement
%type <ast> var_const
%type <ast> function_call
%type <ast> return_stmt
%type <ast> var_const_plain
%type <ast> variable
%type <ast> constant

%type <comparison_op_enum> equality_op
%type <comparison_op_enum> comparison_op
%type <data_type> typecast
%type <arith_op_enum> plus_minus
%type <arith_op_enum> mul_div
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
	variable_declaration_list procedure_declaration_list {
	if(NOT_ONLY_PARSE){
		program_object.set_global_table(*$1);
	}
	}
	procedure_definition_list {
	if(NOT_ONLY_PARSE){
		check_undefined_functions();
	}
	}
|
	procedure_declaration_list procedure_definition_list {
	if(NOT_ONLY_PARSE){
		check_undefined_functions();
	}
	}
|
	variable_declaration_list {
	if(NOT_ONLY_PARSE){
		program_object.set_global_table(*$1);
	}
	} 
	procedure_definition_list {
	if(NOT_ONLY_PARSE){
		check_undefined_functions();
	}
	}
|
	procedure_definition_list {
	if(NOT_ONLY_PARSE){
		check_undefined_functions();
	}
	}
;

procedure_declaration_list:
	procedure_declaration
|	
	procedure_declaration_list procedure_declaration
;

procedure_declaration:
	INTEGER  procedure_name_decl ';' {
	if (NOT_ONLY_PARSE)
	{
		current_procedure->set_data_type(int_data_type);
	}
	}
|
	FLOAT procedure_name_decl ';' {
	if (NOT_ONLY_PARSE)
	{
		current_procedure->set_data_type(float_data_type);
	}
	}
|
	DOUBLE procedure_name_decl ';' {
	if (NOT_ONLY_PARSE)
	{
		current_procedure->set_data_type(float_data_type);
	}
	}
|
	VOID procedure_name_decl ';' {
	if (NOT_ONLY_PARSE)
	{
		current_procedure->set_data_type(void_data_type);
	}
	}
;

procedure_name_decl:
	NAME '(' {	
			if (NOT_ONLY_PARSE)
			{
			if(program_object.variable_in_symbol_list_check(*$1)){
				CHECK_INPUT_AND_ABORT(false,"Procedure name cannot be same as global variable", get_line_number());
			}
			current_procedure = new Procedure(void_data_type, *$1, get_line_number());
			program_object.set_procedure_map(current_procedure, get_line_number());
			}
		}
	parameter_list ')'
	{

		if (NOT_ONLY_PARSE)
		{
		current_procedure->set_arg_list(*$4);
		}
	}
|
	NAME '(' ')' {
		if (NOT_ONLY_PARSE)
		{
		if(program_object.variable_in_symbol_list_check(*$1)){
			CHECK_INPUT_AND_ABORT(false,"Procedure name cannot be same as global variable", get_line_number());
		}
		if(program_object.get_procedure_map(*$1) == NULL){
			current_procedure = new Procedure(void_data_type, *$1, get_line_number());
			current_procedure->set_arg_list(*(new Symbol_Table()));
			program_object.set_procedure_map(current_procedure, get_line_number());
		}
		}
	}
;

parameter_list
:	parameter {
		if (NOT_ONLY_PARSE)
		{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			CHECK_INPUT_AND_ABORT(false,"Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
		current_procedure->addArg($1->get_variable_name());
		}
	}
|	parameter_list ',' parameter {
		if (NOT_ONLY_PARSE)
		{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($3->get_variable_name(), line);

		string var_name = $3->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			CHECK_INPUT_AND_ABORT(false,"Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				CHECK_INPUT_AND_ABORT(false,"Formal Parameter declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($3);
		current_procedure->addArg($3->get_variable_name());
		}
	}
;

parameter:
	INTEGER NAME {
		if (NOT_ONLY_PARSE)
		{
		$$ = new Symbol_Table_Entry(*$2, int_data_type, get_line_number());
		delete $2;
		}
	}
|
	FLOAT NAME {
		if (NOT_ONLY_PARSE)
		{
		$$ = new Symbol_Table_Entry(*$2, float_data_type, get_line_number());
		delete $2;
		}
	}
|
	DOUBLE NAME {
		if (NOT_ONLY_PARSE)
		{
		$$ = new Symbol_Table_Entry(*$2, float_data_type, get_line_number());
		delete $2;
		}
	}
;


procedure_definition_list:
	procedure_definition_list procedure_definition
|
	procedure_definition
;

procedure_definition:
	procedure_name procedure_body
;


procedure_name:
	NAME '(' {	
			if (NOT_ONLY_PARSE)
			{
			if(program_object.variable_in_symbol_list_check(*$1)){
				CHECK_INPUT_AND_ABORT(false,"Procedure name cannot be same as global variable", get_line_number());
			}
			if(program_object.get_procedure_map(*$1) == NULL){
				CHECK_INPUT_AND_ABORT(false,"Procedure corresponding to the name is not found", get_line_number());
			}
			else{
				current_procedure = program_object.get_procedure_map(*$1);
			}
			}
		}
	parameter_list ')'
	{
		if (NOT_ONLY_PARSE)
		{
		current_procedure->match_arg_list(*$4, get_line_number());
		current_procedure->set_defined();
		return_statement_used_flag = false;
		}
	}
|
	NAME '(' ')' {
		if (NOT_ONLY_PARSE)
		{
		if(program_object.variable_in_symbol_list_check(*$1)){
			CHECK_INPUT_AND_ABORT(false,"Procedure name cannot be same as global variable", get_line_number());
		}
		if(program_object.get_procedure_map(*$1) == NULL){
			CHECK_INPUT_AND_ABORT(false,"Procedure corresponding to the name is not found", get_line_number());
		}
		else{
			current_procedure = program_object.get_procedure_map(*$1);
		}
		current_procedure->match_arg_list(*(new Symbol_Table()), get_line_number());
		current_procedure->set_defined();
		return_statement_used_flag = false;
		}
	}
;

procedure_body:
	'{' optional_variable_declaration_list
	{
		if(NOT_ONLY_PARSE){
		current_procedure->set_local_list(*$2);
		delete $2;
		}
	
	}
	basic_block_list '}'
	{
		
		if(NOT_ONLY_PARSE){
		if (return_statement_used_flag == false && current_procedure->get_return_type() != void_data_type)
		{
			int line = get_line_number();
			CHECK_INPUT_AND_ABORT(false, "Return type of procedure and its prototype should match", line);
		}

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
			CHECK_INPUT_AND_ABORT(false, error, -1);
		}
		
		current_procedure->set_basic_block_list(*$4);

		delete $4;
		}
	}
;




optional_variable_declaration_list:
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = new Symbol_Table();
		//$$ = NULL;
	}
	}
|
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

		$$ = $1;
	}
	}
;

variable_declaration_list:
	variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table_Entry * decl_stmt = $1;

		CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

		string decl_name = decl_stmt->get_variable_name();
		CHECK_INPUT_AND_ABORT ((program_object.variable_in_proc_map_check(decl_name, get_line_number()) == false),
				"Variable name cannot be same as the procedure name", get_line_number());

		if (current_procedure != NULL)
		{
			CHECK_INPUT_AND_ABORT((current_procedure->get_proc_name() != decl_name),
				"Variable name cannot be same as procedure name", get_line_number());
		}

		Symbol_Table * decl_list = new Symbol_Table();
		decl_list->push_symbol(decl_stmt);

		$$ = decl_list;
	}
	}
|
	variable_declaration_list variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		Symbol_Table_Entry * decl_stmt = $2;
		Symbol_Table * decl_list = $1;

		CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");
		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

		string decl_name = decl_stmt->get_variable_name();
		CHECK_INPUT_AND_ABORT((program_object.variable_in_proc_map_check(decl_name, get_line_number()) == false),
			"Procedure name cannot be same as the variable name", get_line_number());
		if (current_procedure != NULL)
		{
			CHECK_INPUT_AND_ABORT((current_procedure->get_proc_name() != decl_name),
				"Variable name cannot be same as procedure name", get_line_number());
		}

		CHECK_INPUT_AND_ABORT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
				"Variable is declared twice", get_line_number());

		decl_list->push_symbol(decl_stmt);
		$$ = decl_list;
	}
	}
;

variable_declaration:
	declaration ';'
	{
	if (NOT_ONLY_PARSE)
	{
		pair<Data_Type, string> * decl_stmt = $1;

		CHECK_INVARIANT((decl_stmt != NULL), "Declaration cannot be null");

		Data_Type type = decl_stmt->first;
		string decl_name = decl_stmt->second;

		Symbol_Table_Entry * decl_entry = new Symbol_Table_Entry(decl_name, type, get_line_number());

		$$ = decl_entry;

	}
	}
;

declaration:
	INTEGER NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

		string name = *$2;
		Data_Type type = int_data_type;

		pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

		$$ = declar;
	}
	}
|	FLOAT NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

		string name = *$2;
		Data_Type type = float_data_type;

		pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

		$$ = declar;
	}
	}
;

basic_block_list:
	basic_block_list basic_block
	{
	if (NOT_ONLY_PARSE)
	{
		list<Basic_Block *> * bb_list = $1;
		Basic_Block * bb = $2;

		CHECK_INVARIANT((bb_list != NULL), "New basic block cannot be null");
		CHECK_INVARIANT((bb != NULL), "Basic block cannot be null");

		bb_strictly_increasing_order_check(bb_list, bb->get_bb_number());

		bb_list->push_back($2);
		$$ = bb_list;
	}
	}
|
	basic_block
	{
	if (NOT_ONLY_PARSE)
	{
		Basic_Block * bb = $1;

		CHECK_INVARIANT((bb != NULL), "Basic block cannot be null");

		list<Basic_Block *> * bb_list = new list<Basic_Block *>;
		bb_list->push_back(bb);

		$$ = bb_list;
	}
	}
;

basic_block:
	BBNUM ':' executable_statement_list
	{
	if (NOT_ONLY_PARSE)
	{
		int bb_number = $1;
		list<Ast *> * exe_stmt = $3;

		CHECK_INPUT_AND_ABORT((bb_number >= 2), "Illegal basic block lable", get_line_number());

		Basic_Block * bb = new Basic_Block(bb_number, get_line_number());

		if (exe_stmt != NULL)
			bb->set_ast_list(*exe_stmt);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			bb->set_ast_list(*ast_list);
		}

		$$ = bb;
		bb_made.insert($1);
	}
	}
;

executable_statement_list:
	assignment_or_function_call_list
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	assignment_or_function_call_list return_stmt ';'
	{
	if (NOT_ONLY_PARSE)
	{
		list<Ast *> * assign_list = $1;
		list<Ast *> * exe_list = NULL;

		if (assign_list)
			exe_list = assign_list;

		else
			exe_list = new list<Ast *>;

		exe_list->push_back($2);

		$$ = exe_list;
	}
	}
|	assignment_or_function_call_list goto_statement ';'
	{
	
		if (NOT_ONLY_PARSE)
	{

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		}
	}
|
	assignment_or_function_call_list if_statement
	{
		if (NOT_ONLY_PARSE)
	{
		
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		}
	}
;


assignment_or_function_call_list:
	{
		if (NOT_ONLY_PARSE)
	{
		$$ = NULL;
		}
	}
|
	assignment_or_function_call_list assignment_statement
	{
		if (NOT_ONLY_PARSE)
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		}
	}
|
	assignment_or_function_call_list function_call ';'
	{
		if (NOT_ONLY_PARSE)
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		}
	}
;

function_call
:	NAME '(' argumentList ')' {
	if(NOT_ONLY_PARSE){
		Procedure * called_proc = program_object.get_procedure_map(*$1);
		if(called_proc == NULL){
			CHECK_INPUT_AND_ABORT(false,"Procedure corresponding to the name is not found",get_line_number());
		}
		function_call_set.insert(*$1);
		if(called_proc->get_arg_string_list().size() != $3->size()){
			CHECK_INPUT_AND_ABORT(false,"Actual and formal parameter count do not match",get_line_number());
		}
		if(!called_proc->match_arg_types($3)){
			CHECK_INPUT_AND_ABORT(false,"Actual and formal parameters data types are not matching", get_line_number());
		}
		$$ = new function_call_Ast(*$1,$3);
	}
	}
|	NAME '(' ')' {
	if(NOT_ONLY_PARSE){
		Procedure * called_proc = program_object.get_procedure_map(*$1);
		if(called_proc == NULL){
			CHECK_INPUT_AND_ABORT(false,"Procedure corresponding to the name is not found",get_line_number());
		}
		function_call_set.insert(*$1);
		if(called_proc->get_arg_string_list().size() != 0){
			CHECK_INPUT_AND_ABORT(false,"Actual and formal parameter count do not match",get_line_number());
		}
		$$ = new function_call_Ast(*$1,new list<Ast *>);
	}
	}
;

argumentList
:	var_const {
		if (NOT_ONLY_PARSE)
	{
		$$ = new list<Ast *>;
		$$->push_back($1);
		}
	}
| 	all_expr {
		if (NOT_ONLY_PARSE)
	{
		$$ = new list<Ast *>;
		$$->push_back($1);
		}
	}
|	argumentList ',' var_const {
		if (NOT_ONLY_PARSE)
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;
		else
			$$ = $1;

		$$->push_back($3);
		}
	}
|	argumentList ',' all_expr {
		if (NOT_ONLY_PARSE)
	{
		if ($1 == NULL)
			$$ = new list<Ast *>;
		else
			$$ = $1;

		$$->push_back($3);
		}
	}
;

assignment_statement:
	variable ASSIGN_OP var_const ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * lhs = $1;
		Ast * rhs = $3;

		Ast * assign = new Assignment_Ast(lhs, rhs, get_line_number());

		assign->check_ast();

		$$ = assign;
	}
	}
|
	variable ASSIGN_OP h_comparison_expr ';'
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * assign = new Assignment_Ast($1, $3,get_line_number());

		assign->check_ast();
		$$ = assign;
	}
	}
|
	variable ASSIGN_OP comparison_expr ';'
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * assign = new Assignment_Ast($1, $3,get_line_number());
		assign->check_ast();
		$$ = assign;
	}
	}
|
	variable ASSIGN_OP arithmetic_expr ';'
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * assign = new Assignment_Ast($1, $3,get_line_number());
		assign->check_ast();
		$$ = assign;
	}
	}
|
	variable ASSIGN_OP mul_div_expr ';'
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * assign = new Assignment_Ast($1, $3,get_line_number());
		assign->check_ast();
		$$ = assign;
	}
	}
;

if_statement:
	IF '(' h_comparison_expr ')' GOTO BBNUM ';' ELSE GOTO BBNUM ';'{
		
		if (NOT_ONLY_PARSE)
	{
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10,get_line_number());
		}
	}
|
	IF '(' comparison_expr ')' GOTO BBNUM ';' ELSE GOTO BBNUM ';'{
	
		if (NOT_ONLY_PARSE)
	{
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10,get_line_number());
		}
	}
|	IF '(' var_const ')' GOTO BBNUM ';' ELSE GOTO BBNUM ';'{
	
		if (NOT_ONLY_PARSE)
	{
		bb_requested.push_back($6);
		bb_requested.push_back($10);
		$$ = new If_Ast($3,$6,$10,get_line_number());
		}
	}
;

goto_statement
:	GOTO BBNUM {
		
		if (NOT_ONLY_PARSE)
	{
		$$ = new Goto_Ast($2,get_line_number());
		bb_requested.push_back($2);
		}
	}
;

plus_minus
:	'+' {
		if (NOT_ONLY_PARSE){
		$$ = PLUS_OP;
		}
	}
| 	'-' {
		if (NOT_ONLY_PARSE){
		$$ = MINUS_OP;
		}
	}
;

mul_div
: 	'/' {
		if (NOT_ONLY_PARSE){
		$$ = DIV_OP;
		}
	}
|	'*' {
		if (NOT_ONLY_PARSE){
		$$ = MUL_OP;
		}
	}

;

typecast
:	'(' INTEGER ')' {
		if (NOT_ONLY_PARSE){
		$$ = int_data_type;
		}
	}
|	'(' FLOAT ')' {
		if (NOT_ONLY_PARSE){
		$$ = float_data_type;
		}
	}
|	'(' DOUBLE ')' {
		if (NOT_ONLY_PARSE){
		$$ = float_data_type;
		}
	}
;

h_comparison_expr
:	var_const equality_op var_const 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	var_const equality_op comparison_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	var_const equality_op arithmetic_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	var_const equality_op mul_div_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	comparison_expr equality_op var_const 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	comparison_expr equality_op comparison_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	comparison_expr equality_op arithmetic_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	comparison_expr equality_op mul_div_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	mul_div_expr equality_op var_const 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	mul_div_expr equality_op comparison_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	mul_div_expr equality_op arithmetic_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	mul_div_expr equality_op mul_div_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	arithmetic_expr equality_op comparison_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	arithmetic_expr equality_op var_const 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	arithmetic_expr equality_op arithmetic_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	arithmetic_expr equality_op mul_div_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	h_comparison_expr equality_op comparison_expr 	{
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	h_comparison_expr equality_op arithmetic_expr {
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	h_comparison_expr equality_op var_const {
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
|	h_comparison_expr equality_op mul_div_expr {
		if (NOT_ONLY_PARSE){
		$$ = new Comparison_Ast($1,$2,$3,get_line_number());
		$$->check_ast();
		}
	}
;


comparison_expr
: var_const comparison_op var_const {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| var_const comparison_op mul_div_expr {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| var_const comparison_op arithmetic_expr {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| arithmetic_expr comparison_op var_const{
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| arithmetic_expr comparison_op mul_div_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| arithmetic_expr comparison_op arithmetic_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| mul_div_expr comparison_op var_const{
	//level2
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| mul_div_expr comparison_op mul_div_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| mul_div_expr comparison_op arithmetic_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| comparison_expr comparison_op var_const {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| comparison_expr comparison_op mul_div_expr {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
| comparison_expr comparison_op arithmetic_expr {
	if (NOT_ONLY_PARSE){
	$$ = new Comparison_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
;


mul_div_expr
:	var_const mul_div var_const{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	mul_div_expr mul_div var_const {
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
;


arithmetic_expr
:	var_const plus_minus var_const{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	var_const plus_minus mul_div_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	mul_div_expr plus_minus var_const{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	mul_div_expr plus_minus mul_div_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	arithmetic_expr plus_minus var_const{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
|	arithmetic_expr plus_minus mul_div_expr{
	if (NOT_ONLY_PARSE){
	$$ = new Arithmetic_Ast($1,$2,$3,get_line_number());
	$$->check_ast();
	}
}
;


var_const:
	var_const_plain{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|	typecast var_const_plain {
	if (NOT_ONLY_PARSE){
		$$ = new Typecast_Ast($1, $2);
	}
	}
|	'-' typecast var_const_plain {
	if (NOT_ONLY_PARSE){
		$$ = new Typecast_Ast($2, new UnaryMinus_Ast($3));
	}
	}
;

var_const_plain
:	variable {
	if (NOT_ONLY_PARSE)
	{
	$$ = $1;
	}
}
|	constant {
	if (NOT_ONLY_PARSE)
	{
	$$ = $1;
	}
}
|	function_call {
	if (NOT_ONLY_PARSE)
	{
	$$ = $1;
	}
}
| '(' all_expr ')' {
	if (NOT_ONLY_PARSE)
	{
	$$ = $2;
	}
}
| '(' var_const ')' {
	if (NOT_ONLY_PARSE)
	{
	$$ = $2;
	}
	}
| '-' var_const_plain {
	if (NOT_ONLY_PARSE){
		$$ = new UnaryMinus_Ast($2);
	}
	}
;

return_stmt
:	RETURN {
		if(NOT_ONLY_PARSE)
		{
		if(current_procedure->get_return_type() == void_data_type){
			return_statement_used_flag = true;
		}
		check_return_data_types(void_data_type,current_procedure->get_return_type());
		$$ = new Return_Ast(NULL,current_procedure->get_proc_name(), get_line_number());
		}
	}
|	RETURN var_const {
		if(NOT_ONLY_PARSE)
		{
		if(current_procedure->get_return_type() == $2->get_data_type()){
			return_statement_used_flag = true;
		}
		check_return_data_types($2->get_data_type(),current_procedure->get_return_type());
		$$ = new Return_Ast($2,current_procedure->get_proc_name(), get_line_number());
		}
}
|	RETURN all_expr {
		if(NOT_ONLY_PARSE)
		{
		if(current_procedure->get_return_type() == $2->get_data_type()){
			return_statement_used_flag = true;
		}
		check_return_data_types($2->get_data_type(),current_procedure->get_return_type());
		$$ = new Return_Ast($2,current_procedure->get_proc_name(), get_line_number());
		}
}
;

all_expr
:	mul_div_expr {
	if (NOT_ONLY_PARSE){
		$$ = $1;
	}
	}
|	arithmetic_expr {
	if (NOT_ONLY_PARSE){
		$$ = $1;
	}
	}
|	comparison_expr {
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|	h_comparison_expr {
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

variable:
	NAME
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table_Entry * var_table_entry;

		CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

		string var_name = *$1;

		if (current_procedure->variable_in_symbol_list_check(var_name) == true || current_procedure->variable_in_arg_list_check(var_name) == true)
			 var_table_entry = &(current_procedure->get_symbol_table_entry(var_name));

		else if (program_object.variable_in_symbol_list_check(var_name) == true)
			var_table_entry = &(program_object.get_symbol_table_entry(var_name));

		else
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared");

		Ast * name_ast = new Name_Ast(var_name, *var_table_entry, get_line_number());
		$$ = name_ast;
	}
	}
;

constant:
	INTEGER_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		int num = $1;

		Ast * num_ast = new Number_Ast<int>(num, int_data_type, get_line_number());

		$$ = num_ast;
	}
	}
|	FLOAT_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		float num = $1;

		Ast * num_ast = new Number_Ast<float>(num, float_data_type, get_line_number());

		$$ = num_ast;
	}
	}
;

equality_op
:	NE	{
		if (NOT_ONLY_PARSE){
		$$ = NE_OP; 
		}
	}
|	EQ	{
		if (NOT_ONLY_PARSE){
		$$ = EQ_OP; 
		}
	}
;

comparison_op
:	GE	{
		if (NOT_ONLY_PARSE){
		$$ = GE_OP; 
		}
	}
|	LE	{
		if (NOT_ONLY_PARSE){
		$$ = LE_OP; 
		}
	}
|	LT	{
		if (NOT_ONLY_PARSE){
		$$ = LT_OP; 
		}
	}
|	GT	{
		if (NOT_ONLY_PARSE){
		$$ = GT_OP; 
		}
	}
;