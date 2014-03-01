removed from parser.yy


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