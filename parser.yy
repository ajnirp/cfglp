
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented by Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors (theory and lab)  at IIT
           Bombay.

           Release  date Jan  15, 2013.  Copyrights reserved  by Uday
           Khedker. This implemenation has been made available purely
           for academic purposes without any warranty of any kind.

           A  doxygen   generated  documentation  can  be   found  at
           http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	
	int integer_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
}


%token <integer_value> INTEGER_NUMBER
%token <string_value> NAME
%token RETURN INTEGER 

%token IF ELSE GOTO

%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast> basic_block_body
%type <ast_list> statement_list
%type <ast> statement
%type <ast> expression
%type <ast> comparison_op
%type <ast> comparison_expr
%type <ast> assignment_expr
%type <ast> variable
%type <ast> constant

%right '<' '>'
%left '='

%start program

%%

program:
	declaration_statement_list procedure_name
	{	
	
	#if 0
		program_object.set_global_table(*$1);
		return_statement_used_flag = false;				// No return statement in the current procedure till now
	#endif
	}
	procedure_body
	{
		
	#if 0
		program_object.set_procedure_map(*current_procedure);

		if ($1)
			$1->global_list_in_proc_map_check(get_line_number());

		delete $1;
	#endif
	}
|
	procedure_name
	{
	#if 0
		return_statement_used_flag = false;				// No return statement in the current procedure till now
	#endif
	}
	procedure_body
	{
		#if 0
		program_object.set_procedure_map(*current_procedure);
		#endif
	}
;

procedure_name:
	NAME '(' ')'
	{
		#if 0
		current_procedure = new Procedure(void_data_type, *$1);
		#endif
	}
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
;

basic_block_list:
	basic_block basic_block_list
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
	'<' NAME INTEGER_NUMBER '>' ':' basic_block_body
	{	
	#if 0
		if (*$2 != "bb")
		{
			int line = get_line_number();
			report_error("Not basic block lable", line);
		}

		if ($3 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}

		if ($6 != NULL)
			$$ = new Basic_Block($3, *$6);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($3, *ast_list);
		}

		delete $6;
		delete $2;
		#endif
	}
;

basic_block_body:
	statement_list RETURN ';'
|	statement_list
|	RETURN ';'
;

statement_list:
	statement statement_list
	{	
	#if 0
		$$ = $1;
		#endif
	}
|
	statement
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
;

statement
:	goto_statement
|	IF '(' comparison_expr ')' goto_statement ELSE goto_statement
|	expression ';'
;

expression
:	assignment_expr
|   comparison_expr
|	variable
|	constant
|	'(' expression ')'
;

comparison_op
:	'!''='
|	'>''='
|	'<''='
|	'=''='
|	'<'
|	'>'
;

comparison_expr
:	variable comparison_op variable
|	variable comparison_op constant
;

assignment_expr
:	variable '=' expression
;

goto_statement
:	GOTO '<' NAME INTEGER_NUMBER '>' ';'
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
;
