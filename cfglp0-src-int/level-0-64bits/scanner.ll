
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

%filenames="scanner"
%lex-source="scanner.cc"



%%

int		{
			store_token_name("INTEGER");
			return Parser::INTEGER; 
		}

return		{ 
			store_token_name("RETURN");
			return Parser::RETURN; 
		}

do 		{
			return Parser::DO;
		}
while	{
			return Parser::WHILE;
		}
for		{
			return Parser::FOR;
		}
if		{
			return Parser::IF;
		}
else	{
			return Parser::ELSE;
		}

goto	{
			return Parser::GOTO;
		}

"||"|"&&"	{
			std::cout<<"hey there"<<std::endl;
			return Parser::BINARY_LOGICAL_OPERATOR;
		}

[<>:{}();=!]	{
			store_token_name("META CHAR");
			return matched()[0];
		}


		

[-]?[[:digit:]_]+ 	{ 
				store_token_name("NUM");

				ParserBase::STYPE__ * val = getSval();
				val->integer_value = atoi(matched().c_str());

				return Parser::INTEGER_NUMBER; 
			}

[[:alpha:]_][[:alpha:][:digit:]_]* {
					store_token_name("NAME");

					ParserBase::STYPE__ * val = getSval();
					val->string_value = new std::string(matched());

					return Parser::NAME; 
				}

\n		{ 
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}    

";;".*  	|
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			int line_number = lineNr();
			report_error(error_message, line_number);
		}
